#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

static FILE *in;

typedef unsigned char byte_t;

static long init_offset = 0x2633;

static void
print_special (unsigned char c)
{
		fprintf( stderr, "<0x%x>", c);
}

static void
dump_string (FILE *fin, int char_len)
{
		byte_t datum;
		while (char_len > 0) {
				datum = fgetc(fin);
				if (isascii (datum)) {
						fprintf (stderr, "%c", datum);
						char_len--;
				} else switch (datum) {
				case 0x91:
				case 0x88:
				case 0x8f:
				case 0xc6:
						fprintf (stderr, "<magicnolen0x%x>", datum);
						break;
				case 0x82:
				case 0x8a:	
				case 0x8d:
				case 0x97:
				case 0xa9:
				case 0xb9:
				case 0xbd:
				case 0xbf:
						char_len--;
						fprintf (stderr, "<magic0x%x>", datum);
						break;
				default:
						fprintf (stderr, "Error: unknown code 0x%x\n", datum);
						print_special (datum);
						break;
				}
		}
		fprintf (stderr, "'\n");
}

static void
dump_hex (unsigned char *data, int len)
{
	while (len > 0) {
		int chunk = len < 16 ? len : 16;
		int i;
		
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

typedef enum {
		VALUE_STRING,
		VALUE_BIN
} ValueType;

static int
get_len (byte_t codea, byte_t codeb, ValueType *type)
{
#define MAP(_val,_len) \
		case _val: len = _len; break

		*type = VALUE_BIN;

		int len = -1;
		if (codea == 0x01) {
				switch (codeb) {
				MAP( 0x02, 8 ); /* SSEnd */
				MAP( 0x40, 0); MAP( 0x41, 0 ); MAP( 0x42, 0 );
				MAP( 0x6d, 4 );
				MAP( 0x96, 1 );
				MAP( 0xc0, 0 );
				MAP( 0xf6, 3 );
				default:
						break;
				}
		}
		if (codea == 0x02) {
				switch (codeb) {
				case 0x02:
						len = (byte_t) fgetc(in) + 1;
						*type = VALUE_STRING;
						break;
				case 0x04: 
				{ /* Almost certainly wrong */
						int i;
						for (i = 0; i < 10; i++) fgetc(in);
						len = (byte_t) fgetc(in) + 1;
						*type = VALUE_STRING;
						break;
				}
				case 0x20:
				case 0x26: case 0x27:
				case 0x28: case 0x29: case 0x2a: case 0x2b:
				case 0x2c: case 0x2d: case 0x2e: case 0x2f:
				case 0x30: case 0x31: case 0x32:
						len = 1;
						break;
				MAP( 0x53, 7 ); /* SSEnd */
				case 0x5c:
				case 0x60: case 0x62:
				case 0x64:
				case 0x6a:
				case 0x76:
						len = 1;
						break;
				default:
						fprintf( stderr, "Guess (A) length as 1 for 0x%x\n", codeb );
						len = 1;
						break;
				}
		}
		if (codea == 0x20 && codeb == 0x40)
				len = 0;

		if (codea >= 0x08 && codea <= 0x11 &&
			codeb == 0x61)
				len = 0;
		if (len < 0 && codeb == 0x61) {
				fprintf( stderr, "Guess (B) length 0 for 0x%x\n", codea );
				len = 0;
		}

		if ((codea == 0x12 || codea == 0x13) &&
			codeb == 0x50)
				len = 2;
		if (len < 0 && codeb == 0x50) {
				fprintf( stderr, "Guess (C) length 2 for 0x%x\n", codea );
				len = 2;
		}
#undef MAP
		return len;
}

int
main (int argc, char **argv)
{
		int die = 0;
		byte_t data[65536] = { 0, };

		in = fopen(argv[1], "r");
		fseek (in, init_offset, SEEK_SET);

		while (!feof(in) && !die) {
				byte_t codea = fgetc (in);
				byte_t codeb = fgetc (in);
				ValueType type;
				int len = get_len (codea, codeb, &type);
				if (len < 0) {
						fprintf (stderr, "Parsing error:\n");
						len = 1024;
						die = 1;
				}
				if (type == VALUE_STRING)
				{
						fprintf (stderr, "String (0x%x): %.2x%.2x: ", len, codea, codeb);
						dump_string (in, len);
				}
				else
				{
						if (len == 0)
								fprintf (stderr, "Flag: 0x%.2x%.2x\n", codea, codeb);
						else
						{
								fread (data, 1, len, in);
								fprintf (stderr, "Record: 0x%.2x%.2x (0x%d bytes): ",
										 codea, codeb, len );
								dump_hex (data, len);
						}
				}
		}
		fclose (in);
		return 0;
} 
