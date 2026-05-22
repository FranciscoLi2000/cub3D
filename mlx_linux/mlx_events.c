#include "mlx_int.h"

int	mlx_mouse_hook(void *win_ptr,
		int (*funct_ptr)(int button, int x, int y, void *param),
		void *param)
{
	t_win_list	*win;

	win = (t_win_list *)win_ptr;
	if (!win)
		return (0);
	win->mouse_hook = (int (*)())funct_ptr;
	win->mouse_param = param;
	return (0);
}

int	mlx_key_hook(void *win_ptr, int (*funct_ptr)(int keycode, void *param),
		void *param)
{
	t_win_list	*win;

	win = (t_win_list *)win_ptr;
	if (!win)
		return (0);
	win->key_hook = (int (*)())funct_ptr;
	win->key_param = param;
	return (0);
}

int	mlx_expose_hook(void *win_ptr, int (*funct_ptr)(void *), void *param)
{
	t_win_list	*win;

	win = (t_win_list *)win_ptr;
	if (!win)
		return (0);
	win->expose_hook = (int (*)())funct_ptr;
	win->expose_param = param;
	return (0);
}

int	mlx_loop_hook(void *mlx_ptr, int (*funct_ptr)(void *), void *param)
{
	t_xvar	*xvar;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (0);
	xvar->loop_hook = (int (*)())funct_ptr;
	xvar->loop_param = param;
	return (0);
}

int	mlx_hook(void *win_ptr, int x_event, int x_mask, int (*funct)(),
		void *param)
{
	t_win_list	*win;

	win = (t_win_list *)win_ptr;
	if (!win || x_event < 0 || x_event >= 36)
		return (0);
	win->hooks[x_event].hook = funct;
	win->hooks[x_event].param = param;
	win->hooks[x_event].mask = x_mask;
	return (0);
}

int	mlx_loop_end(void *mlx_ptr)
{
	t_xvar	*xvar;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (0);
	xvar->end_loop = 1;
	return (0);
}

static int	mlx_int_dispatch_key(t_win_list *win, t_xevent *event)
{
	KeySym	keysym;

	if (win->hooks[event->type].hook)
	{
		keysym = XLookupKeysym(&event->xkey, 0);
		return (((int (*)(int, void *))win->hooks[event->type].hook)(
				(int)keysym, win->hooks[event->type].param));
	}
	if (event->type == KeyPress && win->key_hook)
	{
		keysym = XLookupKeysym(&event->xkey, 0);
		return (((int (*)(int, void *))win->key_hook)((int)keysym,
				win->key_param));
	}
	return (0);
}

static int	mlx_int_dispatch_mouse(t_win_list *win, t_xevent *event)
{
	if (win->hooks[event->type].hook)
		return (((int (*)(int, int, int, void *))win->hooks[event->type].hook)(
				event->xbutton.button, event->xbutton.x,
				event->xbutton.y, win->hooks[event->type].param));
	if (event->type == ButtonPress && win->mouse_hook)
		return (((int (*)(int, int, int, void *))win->mouse_hook)(
				event->xbutton.button, event->xbutton.x,
				event->xbutton.y, win->mouse_param));
	return (0);
}

void	mlx_int_event_dispatch(t_xvar *xvar, t_xevent *event)
{
	t_win_list	*win;

	win = NULL;
	if (event->type == ClientMessage)
		win = mlx_int_find_window(xvar, event->xclient.window);
	else if (event->type == KeyPress || event->type == KeyRelease)
		win = mlx_int_find_window(xvar, event->xkey.window);
	else if (event->type == ButtonPress || event->type == ButtonRelease)
		win = mlx_int_find_window(xvar, event->xbutton.window);
	else if (event->type == Expose)
		win = mlx_int_find_window(xvar, event->xexpose.window);
	else if (event->type == DestroyNotify || event->type == ConfigureNotify)
		win = mlx_int_find_window(xvar, event->xconfigure.window);
	if (!win)
		return ;
	if (event->type == ClientMessage
		&& event->xclient.data.l[0] == (long)xvar->wm_delete_window)
	{
		if (win->hooks[ClientMessage].hook)
			((int (*)(void *))win->hooks[ClientMessage].hook)(
				win->hooks[ClientMessage].param);
		return ;
	}
	if (event->type == KeyPress)
		mlx_int_dispatch_key(win, event);
	else if (event->type == KeyRelease)
		mlx_int_dispatch_key(win, event);
	else if (event->type == ButtonPress)
		mlx_int_dispatch_mouse(win, event);
	else if (event->type == ButtonRelease)
		mlx_int_dispatch_mouse(win, event);
	else if (event->type == Expose && win->expose_hook)
		((int (*)(void *))win->expose_hook)(win->expose_param);
	else if (event->type == Expose && win->hooks[Expose].hook)
		((int (*)(void *))win->hooks[Expose].hook)(
			win->hooks[Expose].param);
	else if (event->type == DestroyNotify && win->hooks[DestroyNotify].hook)
		((int (*)(void *))win->hooks[DestroyNotify].hook)(
			win->hooks[DestroyNotify].param);
	else if (event->type == ConfigureNotify
		&& win->hooks[ConfigureNotify].hook)
		((int (*)(void *))win->hooks[ConfigureNotify].hook)(
			win->hooks[ConfigureNotify].param);
	else if (win->hooks[event->type].hook)
		((int (*)(void *))win->hooks[event->type].hook)(
			win->hooks[event->type].param);
}
