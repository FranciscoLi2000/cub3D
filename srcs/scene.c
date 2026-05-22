#include "cub3d.h"

typedef struct s_parse
{
	char	**lines;
	int		line_count;
	char	**map_lines;
	int		map_count;
}	t_parse;

static void	free_str_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

static int	count_lines(char **lines)
{
	int	i;

	i = 0;
	while (lines && lines[i])
		i++;
	return (i);
}

static char	*read_file(const char *path)
{
	int		fd;
	ssize_t	rd;
	char	buffer[4096 + 1];
	char	*content;
	char	*tmp;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (NULL);
	content = ft_strdup("");
	if (!content)
		return (close(fd), NULL);
	rd = read(fd, buffer, 4096);
	while (rd > 0)
	{
		buffer[rd] = '\0';
		tmp = ft_strjoin(content, buffer);
		free(content);
		content = tmp;
		if (!content)
			return (close(fd), NULL);
		rd = read(fd, buffer, 4096);
	}
	close(fd);
	if (rd < 0)
		return (free(content), NULL);
	return (content);
}

static const char	*skip_spaces(const char *s)
{
	while (*s == ' ' || *s == '\t')
		s++;
	return (s);
}

static int	is_directive(const char *line, const char *id)
{
	size_t	len;

	len = ft_strlen(id);
	if (ft_strncmp(line, id, len) != 0)
		return (0);
	return (line[len] == '\0' || line[len] == ' ' || line[len] == '\t');
}

static int	is_map_line(const char *line)
{
	line = skip_spaces(line);
	return (*line == '0' || *line == '1' || *line == 'N'
		|| *line == 'S' || *line == 'E' || *line == 'W');
}

static int	parse_rgb_value(const char **s, int *value)
{
	const char	*cur;

	cur = skip_spaces(*s);
	if (!ft_isdigit(*cur))
		return (0);
	*value = ft_atoi(cur);
	if (*value < 0 || *value > 255)
		return (0);
	while (ft_isdigit(*cur))
		cur++;
	*s = cur;
	return (1);
}

static int	parse_color(const char *line, int *color)
{
	int			r;
	int			g;
	int			b;
	const char	*cur;

	cur = skip_spaces(line);
	if (!parse_rgb_value(&cur, &r) || *skip_spaces(cur) != ',')
		return (0);
	cur = skip_spaces(cur) + 1;
	if (!parse_rgb_value(&cur, &g) || *skip_spaces(cur) != ',')
		return (0);
	cur = skip_spaces(cur) + 1;
	if (!parse_rgb_value(&cur, &b))
		return (0);
	cur = skip_spaces(cur);
	if (*cur != '\0')
		return (0);
	*color = (r << 16) | (g << 8) | b;
	return (1);
}

static int	add_map_line(t_parse *parse, const char *line)
{
	char	**new_lines;
	int		i;

	new_lines = ft_calloc(parse->map_count + 2, sizeof(char *));
	if (!new_lines)
		return (0);
	i = 0;
	while (i < parse->map_count)
	{
		new_lines[i] = parse->map_lines[i];
		i++;
	}
	new_lines[parse->map_count] = ft_strdup(line);
	if (!new_lines[parse->map_count])
		return (free(new_lines), 0);
	free(parse->map_lines);
	parse->map_lines = new_lines;
	parse->map_count++;
	return (1);
}

static void	free_map_lines(t_parse *parse)
{
	int	i;

	if (!parse->map_lines)
		return ;
	i = 0;
	while (i < parse->map_count)
		free(parse->map_lines[i++]);
	free(parse->map_lines);
	parse->map_lines = NULL;
	parse->map_count = 0;
}

static int	parse_texture(t_app *app, int index, const char *line)
{
	const char	*cur;
	char		*path;

	cur = line + 2;
	cur = skip_spaces(cur);
	if (*cur == '\0')
		return (0);
	path = ft_strtrim(cur);
	if (!path)
		return (0);
	if (app->map->textures[index])
		return (free(path), 0);
	app->map->textures[index] = path;
	return (1);
}

