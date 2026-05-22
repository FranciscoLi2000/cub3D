#include "cub3d.h"

int	app_create_frame(t_app *app);
int	app_load_textures(t_app *app);
void	app_destroy_frame(t_app *app);
void	app_destroy_textures(t_app *app);

int	app_init(t_app *app)
{
	app->width = WINDOW_WIDTH;
	app->height = WINDOW_HEIGHT;
	app->running = 1;
	app->mlx = mlx_init();
	if (!app->mlx)
		return (ft_error("mlx_init failed"));
	app->win = mlx_new_window(app->mlx, app->width, app->height, "cub3D");
	if (!app->win)
		return (ft_error("mlx_new_window failed"));
	mlx_hook(app->win, X11_KEY_PRESS, X11_KEY_PRESS_MASK, app_key_press, app);
	mlx_hook(app->win, X11_KEY_RELEASE, X11_KEY_RELEASE_MASK,
		app_key_release, app);
	mlx_hook(app->win, X11_DESTROY_NOTIFY, X11_STRUCTURE_NOTIFY_MASK,
		app_close_hook, app);
	mlx_hook(app->win, X11_CLIENT_MESSAGE, X11_STRUCTURE_NOTIFY_MASK,
		app_close_hook, app);
	mlx_loop_hook(app->mlx, app_loop, app);
	return (0);
}

int	app_prepare(t_app *app)
{
	if (app_create_frame(app) != 0)
		return (1);
	if (app_load_textures(app) != 0)
		return (1);
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
	if (mlx)
		mlx_loop_end(mlx);
	app_destroy_frame(app);
	app_destroy_textures(app);
	if (mlx && app->win)
		mlx_destroy_window(mlx, app->win);
	if (mlx)
		mlx_destroy_display(mlx);
	scene_destroy(app);
	app->win = NULL;
	app->mlx = NULL;
	return (0);
}

int	app_close_hook(void *param)
{
	return (app_close((t_app *)param));
}
