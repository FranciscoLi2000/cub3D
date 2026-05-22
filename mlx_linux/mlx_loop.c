#include "mlx_int.h"

int	mlx_loop(void *mlx_ptr)
{
	t_xvar	*xvar;
	t_xevent	event;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (0);
	while (!xvar->end_loop)
	{
		while (XPending(xvar->display))
		{
			XNextEvent(xvar->display, &event);
			mlx_int_event_dispatch(xvar, &event);
		}
		if (xvar->loop_hook)
			((int (*)(void *))xvar->loop_hook)(xvar->loop_param);
		XFlush(xvar->display);
		usleep(1000);
	}
	return (0);
}

int	mlx_do_sync(void *mlx_ptr)
{
	t_xvar	*xvar;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (0);
	XSync(xvar->display, False);
	return (0);
}

int	mlx_do_key_autorepeatoff(void *mlx_ptr)
{
	t_xvar	*xvar;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (0);
	XAutoRepeatOff(xvar->display);
	return (0);
}

int	mlx_do_key_autorepeaton(void *mlx_ptr)
{
	t_xvar	*xvar;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (0);
	XAutoRepeatOn(xvar->display);
	return (0);
}

int	mlx_get_screen_size(void *mlx_ptr, int *sizex, int *sizey)
{
	t_xvar		*xvar;
	Window		root;
	int			x;
	int			y;
	unsigned int	w;
	unsigned int	h;
	unsigned int	border;
	unsigned int	depth;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar || !sizex || !sizey)
		return (0);
	if (XGetGeometry(xvar->display, xvar->root, &root, &x, &y,
			&w, &h, &border, &depth) != Success)
		return (0);
	*sizex = (int)w;
	*sizey = (int)h;
	return (0);
}
