#include "cub3d.h"

int	main(int argc, char **argv)
{
	t_app	app;

	ft_memset(&app, 0, sizeof(app));
	if (argc != 2)
		return (ft_error("usage: ./cub3D <scene.cub>"));
	if (scene_parse(&app, argv[1]) != 0)
		return (1);
	if (app_init(&app) != 0)
		return (scene_destroy(&app), 1);
	if (app_prepare(&app) != 0)
		return (app_close(&app), 1);
	return (app_run(&app));
}
