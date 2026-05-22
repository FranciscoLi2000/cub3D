#include "cub3d.h"
#include <math.h>
#include <sys/time.h>

static double	now_seconds(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((double)tv.tv_sec + (double)tv.tv_usec / 1000000.0);
}

static int	is_wall(t_app *app, double x, double y)
{
	int	map_x;
	int	map_y;

	map_x = (int)x;
	map_y = (int)y;
	if (!app->map || !app->map->grid)
		return (1);
	if (map_x < 0 || map_y < 0 || map_y >= app->map->height
		|| map_x >= app->map->width)
		return (1);
	return (app->map->grid[map_y][map_x] == '1');
}

int	app_draw_pixel(t_image *img, int x, int y, int color)
{
	char	*dst;

	if (!img || !img->data || x < 0 || y < 0 || x >= img->width
		|| y >= img->height)
		return (0);
	dst = img->data + y * img->size_line + x * (img->bpp / 8);
	*(unsigned int *)dst = (unsigned int)color;
	return (0);
}

void	app_destroy_frame(t_app *app)
{
	if (!app || !app->frame)
		return ;
	if (app->frame->ptr && app->mlx)
		mlx_destroy_image(app->mlx, app->frame->ptr);
	app->frame->ptr = NULL;
	app->frame->data = NULL;
}

void	app_destroy_textures(t_app *app)
{
	int	i;

	if (!app)
		return ;
	i = 0;
	while (i < 4)
	{
		if (app->textures[i].ptr && app->mlx)
			mlx_destroy_image(app->mlx, app->textures[i].ptr);
		app->textures[i].ptr = NULL;
		app->textures[i].data = NULL;
		i++;
	}
}

int	app_create_frame(t_app *app)
{
	if (!app || !app->frame)
		return (1);
	if (!app->mlx)
		return (ft_error("mlx context missing"));
	app->frame->ptr = mlx_new_image(app->mlx, app->width, app->height);
	if (!app->frame->ptr)
		return (ft_error("frame allocation failed"));
	app->frame->data = mlx_get_data_addr(app->frame->ptr, &app->frame->bpp,
			&app->frame->size_line, &app->frame->endian);
	app->frame->width = app->width;
	app->frame->height = app->height;
	return (0);
}

int	app_load_textures(t_app *app)
{
	int	i;
	int	w;
	int	h;

	i = 0;
	while (i < 4)
	{
		if (!app->map || !app->map->textures[i])
			return (ft_error("texture path missing"));
		app->textures[i].ptr = mlx_xpm_file_to_image(app->mlx,
				app->map->textures[i], &w, &h);
		if (!app->textures[i].ptr)
			return (ft_error("texture loading failed"));
		app->textures[i].data = mlx_get_data_addr(app->textures[i].ptr,
				&app->textures[i].bpp, &app->textures[i].size_line,
				&app->textures[i].endian);
		app->textures[i].width = w;
		app->textures[i].height = h;
		if (!app->textures[i].data)
			return (ft_error("texture data failed"));
		i++;
	}
	return (0);
}

static void	clear_frame(t_app *app)
{
	int	x;
	int	y;
	int	color;

	y = 0;
	while (y < app->height)
	{
		x = 0;
		while (x < app->width)
		{
			if (y < app->height / 2)
				color = app->map->ceiling_color;
			else
				color = app->map->floor_color;
			app_draw_pixel(app->frame, x, y, color);
			x++;
		}
		y++;
	}
}

static int	texture_index(int side, double ray_dir_x, double ray_dir_y)
{
	if (side == 0 && ray_dir_x > 0)
		return (TEX_WE);
	if (side == 0 && ray_dir_x < 0)
		return (TEX_EA);
	if (side == 1 && ray_dir_y > 0)
		return (TEX_NO);
	return (TEX_SO);
}

