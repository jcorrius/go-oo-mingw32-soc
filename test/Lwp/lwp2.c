#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

static FILE *in;

typedef unsigned char byte_t;

static long init_offset = 0x68;

static void
print_indent (int indent)
{
  int i;
  for (i = 0; i < indent; i++)
    fprintf( stderr, "  " );
}  

static void
dump_hex (unsigned char *data, int len, int indent)
{
  while (len > 0) {
    int chunk = len < 16 ? len : 16;
    int i;
		
    print_indent (indent);
    for (i = 0; i < chunk; i++)
      fprintf( stderr, "%.2x ", data[i] );
    fprintf( stderr, "| " );
    for (i = 0; i < chunk; i++)
      fprintf( stderr, "%c", data[i] < 127 && data[i] > 30 ? data[i] : '.' );
    fprintf( stderr, "\n" );

    len -= chunk;
    data += 16;
  }
}

static void
dump_at (FILE *in)
{
  int i = 0;
  byte_t frame = 0x01;
  byte_t data[65536] = { 0, };
  byte_t last = 0;
  int indent = 0;

  while (!feof(in)) {
    data[i] = fgetc (in);
    if (data[i] == '@') {
      fprintf( stderr, "Field length 0x%x\n", i );
      dump_hex (data, i, 0);
      data[0] = data[i];
      i = 0;
    }
    i++;
  }
}

int
main (int argc, char **argv)
{
  int i = 0;
  byte_t frame = 0x01;
  byte_t data[655360] = { 0, };

  in = fopen(argv[1], "r");

  if (argc > 2)
    init_offset = atoi (argv[2]);

  fseek (in, init_offset, SEEK_SET);

  while (!feof (in)) {
    byte_t t = fgetc (in);
    if (t < 8) {
      dump_hex (data, i, data[0]);
      i = 0;
    }
    data[i++] = t;
  }
  fclose (in);
  return 0;
} 