static int	parse_directive(t_app *app, const char *line)
{
	if (is_directive(line, "NO"))
		return (parse_texture(app, TEX_NO, line));
	if (is_directive(line, "SO"))
		return (parse_texture(app, TEX_SO, line));
	if (is_directive(line, "WE"))
		return (parse_texture(app, TEX_WE, line));
	if (is_directive(line, "EA"))
		return (parse_texture(app, TEX_EA, line));
	if (is_directive(line, "F"))
		return (app->map->floor_color == -1
			&& parse_color(line + 1, &app->map->floor_color));
	if (is_directive(line, "C"))
		return (app->map->ceiling_color == -1
			&& parse_color(line + 1, &app->map->ceiling_color));
	return (0);
}

static int	is_valid_map_char(char c)
{
	return (c == ' ' || c == '0' || c == '1' || c == 'N'
		|| c == 'S' || c == 'E' || c == 'W');
}

static void	set_player_dir(t_player *player, char c)
{
	player->plane_x = 0;
	player->plane_y = 0;
	if (c == 'N')
	{
		player->dir_x = 0;
		player->dir_y = -1;
		player->plane_x = 0.66;
	}
	else if (c == 'S')
	{
		player->dir_x = 0;
		player->dir_y = 1;
		player->plane_x = -0.66;
	}
	else if (c == 'E')
	{
		player->dir_x = 1;
		player->dir_y = 0;
		player->plane_y = 0.66;
	}
	else if (c == 'W')
	{
		player->dir_x = -1;
		player->dir_y = 0;
		player->plane_y = -0.66;
	}
}

static int	validate_map(t_app *app, t_parse *parse)
{
	int		y;
	int		x;
	int		len;
	int		player_count;

	app->map->height = parse->map_count;
	y = 0;
	while (y < parse->map_count)
	{
		len = (int)ft_strlen(parse->map_lines[y]);
		if (len > app->map->width)
			app->map->width = len;
		y++;
	}
	app->map->grid = ft_calloc(app->map->height + 1, sizeof(char *));
	if (!app->map->grid)
		return (0);
	player_count = 0;
	y = 0;
	while (y < app->map->height)
	{
		app->map->grid[y] = ft_calloc(app->map->width + 1, sizeof(char));
		if (!app->map->grid[y])
		{
			while (y-- > 0)
				free(app->map->grid[y]);
			free(app->map->grid);
			app->map->grid = NULL;
			return (0);
		}
		ft_memset(app->map->grid[y], ' ', app->map->width);
		len = (int)ft_strlen(parse->map_lines[y]);
		x = 0;
		while (x < len)
		{
			char	c;

			c = parse->map_lines[y][x];
			if (!is_valid_map_char(c))
				return (0);
			if (c == 'N' || c == 'S' || c == 'E' || c == 'W')
			{
				player_count++;
				app->player->x = x + 0.5;
				app->player->y = y + 0.5;
				set_player_dir(app->player, c);
				c = '0';
			}
			app->map->grid[y][x] = c;
			x++;
		}
		y++;
	}
	app->map->player_count = player_count;
	return (player_count == 1);
}

static int	flood_validate(t_app *app)
{
	int			total;
	int			head;
	int			tail;
	int			*queue;
	char		*visited;
	int			idx;
	int			x;
	int			y;
	int			nx;
	int			ny;
	int			dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};

	total = app->map->width * app->map->height;
	queue = ft_calloc(total, sizeof(int));
	visited = ft_calloc(total, sizeof(char));
	if (!queue || !visited)
		return (free(queue), free(visited), 0);
	head = 0;
	tail = 0;
	idx = (int)app->player->y * app->map->width + (int)app->player->x;
	queue[tail++] = idx;
	visited[idx] = 1;
	while (head < tail)
	{
		idx = queue[head++];
		x = idx % app->map->width;
		y = idx / app->map->width;
		if (x < 0 || y < 0 || x >= app->map->width || y >= app->map->height)
			return (free(queue), free(visited), 0);
		if (app->map->grid[y][x] == ' ')
			return (free(queue), free(visited), 0);
		if (app->map->grid[y][x] == '1')
			continue;
		for (int i = 0; i < 4; i++)
		{
			nx = x + dirs[i][0];
			ny = y + dirs[i][1];
			if (nx < 0 || ny < 0 || nx >= app->map->width
				|| ny >= app->map->height)
				return (free(queue), free(visited), 0);
			if (app->map->grid[ny][nx] == ' ')
				return (free(queue), free(visited), 0);
			if (app->map->grid[ny][nx] != '1'
				&& !visited[ny * app->map->width + nx])
			{
				visited[ny * app->map->width + nx] = 1;
				queue[tail++] = ny * app->map->width + nx;
			}
		}
	}
	free(queue);
	free(visited);
	return (1);
}