static void	draw_column(t_app *app, int screen_x)
{
	double	camera_x;
	double	ray_dir_x;
	double	ray_dir_y;
	int		map_x;
	int		map_y;
	double	delta_dist_x;
	double	delta_dist_y;
	int		step_x;
	int		step_y;
	double	side_dist_x;
	double	side_dist_y;
	int		hit;
	int		side;
	double	perp_wall_dist;
	int		line_height;
	int		draw_start;
	int		draw_end;
	double	wall_x;
	int		tex_num;
	t_image	*tex;
	int		tex_x;
	double	step;
	double	tex_pos;
	int		y;
	int		tex_y;
	unsigned int	color;

	camera_x = 2.0 * screen_x / (double)app->width - 1.0;
	ray_dir_x = app->player->dir_x + app->player->plane_x * camera_x;
	ray_dir_y = app->player->dir_y + app->player->plane_y * camera_x;
	map_x = (int)app->player->x;
	map_y = (int)app->player->y;
	if (ray_dir_x == 0)
		delta_dist_x = 1e30;
	else
		delta_dist_x = fabs(1.0 / ray_dir_x);
	if (ray_dir_y == 0)
		delta_dist_y = 1e30;
	else
		delta_dist_y = fabs(1.0 / ray_dir_y);
	if (ray_dir_x < 0)
	{
		step_x = -1;
		side_dist_x = (app->player->x - map_x) * delta_dist_x;
	}
	else
	{
		step_x = 1;
		side_dist_x = (map_x + 1.0 - app->player->x) * delta_dist_x;
	}
	if (ray_dir_y < 0)
	{
		step_y = -1;
		side_dist_y = (app->player->y - map_y) * delta_dist_y;
	}
	else
	{
		step_y = 1;
		side_dist_y = (map_y + 1.0 - app->player->y) * delta_dist_y;
	}
	hit = 0;
	side = 0;
	while (!hit)
	{
		if (side_dist_x < side_dist_y)
		{
			side_dist_x += delta_dist_x;
			map_x += step_x;
			side = 0;
		}
		else
		{
			side_dist_y += delta_dist_y;
			map_y += step_y;
			side = 1;
		}
		if (map_x < 0 || map_y < 0 || map_y >= app->map->height
			|| map_x >= app->map->width || app->map->grid[map_y][map_x] == '1')
			hit = 1;
	}
	if (side == 0)
		perp_wall_dist = side_dist_x - delta_dist_x;
	else
		perp_wall_dist = side_dist_y - delta_dist_y;
	if (perp_wall_dist <= 0.0001)
		perp_wall_dist = 0.0001;
	line_height = (int)(app->height / perp_wall_dist);
	draw_start = -line_height / 2 + app->height / 2;
	if (draw_start < 0)
		draw_start = 0;
	draw_end = line_height / 2 + app->height / 2;
	if (draw_end >= app->height)
		draw_end = app->height - 1;
	if (side == 0)
		wall_x = app->player->y + perp_wall_dist * ray_dir_y;
	else
		wall_x = app->player->x + perp_wall_dist * ray_dir_x;
	wall_x -= floor(wall_x);
	tex_num = texture_index(side, ray_dir_x, ray_dir_y);
	tex = &app->textures[tex_num];
	if (!tex->data || tex->width <= 0 || tex->height <= 0)
		return ;
	tex_x = (int)(wall_x * (double)tex->width);
	if (tex_x < 0)
		tex_x = 0;
	if (tex_x >= tex->width)
		tex_x = tex->width - 1;
	if (side == 0 && ray_dir_x < 0)
		tex_x = tex->width - tex_x - 1;
	if (side == 1 && ray_dir_y > 0)
		tex_x = tex->width - tex_x - 1;
	step = 1.0 * tex->height / line_height;
	tex_pos = (draw_start - app->height / 2 + line_height / 2) * step;
	y = draw_start;
	while (y <= draw_end)
	{
		tex_y = (int)tex_pos;
		if (tex_y < 0)
			tex_y = 0;
		if (tex_y >= tex->height)
			tex_y = tex->height - 1;
		tex_pos += step;
		color = *(unsigned int *)(tex->data + tex_y * tex->size_line
				+ tex_x * (tex->bpp / 8));
		if (side == 1)
			color = (color >> 1) & 0x7F7F7F;
		app_draw_pixel(app->frame, screen_x, y, (int)color);
		y++;
	}
}

int	app_render(t_app *app)
{
	int	x;

	if (!app || !app->frame || !app->frame->data)
		return (1);
	clear_frame(app);
	x = 0;
	while (x < app->width)
	{
		draw_column(app, x);
		x++;
	}
	mlx_put_image_to_window(app->mlx, app->win, app->frame->ptr, 0, 0);
	return (0);
}

