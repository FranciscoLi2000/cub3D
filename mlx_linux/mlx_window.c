#include "mlx_int.h"

t_win_list	*mlx_int_find_window(t_xvar *xvar, Window window)
{
	t_win_list	*curr;

	curr = xvar->win_list;
	while (curr)
	{
		if (curr->window == window)
			return (curr);
		curr = curr->next;
	}
	return (NULL);
}

static void	mlx_int_add_window(t_xvar *xvar, t_win_list *win)
{
	win->next = xvar->win_list;
	xvar->win_list = win;
}

void	*mlx_new_window(void *mlx_ptr, int size_x, int size_y, char *title)
{
	t_xvar		*xvar;
	t_win_list	*win;
	long		mask;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (NULL);
	win = mlx_int_calloc(1, sizeof(*win));
	if (!win)
		return (NULL);
	win->window = XCreateSimpleWindow(xvar->display, xvar->root, 0, 0,
			size_x, size_y, 0, 0, 0);
	if (!win->window)
	{
		free(win);
		return (NULL);
	}
	win->gc = XCreateGC(xvar->display, win->window, 0, NULL);
	win->width = size_x;
	win->height = size_y;
	win->xvar = xvar;
	mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask
		| ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;
	XSelectInput(xvar->display, win->window, mask);
	XStoreName(xvar->display, win->window, title);
	XSetWMProtocols(xvar->display, win->window,
		&xvar->wm_delete_window, 1);
	if (!XMapWindow(xvar->display, win->window))
	{
		XFreeGC(xvar->display, win->gc);
		free(win);
		return (NULL);
	}
	mlx_int_add_window(xvar, win);
	return (win);
}

int	mlx_clear_window(void *mlx_ptr, void *win_ptr)
{
	t_xvar		*xvar;
	t_win_list	*win;

	xvar = (t_xvar *)mlx_ptr;
	win = (t_win_list *)win_ptr;
	if (!xvar || !win)
		return (0);
	XClearWindow(xvar->display, win->window);
	return (0);
}

int	mlx_pixel_put(void *mlx_ptr, void *win_ptr, int x, int y, int color)
{
	t_xvar		*xvar;
	t_win_list	*win;

	xvar = (t_xvar *)mlx_ptr;
	win = (t_win_list *)win_ptr;
	if (!xvar || !win)
		return (0);
	XSetForeground(xvar->display, win->gc, (unsigned long)color);
	XDrawPoint(xvar->display, win->window, win->gc, x, y);
	if (xvar->do_flush)
		XFlush(xvar->display);
	return (0);
}

int	mlx_destroy_window(void *mlx_ptr, void *win_ptr)
{
	t_xvar		*xvar;
	t_win_list	*win;
	t_win_list	*prev;
	t_win_list	*curr;

	xvar = (t_xvar *)mlx_ptr;
	win = (t_win_list *)win_ptr;
	prev = NULL;
	curr = xvar ? xvar->win_list : NULL;
	if (!xvar || !win)
		return (0);
	while (curr && curr != win)
	{
		prev = curr;
		curr = curr->next;
	}
	if (curr == win)
	{
		if (prev)
			prev->next = win->next;
		else
			xvar->win_list = win->next;
	}
	if (win->gc)
		XFreeGC(xvar->display, win->gc);
	if (win->window)
		XDestroyWindow(xvar->display, win->window);
	free(win);
	return (0);
}

int	mlx_destroy_display(void *mlx_ptr)
{
	t_xvar	*xvar;
	t_win_list	*next;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (0);
	while (xvar->win_list)
	{
		next = xvar->win_list->next;
		mlx_destroy_window(xvar, xvar->win_list);
		xvar->win_list = next;
	}
	if (xvar->gc)
		XFreeGC(xvar->display, xvar->gc);
	XCloseDisplay(xvar->display);
	free(xvar);
	return (0);
}
