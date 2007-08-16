#include <windows.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int
main (int argc, char **argv)
{
  unsigned int new_v[4];
  int update_version = 0;
  DWORD version_info_size;
  DWORD dummy;
  unsigned char *buffer;
  VS_FIXEDFILEINFO *fixed_file_info;
  UINT fixed_file_info_len;

  if (argc == 4 &&
      strcmp (argv[1], "-s") == 0)
    {
      int i;

      if (sscanf (argv[2], "%u.%u.%u.%u",
		  &new_v[0],
		  &new_v[1],
		  &new_v[2],
		  &new_v[3]) != 4)
	{
	  fprintf (stderr, "Wrong new version format.\n");
	  exit (1);
	}

      for (i = 0; i < 4; i++)
	if (new_v[i] > 0xFFFF)
	  {
	    fprintf (stderr, "Wrong new version format.\n");
	    exit (1);
	  }

      update_version = 1;
      argc -= 2;
      argv += 2;
    }

  if (argc != 2)
    {
      fprintf (stderr, "Usage: fileversion [ -s a.b.c.d ] file\n");
      exit (1);
    }

  version_info_size = GetFileVersionInfoSize (argv[1], &dummy);

  buffer = malloc (version_info_size);

  if (!GetFileVersionInfo (argv[1], 0, version_info_size, buffer))
    exit (1);

  if (!VerQueryValue (buffer, "\\", (LPVOID*) &fixed_file_info, &fixed_file_info_len))
    exit (1);

  if (fixed_file_info_len < sizeof (*fixed_file_info))
    exit (1);

  if (update_version)
    {
      HANDLE resource;

      fixed_file_info->dwFileVersionMS = 0x10000 * new_v[0] + new_v[1];
      fixed_file_info->dwFileVersionLS = 0x10000 * new_v[2] + new_v[3];
      
      if (!(resource = BeginUpdateResource (argv[1], FALSE)))
	{
	  fprintf (stderr, "BeginUpdateResource() failed.\n");
	  exit (1);
	}
      if (!UpdateResource (resource,
			   RT_VERSION,
			   MAKEINTRESOURCE (VS_VERSION_INFO),
			   MAKELANGID (LANG_NEUTRAL, SUBLANG_NEUTRAL),
			   buffer,
			   version_info_size))
	{
	  fprintf (stderr, "UpdateResource() failed.\n");
	  exit (1);
	}
      if (!EndUpdateResource (resource, FALSE))
	{
	  fprintf (stderr, "EndUpdateResource() failed.\n");
	  exit (1);
	}
    }
  else
    printf ("%d.%d.%d.%d\n",
	    HIWORD (fixed_file_info->dwFileVersionMS),
	    LOWORD (fixed_file_info->dwFileVersionMS),
	    HIWORD (fixed_file_info->dwFileVersionLS),
	    LOWORD (fixed_file_info->dwFileVersionLS));

  exit (0);
}
