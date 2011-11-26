struct pron_context;

struct pron_context	*pron_context_open(size_t _width, size_t height);
int			 pron_pronisate(struct pron_context *_ctx, char *_filename, ssize_t frame);
unsigned char		*pron_get_stream(struct pron_context *_ctx);
size_t			 pron_get_width(struct pron_context *_ctx);
size_t			 pron_get_height(struct pron_context *_ctx);
void			 pron_context_close(struct pron_context *_ctx);
