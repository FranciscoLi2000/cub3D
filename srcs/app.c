#include "cub3d.h"

static int	app_close_window(void *param)
{
	return (app_close((t_app *)param));
}

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
	mlx_hook(app->win, 17, 1L << 17,
		(int (*)())app_close_window, app);
	mlx_hook(app->win, 33, 1L << 17,
		(int (*)())app_close_window, app);
	mlx_key_hook(app->win, app_key_hook, app);
	return (0);
}

int	app_run(t_app *app)
{
	(void)app;
	mlx_loop(app->mlx);
	return (0);
}

int	app_close(t_app *app)
{
	if (!app || !app->running)
		return (0);
	app->running = 0;
	if (app->win)
	{
		mlx_destroy_window(app->mlx, app->win);
		app->win = NULL;
	}
	if (app->mlx)
	{
		mlx_destroy_display(app->mlx);
		app->mlx = NULL;
	}
	exit(0);
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
