#include "cub3d.h"

int	app_init(t_app *app)
{
	app->width = WINDOW_WIDTH;
	app->height = WINDOW_HEIGHT;
	app->running = 1;
	app->mlx = mlx_init();
	if (!app->mlx)
		return (ft_error("mlx_init failed"));
	app->win = mlx_new_window(app->mlx, app->width, app->height,
			"cub3D");
	if (!app->win)
		return (ft_error("mlx_new_window failed"));
	mlx_hook(app->win, X11_DESTROY_NOTIFY, X11_STRUCTURE_NOTIFY_MASK,
		(int (*)())app_close_hook, app);
	mlx_hook(app->win, X11_CLIENT_MESSAGE, X11_STRUCTURE_NOTIFY_MASK,
		(int (*)())app_close_hook, app);
	mlx_key_hook(app->win, app_key_hook, app);
	return (0);
}

int	app_run(t_app *app)
{
	mlx_loop(app->mlx);
	return (0);
}

int	app_close(t_app *app)
{
	void	*mlx;

	if (!app || !app->running)
		return (0);
	app->running = 0;
	mlx = app->mlx;
	app->mlx = NULL;
	if (mlx)
		mlx_loop_end(mlx);
	if (app->win)
	{
		mlx_destroy_window(mlx, app->win);
		app->win = NULL;
	}
	if (mlx)
		mlx_destroy_display(mlx);
	return (0);
}

int	app_key_hook(int keycode, void *param)
{
	t_app	*app;

	app = (t_app *)param;
	if (keycode == KEY_ESC)
		return (app_close(app));
	return (0);
}

int	app_close_hook(void *param)
{
	return (app_close((t_app *)param));
}