int	app_move_player(t_app *app, double dt)
{
	double	move_speed;
	double	rot_speed;
	double	new_x;
	double	new_y;
	double	old_dir_x;
	double	old_plane_x;
	double	strafe_x;
	double	strafe_y;

	if (!app || !app->player || !app->map)
		return (1);
	move_speed = 3.0 * dt;
	rot_speed = 2.0 * dt;
	new_x = app->player->x;
	new_y = app->player->y;
	if (app->player->move_forward)
	{
		if (!is_wall(app, new_x + app->player->dir_x * move_speed, new_y))
			new_x += app->player->dir_x * move_speed;
		if (!is_wall(app, new_x, new_y + app->player->dir_y * move_speed))
			new_y += app->player->dir_y * move_speed;
	}
	if (app->player->move_backward)
	{
		if (!is_wall(app, new_x - app->player->dir_x * move_speed, new_y))
			new_x -= app->player->dir_x * move_speed;
		if (!is_wall(app, new_x, new_y - app->player->dir_y * move_speed))
			new_y -= app->player->dir_y * move_speed;
	}
	strafe_x = app->player->dir_y;
	strafe_y = -app->player->dir_x;
	if (app->player->move_left)
	{
		if (!is_wall(app, new_x - strafe_x * move_speed, new_y))
			new_x -= strafe_x * move_speed;
		if (!is_wall(app, new_x, new_y - strafe_y * move_speed))
			new_y -= strafe_y * move_speed;
	}
	if (app->player->move_right)
	{
		if (!is_wall(app, new_x + strafe_x * move_speed, new_y))
			new_x += strafe_x * move_speed;
		if (!is_wall(app, new_x, new_y + strafe_y * move_speed))
			new_y += strafe_y * move_speed;
	}
	app->player->x = new_x;
	app->player->y = new_y;
	if (app->player->turn_left || app->player->turn_right)
	{
		if (app->player->turn_left)
			rot_speed = -rot_speed;
		if (app->player->turn_right)
			rot_speed = -rot_speed;
		old_dir_x = app->player->dir_x;
		app->player->dir_x = app->player->dir_x * cos(rot_speed)
			- app->player->dir_y * sin(rot_speed);
		app->player->dir_y = old_dir_x * sin(rot_speed)
			+ app->player->dir_y * cos(rot_speed);
		old_plane_x = app->player->plane_x;
		app->player->plane_x = app->player->plane_x * cos(rot_speed)
			- app->player->plane_y * sin(rot_speed);
		app->player->plane_y = old_plane_x * sin(rot_speed)
			+ app->player->plane_y * cos(rot_speed);
	}
	return (0);
}

int	app_loop(void *param)
{
	t_app		*app;
	static double	last;
	double			now;
	double			dt;

	app = (t_app *)param;
	now = now_seconds();
	if (last == 0)
		last = now;
	dt = now - last;
	last = now;
	app_move_player(app, dt);
	app_render(app);
	return (0);
}

int	app_key_press(int keycode, void *param)
{
	t_app	*app;

	app = (t_app *)param;
	if (!app || !app->player)
		return (0);
	if (keycode == KEY_ESC)
		return (app_close(app));
	if (keycode == KEY_W)
		app->player->move_forward = 1;
	else if (keycode == KEY_S)
		app->player->move_backward = 1;
	else if (keycode == KEY_A)
		app->player->move_left = 1;
	else if (keycode == KEY_D)
		app->player->move_right = 1;
	else if (keycode == KEY_LEFT_ARROW)
		app->player->turn_left = 1;
	else if (keycode == KEY_RIGHT_ARROW)
		app->player->turn_right = 1;
	return (0);
}

int	app_key_release(int keycode, void *param)
{
	t_app	*app;

	app = (t_app *)param;
	if (!app || !app->player)
		return (0);
	if (keycode == KEY_W)
		app->player->move_forward = 0;
	else if (keycode == KEY_S)
		app->player->move_backward = 0;
	else if (keycode == KEY_A)
		app->player->move_left = 0;
	else if (keycode == KEY_D)
		app->player->move_right = 0;
	else if (keycode == KEY_LEFT_ARROW)
		app->player->turn_left = 0;
	else if (keycode == KEY_RIGHT_ARROW)
		app->player->turn_right = 0;
	return (0);
}