static int	finalize_map(t_app *app, t_parse *parse)
{
	if (!validate_map(app, parse))
		return (0);
	if (!flood_validate(app))
		return (0);
	free_map_lines(parse);
	return (1);
}

int	scene_parse(t_app *app, const char *path)
{
	t_parse	parse;
	char	*content;
	int		i;
	char	**lines;
	char	*trimmed;
	int		map_started;

	if (!app || !path)
		return (1);
	app->map = ft_calloc(1, sizeof(t_map));
	app->player = ft_calloc(1, sizeof(t_player));
	app->frame = ft_calloc(1, sizeof(t_image));
	if (!app->map || !app->player || !app->frame)
		return (scene_destroy(app), ft_error("memory allocation failed"));
	app->map->floor_color = -1;
	app->map->ceiling_color = -1;
	content = read_file(path);
	if (!content)
		return (scene_destroy(app), ft_error("cannot read scene"));
	lines = ft_split(content, '\n');
	free(content);
	if (!lines)
		return (scene_destroy(app), ft_error("invalid scene"));
	parse.lines = lines;
	parse.line_count = count_lines(lines);
	parse.map_lines = NULL;
	parse.map_count = 0;
	map_started = 0;
	i = 0;
	while (i < parse.line_count)
	{
		trimmed = ft_strtrim(lines[i]);
		if (!trimmed)
			return (free_str_array(lines), free_map_lines(&parse), scene_destroy(app),
				ft_error("memory allocation failed"));
		if (*trimmed == '\0')
		{
			if (map_started)
			{
				free(trimmed);
				return (free_str_array(lines), free_map_lines(&parse), scene_destroy(app),
					ft_error("empty line inside map"));
			}
			free(trimmed);
			i++;
			continue ;
		}
		if (parse_directive(app, trimmed))
		{
			if (map_started)
			{
				free(trimmed);
				free_str_array(lines);
				free_map_lines(&parse);
				scene_destroy(app);
				return (ft_error("directive found after map"));
			}
			free(trimmed);
			i++;
			continue ;
		}
		if (is_map_line(trimmed))
		{
			map_started = 1;
			if (!add_map_line(&parse, lines[i]))
			{
				free(trimmed);
				return (free_str_array(lines), free_map_lines(&parse), scene_destroy(app),
					ft_error("memory allocation failed"));
			}
			free(trimmed);
			i++;
			continue ;
		}
		free(trimmed);
		return (free_str_array(lines), free_map_lines(&parse), scene_destroy(app),
			ft_error("invalid scene content"));
	}
	free_str_array(lines);
	if (app->map->floor_color == -1 || app->map->ceiling_color == -1)
		return (free_map_lines(&parse), scene_destroy(app), ft_error("missing color definitions"));
	if (parse.map_count == 0)
		return (free_map_lines(&parse), scene_destroy(app), ft_error("missing map"));
	if (!app->map->textures[TEX_NO] || !app->map->textures[TEX_SO]
		|| !app->map->textures[TEX_WE] || !app->map->textures[TEX_EA])
		return (free_map_lines(&parse), scene_destroy(app), ft_error("missing texture definitions"));
	if (!finalize_map(app, &parse))
		return (free_map_lines(&parse), scene_destroy(app),
			ft_error("invalid map"));
	app->width = WINDOW_WIDTH;
	app->height = WINDOW_HEIGHT;
	app->running = 1;
	return (0);
}

void	scene_destroy(t_app *app)
{
	int	i;

	if (!app)
		return ;
	if (app->map)
	{
		if (app->map->grid)
		{
			i = 0;
			while (app->map->grid[i])
				free(app->map->grid[i++]);
			free(app->map->grid);
		}
		i = 0;
		while (i < 4)
			free(app->map->textures[i++]);
		free(app->map);
		app->map = NULL;
	}
	free(app->player);
	app->player = NULL;
	free(app->frame);
	app->frame = NULL;
}
