/// To run: gcc lotus.c && ./a.out TextAttr.123 2>&1 | less

#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

static FILE *in;
typedef unsigned char byte_t;

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

int
main (int argc, char **argv)
{
		byte_t data[65550] = { 0, };
		int indent = 0;

		in = fopen(argv[1], "r");
		fseek (in, 0, SEEK_SET);
		while (!feof(in)) {
				unsigned short nOp = 0;
				unsigned short nLength = 0;

				fread( data, 1, 4, in );
				nOp = data[0] + (data[1] << 8);
				nLength = data[2] + (data[3] << 8);

				if (nOp == 0x107)
						indent-=2;
				print_indent (indent);
				if (nOp == 0x106)
						indent+=2;

				fprintf( stderr, "Op 0x%x, length 0x%x\n", nOp, nLength );
				fread( data, 1, nLength, in);
				dump_hex( data, nLength, indent + 1 );
		}
		fclose (in);
		return 0;
} 
