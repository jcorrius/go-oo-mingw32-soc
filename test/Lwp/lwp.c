#include <stdio.h>
#include <glib.h>

/*
 * To build:
 *   gcc -Wall `pkg-config --libs --cflags glib-2.0` lwp.c
 */
int verbose = 0;


typedef struct {
  unsigned char  magic[8];
  guint16 flags;
  guint16 blockSize;
  guint16 majorVersion;
  guint16 minorVersion;
  guint32  tocOffset;
  guint32  tocSize;
} ContainerLabel; /* 24 bytes */


#define TOC_NewObject        1
#define TOC_NewProperty      2
#define TOC_NewType          3
#define TOC_ExplicitGen      4
#define TOC_Offset4Len4      5
#define TOC_ContdOffset4Len4 6
#define TOC_Offset8Len4      7
#define TOC_ContdOffset8Len4 8
#define TOC_Immediate0       9
#define TOC_Immediate1       10 /* 0x0a */
#define TOC_Immediate2       11 /* 0x0b */
#define TOC_Immediate3       12 /* 0x0c */
#define TOC_Immediate4       13 /* 0x0d */
#define TOC_ContdImmediate4  14 /* 0x0e */
#define TOC_ReferenceListId  15 /* 0x0f */
#define TOC_EndOfBufr        24 /* 0x16 */
#define TOC_NOP             255 /* 0xff */

typedef struct {
  guint32 typeId;
  guint32 gen_num;
  guint32 ref_obj_id;
  /* data */
} ContainerTocValue;

typedef struct {
  guint32 propId;
  GSList *values;
} ContainerTocProperty;

typedef struct {
  guint32 objectId;
  GSList *properties;
} ContainerTocObject;

typedef GSList *  ContainerToc;       /* objects */

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

static guint16
read_ushort (FILE *fin)
{
  unsigned char data[2];
  fread (data, 1, 2, fin);
  return data[0] + (data[1] << 8);
}

static guint32
read_ulong (FILE *fin)
{
  unsigned char data[4];
  fread (data, 1, 4, fin);
  return data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
}

/*
 * The 'Label' is always at the end of the Bento container.
 */
static ContainerLabel *
read_label (FILE *fin)
{
  ContainerLabel *label = g_new0 (ContainerLabel, 1);

  fseek (fin, -24, SEEK_END);
  fread (label->magic, 1, 8, fin);
  label->flags = read_ushort (fin);
  label->blockSize = read_ushort (fin); /* size of toc in blocks multiples of 1024 */
  label->majorVersion = read_ushort (fin);
  label->minorVersion = read_ushort (fin);
  label->tocOffset = read_ulong (fin);
  label->tocSize = read_ulong (fin);

  if (verbose)
    fprintf (stderr, "flags 0x%x blockSize 0x%x, ver: 0x%x.%x "
	     "toc Offset 0x%x, size 0x%x\n",
	     label->flags, label->blockSize, label->majorVersion,
	     label->minorVersion, label->tocOffset, label->tocSize);

  return label;
}

static ContainerTocValue *
read_toc_value (FILE *fin)
{
  int end = 0;
  guint8 ctrl;
  int have_contd = 0;
  ContainerTocValue *value = g_new0 (ContainerTocValue, 1);

  value->typeId = read_ulong (fin);
  fprintf( stderr, "Read value 0x%x\n", value->typeId );
  
  while (!end && !feof (fin))
    {
      ctrl = fgetc (fin);
      switch (ctrl) {
      case TOC_NewObject:
      case TOC_NewProperty:
      case TOC_NewType:
	//	fprintf( stderr, "FIXME: Unexpected parent op in value: 0x%x\n", ctrl );
	ungetc (ctrl, fin);
	end = 1;
	break;
      case TOC_ExplicitGen: {
	guint32 gen = read_ulong (fin);
	fprintf (stderr, "Explict Gen: 0x%x\n", gen);
	break;
      }
      case TOC_Offset4Len4: {
	guint32 offset = read_ulong (fin);
	fprintf (stderr, "Offset4Len4: 0x%x\n", offset);
      }
      case TOC_ContdOffset4Len4: {
	guint32 offset = read_ulong (fin);
	fprintf (stderr, "ContdOffset4Len4: 0x%x\n", offset);
	have_contd = 1;
      }
      case TOC_Offset8Len4: {
	guint8 data[8];
	fread (data, 1, 8, fin);
	fprintf (stderr, "Offset8Len4: ");
	dump_hex (data, 8);
      }
      case TOC_ContdOffset8Len4: {
	guint8 data[8];
	fread (data, 1, 8, fin);
	fprintf (stderr, "ContdOffset8Len4: ");
	dump_hex (data, 8);
	have_contd = 1;
      }
      case TOC_EndOfBufr:
	if (have_contd) {
	  have_contd = 0;
	  fprintf (stderr, "end continuation\n");
	} else {
	  end = 1;
	  fprintf (stderr, "end value\n");
	}
	break;
      case TOC_Immediate4:
      case TOC_Immediate3:
      case TOC_Immediate2:
      case TOC_Immediate1: {
	guint32 data = read_ulong (fin);
	fprintf (stderr, "Immediate [%d] 0x%x\n", ctrl, data);
	break;
      }
      case TOC_Immediate0:
	fprintf (stderr, "Immediate 0\n");
	break;
      default:
	fprintf (stderr, "Unknown value type %d (0x%x)\n", ctrl, ctrl);
	break;
      }
    }  

  return value;
}

