
typedef struct {
	int channels;
	int sampling_rate;
	int bitrate;
	int length;
	int object_type;
	int headertype;
} faadAACInfo;


int get_AAC_format(char *filename, faadAACInfo *info, int *seek_table);

static int f_id3v2_tag(FILE *file);
static int read_ADIF_header(FILE *file, faadAACInfo *info);
static int read_ADTS_header(FILE *file, faadAACInfo *info, int *seek_table,
							int tagsize);
