#include <stdio.h>
#include <malloc.h>

// Define VERBOSE to get more debug
// #define VERBOSE

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
	if (len > 4) {
		if (data[1] == 0x20 &&
		    len > 2) {
			int ptg = dump_runs( data + 2, len - 2);
		} else
			return;
	}
#ifdef VERBOSE
	fprintf( stderr, "Record:\n" );
	dump_hex(data, len);
#endif
}

int
main (int argc, char **argv)
{
	int i = 0;
	unsigned char data[256];

	in = fopen(argv[1], "r");

	while (!feof(in)) {
		data[i] = fgetc(in);

		if (data[i] == '@') { // Magic char
			dump(data, i);
			data[0] = '@';
			i = 0;
		}
		i++;
	}

	close (in);
	return 0;
} 
