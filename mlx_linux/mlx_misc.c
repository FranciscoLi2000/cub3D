#include "mlx_int.h"

int	mlx_string_put(void *mlx_ptr, void *win_ptr, int x, int y, int color,
		char *string)
{
	(void)mlx_ptr;
	(void)win_ptr;
	(void)x;
	(void)y;
	(void)color;
	(void)string;
	return (0);
}

void	mlx_set_font(void *mlx_ptr, void *win_ptr, char *name)
{
	(void)mlx_ptr;
	(void)win_ptr;
	(void)name;
}

static int	mlx_int_put_hex_color(char *src, unsigned int *color)
{
	unsigned int	value;
	int				digit;

	if (src[0] != '#' || ft_strlen(src) < 7)
		return (0);
	value = 0;
	while (*++src)
	{
		if (*src >= '0' && *src <= '9')
			digit = *src - '0';
		else if (*src >= 'a' && *src <= 'f')
			digit = *src - 'a' + 10;
		else if (*src >= 'A' && *src <= 'F')
			digit = *src - 'A' + 10;
		else
			break ;
		value = (value << 4) | (unsigned int)digit;
	}
	*color = value;
	return (1);
}

int	mlx_mouse_get_pos(void *mlx_ptr, void *win_ptr, int *x, int *y)
{
	t_xvar		*xvar;
	t_win_list	*win;
	Window		root;
	Window		child;
	unsigned int	mask;

	xvar = (t_xvar *)mlx_ptr;
	win = (t_win_list *)win_ptr;
	if (!xvar || !win || !x || !y)
		return (0);
	if (!XQueryPointer(xvar->display, win->window, &root, &child, x, y,
			x, y, &mask))
		return (0);
	return (1);
}

int	mlx_mouse_move(void *mlx_ptr, void *win_ptr, int x, int y)
{
	t_xvar		*xvar;
	t_win_list	*win;

	xvar = (t_xvar *)mlx_ptr;
	win = (t_win_list *)win_ptr;
	if (!xvar || !win)
		return (0);
	XWarpPointer(xvar->display, None, win->window, 0, 0, 0, 0, x, y);
	return (0);
}

int	mlx_mouse_hide(void *mlx_ptr, void *win_ptr)
{
	t_xvar	*xvar;
	t_win_list	*win;
	void	*cursor;

	xvar = (t_xvar *)mlx_ptr;
	win = (t_win_list *)win_ptr;
	if (!xvar || !win)
		return (0);
	cursor = XCreateFontCursor(xvar->display, 68);
	XDefineCursor(xvar->display, win->window, cursor);
	XFreeCursor(xvar->display, cursor);
	return (0);
}

int	mlx_mouse_show(void *mlx_ptr, void *win_ptr)
{
	t_xvar	*xvar;
	t_win_list	*win;

	xvar = (t_xvar *)mlx_ptr;
	win = (t_win_list *)win_ptr;
	if (!xvar || !win)
		return (0);
	XDefineCursor(xvar->display, win->window, None);
	return (0);
}

static int	mlx_int_parse_color(const char *line, unsigned int *color)
{
	const char	*pos;

	pos = ft_strchr(line, '#');
	if (!pos)
		return (0);
	return (mlx_int_put_hex_color((char *)pos, color));
}

static int	mlx_int_parse_xpm_header(char *line, int *w, int *h, int *colors,
		int *cpp)
{
	char	*cursor;

	cursor = line;
	while (*cursor == ' ' || *cursor == '\t' || *cursor == '"')
		cursor++;
	*w = mlx_int_atoi(cursor);
	while (ft_isdigit(*cursor))
		cursor++;
	*h = mlx_int_atoi(cursor);
	while (*cursor && !ft_isdigit(*cursor))
		cursor++;
	*colors = mlx_int_atoi(cursor);
	while (ft_isdigit(*cursor))
		cursor++;
	while (*cursor && !ft_isdigit(*cursor))
		cursor++;
	*cpp = mlx_int_atoi(cursor);
	return (*w > 0 && *h > 0 && *colors > 0 && *cpp > 0);
}

static char	*mlx_int_clean_xpm_line(char *line)
{
	size_t	len;

	while (*line == ' ' || *line == '\t')
		line++;
	len = ft_strlen(line);
	while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'
			|| line[len - 1] == '"' || line[len - 1] == ','))
		line[--len] = '\0';
	if (*line == '"')
		line++;
	return (line);
}

