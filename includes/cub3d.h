#ifndef CUB3D_H
# define CUB3D_H

# include <fcntl.h>
# include <stddef.h>
# include <stdlib.h>
# include <unistd.h>

# include "mlx.h"

# define WINDOW_WIDTH 1280
# define WINDOW_HEIGHT 720
# define KEY_ESC 65307
# define KEY_LEFT 65361
# define KEY_RIGHT 65363
# define KEY_W 'w'
# define KEY_A 'a'
# define KEY_S 's'
# define KEY_D 'd'
# define KEY_LEFT_ARROW 65361
# define KEY_RIGHT_ARROW 65363
# define KEY_UP_ARROW 65362
# define KEY_DOWN_ARROW 65364
# define X11_KEY_PRESS 2
# define X11_KEY_RELEASE 3
# define X11_DESTROY_NOTIFY 17
# define X11_CLIENT_MESSAGE 33
# define X11_STRUCTURE_NOTIFY_MASK (1L << 17)
# define X11_KEY_PRESS_MASK (1L << 0)
# define X11_KEY_RELEASE_MASK (1L << 1)
# define TEX_NO 0
# define TEX_SO 1
# define TEX_WE 2
# define TEX_EA 3

typedef struct s_image
{
	void	*ptr;
	char	*data;
	int		bpp;
	int		size_line;
	int		endian;
	int		width;
	int		height;
}	t_image;

typedef struct s_app
{
	void	*mlx;
	void	*win;
	int		width;
	int		height;
	int		running;
	struct s_map	*map;
	struct s_player	*player;
	struct s_image	*frame;
	t_image		textures[4];
}	t_app;

typedef struct s_player
{
	double	x;
	double	y;
	double	dir_x;
	double	dir_y;
	double	plane_x;
	double	plane_y;
	int		move_forward;
	int		move_backward;
	int		move_left;
	int		move_right;
	int		turn_left;
	int		turn_right;
}	t_player;

typedef struct s_map
{
	char	**grid;
	int		width;
	int		height;
	char	*textures[4];
	int		floor_color;
	int		ceiling_color;
	int		player_count;
}	t_map;

typedef struct s_scene
{
	t_map		map;
	t_player	player;
	t_image		textures[4];
}	t_scene;

size_t	ft_strlen(const char *s);
int		ft_isdigit(int c);
void	*ft_memset(void *s, int c, size_t n);
void	*ft_memcpy(void *dst, const void *src, size_t n);
void	*ft_calloc(size_t count, size_t size);
char	*ft_strdup(const char *s);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strchr(const char *s, int c);
int		ft_putstr_fd(const char *s, int fd);
int		ft_error(const char *msg);
int		ft_atoi(const char *s);
char	*ft_strjoin(const char *s1, const char *s2);
char	*ft_substr(const char *s, unsigned int start, size_t len);
char	**ft_split(const char *s, char c);
void	ft_free_split(char **split);
char	*ft_strtrim(const char *s);
int		scene_parse(t_app *app, const char *path);
void	scene_destroy(t_app *app);
int		app_init(t_app *app);
int		app_run(t_app *app);
int		app_close(t_app *app);
int		app_loop(void *param);
int		app_key_press(int keycode, void *param);
int		app_key_release(int keycode, void *param);
int		app_close_hook(void *param);
int		app_render(t_app *app);
int		app_prepare(t_app *app);
int		app_move_player(t_app *app, double dt);
int		app_load_textures(t_app *app);
int		app_create_frame(t_app *app);
void	app_destroy_frame(t_app *app);
void	app_destroy_textures(t_app *app);
int		app_draw_pixel(t_image *img, int x, int y, int color);

#endif
