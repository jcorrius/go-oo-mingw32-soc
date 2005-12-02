/*
 * Compile with:
 *     gcc -Wall -lbfd -o redirect redirect.c
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "bfd.h"
/* #include "elf/common.h" */

#ifndef DT_DIRECT_VAGUE
#  define DT_DIRECT_VAGUE 0x8000
#endif

static int removed_count = 0;

static int
redirect_symbols (bfd *abfd, asection *direct_sec, bfd_byte *contents,
		  int (*match_fn) (const char *))
{
  asymbol **sy = NULL;
  long storage, i;
  long dynsymcount;

  if ((storage = bfd_get_dynamic_symtab_upper_bound (abfd)) <= 0)
    {
      fprintf (stderr, "no dynsym section\n");
      return 1;
    }
  sy = malloc (storage);

  dynsymcount = bfd_canonicalize_dynamic_symtab (abfd, sy);
  if (dynsymcount < 0 || !sy)
    {
      fprintf (stderr, "failed to canonicalise dymsym\n");
      return 1;
    }
  
  if (dynsymcount + 1 != (long) bfd_section_size (abfd, direct_sec) / 2)
    {
      fprintf (stderr, ".direct section size mismatch %d != %d\n",
	       (int) dynsymcount + 1, (int) bfd_section_size (abfd, direct_sec) / 2);
      return 1;
    }

  sy--; /* 1st dynsym slot is blank with no symbol */

  for (i = 1; i < dynsymcount + 1; i++)
    {
      const char *sym_name;
      sym_name = bfd_asymbol_name (sy[i]);
      if (match_fn (sym_name))
        {
	  unsigned short entry;
	  entry = bfd_get_16 (abfd, contents + i * 2);
/*	  fprintf (stderr, "Masking 0x%x for '%s'\n", entry, sym_name); */
	  if (entry & DT_DIRECT_VAGUE)
            {
	      entry &= ~DT_DIRECT_VAGUE;
	      bfd_put_16 (abfd, entry, contents + i * 2);
	      removed_count++;
	    }
	}
//      fprintf (stderr, "Symbol '%s'\n", bfd_asymbol_name (sy[i]));
    }
  free (sy + 1);

  return 0;
}

static int
write_section (const char *lib,
	       unsigned long sec_offset,
	       bfd_byte *new_direct,
	       unsigned long sec_size)
{
  FILE *outp;
  outp = fopen (lib, "r+b");
  if (!outp)
    return 1;

  fseek (outp, sec_offset, SEEK_SET);
/*  fprintf (stderr, "write 0x%x bytes to 0x%x\n", sec_size, sec_offset); */
  fwrite (new_direct, 1, sec_size, outp);

  fclose (outp);

  return 0;
}


static int
zthb_match_fn (const char *str)
{
  if (str && !strncmp (str, "_ZThn", sizeof ("_ZThn") - 1))
    return 1;
  else
    return 0;
}

int
main (int argc, char **argv)
{
  bfd *abfd;
  const char *lib = argv [ argc - 1];
  asection *direct_sec;
  unsigned long sec_offset;
  unsigned long sec_size;
  bfd_byte *new_direct;
  
  abfd = bfd_openr (lib, NULL);
  if (!abfd)
  {
    fprintf (stderr, "Failed to open '%s'\n", lib);
    return 1;
  }
  else
    fprintf (stderr, "Opened '%s'\n", bfd_get_filename (abfd));

  if (!bfd_check_format_matches (abfd, bfd_object, NULL))
    fprintf (stderr, "Format doesn't match\n");

  if (!(bfd_get_file_flags (abfd) & DYNAMIC))
    {
      fprintf (stderr, "not a dynamic object\n");
      return 0;
    }

  if (!(direct_sec = bfd_get_section_by_name (abfd, ".direct")))
  {
    fprintf (stderr, "No direct section\n");
    return 0;
  }

  if (!bfd_malloc_and_get_section (abfd, direct_sec, &new_direct))
  {
    fprintf (stderr, "Failed to read direct section\n");
    return 0;
  }

  if (redirect_symbols (abfd, direct_sec, new_direct, zthb_match_fn))
  {
    fprintf (stderr, "Error re-writing direct section\n");
    return 0;
  }

  sec_offset = (unsigned long) direct_sec->filepos;
  sec_size = (unsigned long) bfd_section_size (abfd, direct_sec);

  bfd_close (abfd);

  /* Now the dodgy re-writing lark - nice race possible here ... */
  if (write_section (lib, sec_offset, new_direct, sec_size))
    fprintf (stderr, "Failed to write new direct section\n");
  
  free (new_direct);

  fprintf (stderr, "Saved %d vague lookups\n", removed_count);

  return 0;
}
