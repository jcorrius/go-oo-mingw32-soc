#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

static FILE *in;

typedef unsigned char byte_t;

static long init_offset = 0x2633;

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
						fprintf( stderr, "Field length %d\n", i );
						dump_hex (data, i, 0);
						data[0] = data[i];
						i = 0;
				}
				i++;
		}
}

static void
dump_simple (FILE *in)
{
		int i = 0;
		byte_t frame = 0x01;
		byte_t data[65536] = { 0, };
		byte_t last = 0;
		int indent = 0;

		while (!feof(in)) {
				data[i] = fgetc (in);
				if (data[i] == 1 ||
					data[i] == 2) {

						if (data[0] == 1 && last == 2)
								indent-=2;
						else if (data[0] == 2 && last == 1)
								indent+=2;

						print_indent (indent);
						fprintf( stderr, "Field 0x%.2x (last %.2x) [%d] length %d\n",
								 data[0], last, indent, i );
						dump_hex (data, i, indent + 1);
						last = data [0];
						data[0] = data[i];
						i = 0;
				}
				i++;
		}
}

// Odd section:
// 02 02 3f bf 62 65 20 6f 6e 20 68 69 73 20 66 69 | ..?.be on his fi
// 72 73 74 20 65 6e 74 65 72 69 6e 67 20 61 20 6e | rst entering a n
// 65 69 67 68 62 6f 75 72 68 6f 6f 64 2c 20 74 68 | eighbourhood, th
// 69 73 20 74 72 75 74 68 20 69 73 20 73 6f 20 77 | is truth is so w
// 65 6c 6c 01 c0 02 53 53 45 6e 64 00 00 94 02 2e | ell...SSEnd.....
// 00 02 6a c1 02 2f 41 02 2d 48 0f 61 01 6d 46 8d | ..j../A.-H.a.mF.
// 46 c3 01 f6 28 0e 41 01 96 40 01 42 | F...(.A..@.B

// #define is_ctrl(c) ((c) == 1 || (c) == 2)
#define is_ctrl(c) ((c) == '@')
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

#if 1
#  if 1
		dump_at (in);
#  else
		dump_simple (in);
#  endif
#else

		while (!feof(in)) {
				data[i] = fgetc (in);
				if (is_ctrl(data[i]) && i > 0 && is_ctrl(data[i - 1])) {
						fprintf (stderr, "Record %.2x %.2x\n",
								 data[0], data[1]);
						dump_hex (data, i - 1, 0);
						data[0] = data[i - 1];
						data[1] = data[i];
						i = 1;
				}
				i++;
		}
#endif
		fclose (in);
		return 0;
} 
