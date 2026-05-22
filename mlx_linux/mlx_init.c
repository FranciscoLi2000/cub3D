#include "mlx_int.h"

void	*mlx_init(void)
{
	t_xvar	*xvar;

	xvar = mlx_int_calloc(1, sizeof(*xvar));
	if (!xvar)
		return (NULL);
	xvar->display = XOpenDisplay(NULL);
	if (!xvar->display)
	{
		free(xvar);
		return (NULL);
	}
	xvar->screen = XDefaultScreen(xvar->display);
	xvar->root = XRootWindow(xvar->display, xvar->screen);
	xvar->cmap = XDefaultColormap(xvar->display, xvar->screen);
	xvar->depth = XDefaultDepth(xvar->display, xvar->screen);
	xvar->visual = XDefaultVisual(xvar->display, xvar->screen);
	xvar->gc = XCreateGC(xvar->display, xvar->root, 0, NULL);
	xvar->do_flush = 1;
	xvar->wm_delete_window = XInternAtom(xvar->display,
			"WM_DELETE_WINDOW", False);
	xvar->wm_protocols = XInternAtom(xvar->display, "WM_PROTOCOLS", False);
	xvar->end_loop = 0;
	return (xvar);
}