static ContainerTocProperty *
read_toc_prop (FILE *fin)
{
  int end = 0;
  guint8 ctrl = TOC_NewType;
  ContainerTocProperty *prop = g_new0 (ContainerTocProperty, 1);

  prop->propId = read_ulong (fin);
  fprintf (stderr, "Read prop id 0x%x\n", prop->propId);

  while (!end && !feof (fin))
    {
      switch (ctrl)
	{
	case TOC_NewObject:
	case TOC_NewProperty:
	  //	  fprintf( stderr, "FIXME: Unexpected parent op in property: 0x%x\n", ctrl );
	  ungetc (ctrl, fin);
	  end = 1;
	  break;
	case TOC_NewType:
	  prop->values = g_slist_append (prop->values,
					 read_toc_value (fin));
	  break;
	case TOC_EndOfBufr:
	  end = 1;
	  fprintf (stderr, "end property\n");
	  break;
	default:
	  fprintf (stderr, "Unknown property type %d\n", ctrl);
	  break;
	}
      if (!end)
	ctrl = fgetc (fin);
    }

  return prop;
}

static ContainerTocObject *
read_toc_object (FILE *fin, ContainerTocObject *last)
{
  int end = 0;
  guint8 ctrl = TOC_NewProperty;
  ContainerTocObject *object = g_new0 (ContainerTocObject, 1);
  
  object->objectId = read_ulong (fin);
  fprintf (stderr, "Read object id 0x%x\n", object->objectId);
  while (!end && !feof (fin)) {
    switch (ctrl) {
    case TOC_NewObject:
      //      fprintf( stderr, "FIXME: Unexpected parent op in object: 0x%x\n", ctrl );
      ungetc (ctrl, fin);
      end = 1;
      break;
    case TOC_NewProperty: {
      ContainerTocProperty *prop = read_toc_prop (fin);
      object->properties = g_slist_append (object->properties, prop);
      break;
    }
    case TOC_EndOfBufr:
      end = 1;
      fprintf (stderr, "end object\n");
      break;
    default:
      fprintf (stderr, "Unknown object type %d\n", ctrl);
      break;
    }
    if (!end)
      ctrl = fgetc (fin);
  }
  return object;
}

static ContainerToc *
read_toc (FILE *fin, ContainerLabel *label)
{
  int end;
  GSList *toc = NULL;
  ContainerTocObject *last = NULL;
  
  if (verbose)
    {
      guint8 *data = g_malloc (label->tocSize);
      fprintf (stderr, "Tok:\n");
      fseek (fin, label->tocOffset, SEEK_SET);
      fread (data, 1, label->tocSize, fin);
      dump_hex (data, label->tocSize);
    }

  fseek (fin, label->tocOffset, SEEK_SET);
  while (!end && !feof (fin)) {
    guint8 ctrl = fgetc (fin);
    switch (ctrl) {
    case TOC_NewObject: {
      ContainerTocObject *obj = read_toc_object (fin, last);
      toc = g_slist_append (toc, obj);
      last = obj;
      break;
    }
    case TOC_NOP:
      fprintf (stderr, "NOP\n");
      break;
    case TOC_EndOfBufr:
      end = 1;
      fprintf (stderr, "end toc\n");
      break;
    default:
      fprintf (stderr, "Unknown code %d\n", ctrl);
      break;
    }
  }

  return (ContainerToc *)toc;
}

int
main (int argc, char **argv)
{
  int i;
  const char *fname = NULL;
  ContainerLabel *label;
  ContainerToc *toc;
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

  label = read_label (fin);
  toc = read_toc (fin, label);

  fclose (fin);

  return 0;
} 
