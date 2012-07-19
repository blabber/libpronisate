typedef enum pron_error {
	PRON_SUCCESS = 0,
	PRON_MALLOC_ERROR,
	PRON_READ_IMAGE_ERROR,
	PRON_WAND_ERROR,
	PRON_PIXELSETCOLOR_ERROR,
} PRON_ERROR;

struct pron_context;

void		 pron_init(void);
PRON_ERROR	 pron_context_open(char *_filename, size_t _width, size_t _height, ssize_t _panels_x, ssize_t _panels_y, struct pron_context **_ctx);
PRON_ERROR	 pron_pronisate(struct pron_context *_ctx, ssize_t _frame);
unsigned char	*pron_get_stream(struct pron_context *_ctx, ssize_t px, ssize_t py);
size_t		 pron_get_width(struct pron_context *_ctx);
size_t		 pron_get_height(struct pron_context *_ctx);
ssize_t		 pron_get_frame_count(struct pron_context *_ctx);
const char	*pron_get_error(PRON_ERROR error);
void		 pron_context_close(struct pron_context *_ctx);
void		 pron_deinit(void);
