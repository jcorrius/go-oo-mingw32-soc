#include <stdio.h>
#include <malloc.h>

// Define VERBOSE to get more debug
int verbose = 0;

// Odd section:
// 02 02 3f bf 62 65 20 6f 6e 20 68 69 73 20 66 69 | ..?.be on his fi
// 72 73 74 20 65 6e 74 65 72 69 6e 67 20 61 20 6e | rst entering a n
// 65 69 67 68 62 6f 75 72 68 6f 6f 64 2c 20 74 68 | eighbourhood, th
// 69 73 20 74 72 75 74 68 20 69 73 20 73 6f 20 77 | is truth is so w
// 65 6c 6c 01 c0 02 53 53 45 6e 64 00 00 94 02 2e | ell...SSEnd.....
// 00 02 6a c1 02 2f 41 02 2d 48 0f 61 01 6d 46 8d | ..j../A.-H.a.mF.
// 46 c3 01 f6 28 0e 41 01 96 40 01 42 | F...(.A..@.B


FILE *in;

static void
dump_string(unsigned char *data, int len)
{
	int i;
	fprintf(stderr ,"(%d): '", len);
	for (i = 0; i < len; i++)
		fprintf(stderr ,"%c", data[i]);
	fprintf(stderr ,"'\n");
}

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

static void
dump(unsigned char *data, int len)
{
	if (verbose) {
			fprintf( stderr, "Record:\n" );
			dump_hex(data, len);
	}

	if (len > 4) {
		if (data[1] == 0x20 &&
		    len > 2) {
			int ptg = dump_runs( data + 2, len - 2);
		} 
	}
}

int
main (int argc, char **argv)
{
	int i = 0;
	const char *fname = NULL;
	unsigned char data[256];

	for (i = 1; i < argc; i++)
	{
			if (argv[i][0] == '-' &&
				argv[i][1] == 'v')
					verbose = 1;
			else if (!fname)
					fname = argv[i];
	}

	in = fopen (fname, "r");

	i = 0;
	while (!feof (in)) {b
		data[i] = fgetc (in);

		if (data[i] == '@') { // Magic char
			dump (data, i);
			data[0] = '@';
			i = 0;
		}
		i++;
	}

	close (in);
	return 0;
} 
