#include "mlx_int.h"

void	*mlx_new_image(void *mlx_ptr, int width, int height)
{
	t_xvar	*xvar;
	t_img	*img;
	char	*data;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (NULL);
	img = mlx_int_calloc(1, sizeof(*img));
	if (!img)
		return (NULL);
	img->image = XCreateImage(xvar->display, xvar->visual, xvar->depth,
			ZPixmap, 0, NULL, width, height, 32, 0);
	if (!img->image)
	{
		free(img);
		return (NULL);
	}
	img->width = width;
	img->height = height;
	img->bpp = img->image->bits_per_pixel;
	img->size_line = img->image->bytes_per_line;
	img->endian = img->image->byte_order;
	img->xvar = xvar;
	data = mlx_int_calloc((size_t)img->size_line * (size_t)height, 1);
	if (!data)
	{
		XDestroyImage(img->image);
		free(img);
		return (NULL);
	}
	img->data = data;
	img->image->data = data;
	return (img);
}

char	*mlx_get_data_addr(void *img_ptr, int *bits_per_pixel,
		int *size_line, int *endian)
{
	t_img	*img;

	img = (t_img *)img_ptr;
	if (!img)
		return (NULL);
	if (bits_per_pixel)
		*bits_per_pixel = img->bpp;
	if (size_line)
		*size_line = img->size_line;
	if (endian)
		*endian = img->endian;
	return (img->data);
}

int	mlx_put_image_to_window(void *mlx_ptr, void *win_ptr,
		void *img_ptr, int x, int y)
{
	t_xvar		*xvar;
	t_win_list	*win;
	t_img		*img;

	xvar = (t_xvar *)mlx_ptr;
	win = (t_win_list *)win_ptr;
	img = (t_img *)img_ptr;
	if (!xvar || !win || !img)
		return (0);
	XPutImage(xvar->display, win->window, win->gc, img->image,
		0, 0, x, y, img->width, img->height);
	if (xvar->do_flush)
		XFlush(xvar->display);
	return (0);
}

int	mlx_destroy_image(void *mlx_ptr, void *img_ptr)
{
	t_xvar	*xvar;
	t_img	*img;

	xvar = (t_xvar *)mlx_ptr;
	img = (t_img *)img_ptr;
	if (!xvar || !img)
		return (0);
	if (img->image)
		XDestroyImage(img->image);
	free(img);
	return (0);
}

int	mlx_get_color_value(void *mlx_ptr, int color)
{
	(void)mlx_ptr;
	return (color);
}