static int	mlx_int_read_file(const char *filename, char **content)
{
	int		fd;
	char	buffer[4096];
	ssize_t	rd;
	char	*out;
	char	*tmp;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (0);
	out = ft_strdup("");
	if (!out)
		return (close(fd), 0);
	rd = read(fd, buffer, sizeof(buffer) - 1);
	while (rd > 0)
	{
		buffer[rd] = '\0';
		tmp = mlx_int_strjoin(out, buffer);
		free(out);
		out = tmp;
		if (!out)
			return (close(fd), 0);
		rd = read(fd, buffer, sizeof(buffer) - 1);
	}
	close(fd);
	*content = out;
	return (1);
}

static int	mlx_int_fill_image_from_xpm(t_xvar *xvar, char **lines, int w,
		int h, int colors, int cpp, t_img *img)
{
	int				i;
	int				j;
	unsigned int	palette[256];
	char			keys[256][3];
	char			*line;
	int				index;

	i = -1;
	while (++i < 256)
		palette[i] = 0;
	i = 0;
	while (i < colors)
	{
		line = mlx_int_clean_xpm_line(lines[1 + i]);
		keys[i][0] = line[0];
		keys[i][1] = cpp > 1 ? line[1] : '\0';
		keys[i][2] = '\0';
		if (!mlx_int_parse_color(line, &palette[i]))
			palette[i] = 0;
		i++;
	}
	i = 0;
	while (i < h)
	{
		line = mlx_int_clean_xpm_line(lines[1 + colors + i]);
		j = 0;
		while (j < w)
		{
			index = 0;
			while (index < colors)
			{
				if (cpp == 1 && line[j] == keys[index][0])
					break ;
				if (cpp > 1 && line[j] == keys[index][0]
					&& line[j + 1] == keys[index][1])
					break ;
				index++;
			}
			mlx_int_put_pixel(img, j, i, palette[index]);
			j++;
		}
		i++;
	}
	(void)xvar;
	return (1);
}

int	mlx_int_xpm_load(t_xvar *xvar, char **xpm_data, int *width, int *height,
		t_img **out_img)
{
	t_img	*img;
	int		w;
	int		h;
	int		colors;
	int		cpp;

	if (!xvar || !xpm_data || !xpm_data[0] || !xpm_data[1])
		return (0);
	if (!mlx_int_parse_xpm_header(xpm_data[0], &w, &h, &colors, &cpp))
		return (0);
	img = mlx_new_image(xvar, w, h);
	if (!img)
		return (0);
	if (!mlx_int_fill_image_from_xpm(xvar, xpm_data, w, h, colors, cpp, img))
	{
		mlx_destroy_image(xvar, img);
		return (0);
	}
	if (width)
		*width = w;
	if (height)
		*height = h;
	*out_img = img;
	return (1);
}

int	mlx_int_xpm_load_file(t_xvar *xvar, char *filename, int *width,
		int *height, t_img **out_img)
{
	char	*content;
	char	**lines;
	int		count;
	int		i;
	char	*cursor;
	int		ok;

	content = NULL;
	if (!mlx_int_read_file(filename, &content))
		return (0);
	count = 0;
	i = 0;
	while (content[i])
		if (content[i++] == '\n')
			count++;
	lines = malloc(sizeof(char *) * (count + 2));
	if (!lines)
		return (free(content), 0);
	i = 0;
	cursor = content;
	lines[i++] = cursor;
	while (*cursor)
	{
		if (*cursor == '\n')
		{
			*cursor = '\0';
			if (cursor[1])
				lines[i++] = cursor + 1;
		}
		cursor++;
	}
	lines[i] = NULL;
	ok = mlx_int_xpm_load(xvar, lines, width, height, out_img);
	free(lines);
	free(content);
	return (ok);
}

void	*mlx_xpm_to_image(void *mlx_ptr, char **xpm_data, int *width,
		int *height)
{
	t_xvar	*xvar;
	t_img	*img;

	xvar = (t_xvar *)mlx_ptr;
	img = NULL;
	if (!mlx_int_xpm_load(xvar, xpm_data, width, height, &img))
		return (NULL);
	return (img);
}

void	*mlx_xpm_file_to_image(void *mlx_ptr, char *filename, int *width,
		int *height)
{
	t_xvar	*xvar;
	t_img	*img;

	xvar = (t_xvar *)mlx_ptr;
	img = NULL;
	if (!mlx_int_xpm_load_file(xvar, filename, width, height, &img))
		return (NULL);
	return (img);
}
