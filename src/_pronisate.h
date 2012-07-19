struct pron_context {
	size_t		   width;
	size_t		   height;
	ssize_t		   frame_count;
	ssize_t		   panels_x;
	ssize_t		   panels_y;
	unsigned char	***streams;
	MagickWand	  *wand;
};
