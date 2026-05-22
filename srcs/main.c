#include "cub3d.h"

int	main(int argc, char **argv)
{
	t_app	app;

	if (argc != 2)
		return (ft_error("usage: ./cub3D <scene.cub>"));
	if (scene_validate_path(argv[1]) != 0)
		return (1);
	if (app_init(&app) != 0)
		return (1);
	return (app_run(&app));
}
