#include <stdio.h>
#include <malloc.h>

// This is only any good for austin.lwp - oddly.

int verbose = 0;
int init_offset = 68;

// Odd section:
// 02 02 3f bf 62 65 20 6f 6e 20 68 69 73 20 66 69 | ..?.be on his fi
// 72 73 74 20 65 6e 74 65 72 69 6e 67 20 61 20 6e | rst entering a n
// 65 69 67 68 62 6f 75 72 68 6f 6f 64 2c 20 74 68 | eighbourhood, th
// 69 73 20 74 72 75 74 68 20 69 73 20 73 6f 20 77 | is truth is so w
// 65 6c 6c 01 c0 02 53 53 45 6e 64 00 00 94 02 2e | ell...SSEnd.....
// 00 02 6a c1 02 2f 41 02 2d 48 0f 61 01 6d 46 8d | ..j../A.-H.a.mF.
// 46 c3 01 f6 28 0e 41 01 96 40 01 42 | F...(.A..@.B


static unsigned char
my_fgetc (FILE *fin)
{
		/* Very, very odd: custom hacks for austin.lwp */
		switch (ftell (fin)) {

		case 0x28c4: // 0xc6 - mid-string
		case 0x2b67: // 0xc2 - mid string
		case 0x2fd4: // 0xc1 - c1 01 02
				fgetc (fin); // skip duff byte
				break;
		default:
				break;
		};
		return fgetc (fin);
}

static void
dump_string (FILE *fin, int len)
{
	int i;
	fprintf(stderr ,"(%d): '", len);
	for (i = 0; i < len; i++) {
			unsigned char c = my_fgetc (fin);
			if (c >= 0x20 && c < 0x80)
					fprintf (stderr, "%c", c);
			else
					fprintf (stderr, "#0x%x#", c);
	}
	fprintf (stderr,"'\n");
}

#if 0
static int
dump_runs(unsigned char *data, int len)
{
	int i;
	int pos = 0;

	while (pos < len) {
		int typea = data[0]; // Continuations show hdr to be 4 bytes
		int typeb = data[1];
		int run_len = data[2];
		int typec = data[3];

		if (run_len > len - 5)
			run_len = len - 5;
		
		if (typea == 0x0b &&
		    typeb == 0xc0) { // starter thing
			fprintf( stderr, "String: [0x%x]\n", typeb );
			dump_string( data + 4, run_len );
		} else if (typea == 0x82 &&
			   typeb == 0x02 &&
			   run_len == 0x04) {
			// CRLF ? - end marker anyway.
			break;
		} else if (typeb == 0x02) {
			fprintf( stderr, "StringC: [0x%x]\n", typeb );
			dump_string( data + 4, run_len );
		} else {
			fprintf( stderr, "unknown type 0x%x 0x%x 0x%x 0x%x\n", 
				 typea, typeb, run_len, typec );
			break;
		}
		pos += run_len + 4;
		data += run_len + 4;
	}	
	return pos;
}
#endif

static void
dump_hex(unsigned char *data, int len)
{
	while (len > 0) {
		int i;
		int chunk = len < 16 ? len : 16;

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
skip_to_at (FILE *fin, unsigned char code, int print)
{
		int i;
		unsigned char skip_data[65536];
		skip_data[0] = code;
		for (i = 1; (skip_data[i] = my_fgetc (fin)) != '@' && i < sizeof (skip_data); i++);
		if (print)
				dump_hex (skip_data, i + 1);
}


static void
scan_text (FILE *fin)
{
		int end_of_run = 0;
		while (!feof (fin) && !end_of_run) {
				unsigned char codea = my_fgetc (fin);
				unsigned char codeb = my_fgetc (fin);
				int code = codeb + (codea << 8);
				int skip = 0;
				switch (code) {
				case 0x0202:
				case 0x0bc0: {
						unsigned char len = my_fgetc (fin);
						unsigned char dummy = my_fgetc (fin);
						dump_string (fin, len);
						break;
				}
				case 0x01c0: // for some reason we often get 0x01c002
						codeb = my_fgetc (fin);
						if (codeb != 0x02)
						{
								fprintf (stderr, "Odd 0x1c0\n");
								end_of_run = 1;
								break;
						}
						// drop through
				case 0x0102: { /* SSE */
						char data[29];
						int len = 22;
						fprintf (stderr, "SSE\n");
						fread (data, 1, len, fin); // or 29 ?
						if (verbose)
								dump_hex (data, len);
						break;
				}
				case 0x016d: /* SSE */
						skip = 3;
						break;

				case 0x0c10: /* SSE */
				case 0x0c41: /* SSE */
				case 0x0141:
				case 0x0e41:
						fprintf (stderr, "0x%x\n", code);
						break;
				case 0x01f6:
						fprintf (stderr, "0x%x\n", code);
						skip = 3;
						break;
				case 0x0196: { // embedded @ somehow
						skip = 3;
						break;
				}
				case 0x8202:
						fprintf (stderr, "CRLF?\n");
						skip = 2;
						break;
				case 0xc301:
						fprintf (stderr, "0x%x\n", code);
						skip = 4;
						break;
				default: {
						fprintf (stderr, "Unknown code 0x%x\n", code);
						fseek (fin, -2, SEEK_CUR);
						skip_to_at (fin, 0x20, 1);
						end_of_run = 1;
						break;
				}
				}
				if (skip)
						fseek (fin, skip, SEEK_CUR);
		}
}

int
main (int argc, char **argv)
{
	int i;
	const char *fname = NULL;
	FILE *fin;

	for (i = 1; i < argc; i++)
	{
			if (argv[i][0] == '-' &&
				argv[i][1] == 'v')
					verbose = 1;
			else if (!fname)
					fname = argv[i];
	}

	fin = fopen (fname, "r");
	fseek (fin, init_offset, SEEK_SET);
	
	while (!feof (fin)) {
		unsigned char code;
		code = my_fgetc (fin);
		switch (code) {
		case 0x20:
				code = my_fgetc (fin);
				if (code == 0x40)
						fprintf (stderr, "2040\n");
				else {
						ungetc (code, fin);
						scan_text (fin);
				}
				break;
		default: {
				skip_to_at (fin, code, verbose);
				break;
		}
		}
	}

	fclose (fin);

	return 0;
} 
