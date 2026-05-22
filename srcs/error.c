#include "cub3d.h"

int	ft_error(const char *msg)
{
	ft_putstr_fd("Error\n", 2);
	if (msg && *msg)
	{
		ft_putstr_fd(msg, 2);
		ft_putstr_fd("\n", 2);
	}
	return (1);
}

int	scene_validate_path(const char *path)
{
	int	fd;
	size_t	len;

	if (!path)
		return (ft_error("missing scene file"));
	len = ft_strlen(path);
	if (len < 4 || ft_strncmp(path + len - 4, ".cub", 4) != 0)
		return (ft_error("scene file must end with .cub"));
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (ft_error("cannot open scene file"));
	close(fd);
	return (0);
}
