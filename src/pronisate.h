struct pron_context;

void			   pron_init(void);
struct pron_context	  *pron_context_open(char *_filename, size_t _width, size_t _height, ssize_t _panels_x, ssize_t _panels_y);
int			   pron_pronisate(struct pron_context *_ctx, ssize_t _frame);
unsigned char		***pron_get_streams(struct pron_context *_ctx);
size_t			   pron_get_width(struct pron_context *_ctx);
size_t			   pron_get_height(struct pron_context *_ctx);
ssize_t			   pron_get_frame_count(struct pron_context *_ctx);
void			   pron_context_close(struct pron_context *_ctx);
void			   pron_deinit(void);
