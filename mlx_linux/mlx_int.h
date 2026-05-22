#ifndef MLX_INT_H
# define MLX_INT_H

# include "cub3d.h"
# include "x11_compat.h"

typedef struct s_hook
{
	int		mask;
	int		(*hook)();
	void	*param;
}	t_hook;

typedef struct s_win_list
{
	Window				window;
	GC					gc;
	int					width;
	int					height;
	int					(*key_hook)();
	void				*key_param;
	int					(*mouse_hook)();
	void				*mouse_param;
	int					(*expose_hook)();
	void				*expose_param;
	t_hook				hooks[36];
	struct s_win_list	*next;
	struct s_xvar		*xvar;
}	t_win_list;

typedef struct s_img
{
	t_ximage	*image;
	char		*data;
	int			bpp;
	int			size_line;
	int			endian;
	int			width;
	int			height;
	GC			gc;
	struct s_xvar	*xvar;
}	t_img;

typedef struct s_xvar
{
	Display		*display;
	Window		root;
	int			screen;
	GC			gc;
	Visual		*visual;
	Colormap	cmap;
	int			depth;
	int			private_cmap;
	t_win_list	*win_list;
	int			(*loop_hook)();
	void		*loop_param;
	int			do_flush;
	Atom		wm_delete_window;
	Atom		wm_protocols;
	int			end_loop;
}	t_xvar;

int		mlx_int_destroy_window(t_xvar *xvar, t_win_list *win);
t_win_list	*mlx_int_find_window(t_xvar *xvar, Window window);
void	mlx_int_event_dispatch(t_xvar *xvar, t_xevent *event);
int		mlx_int_xpm_load(t_xvar *xvar, char **xpm_data, int *width,
			int *height, t_img **out_img);
int		mlx_int_xpm_load_file(t_xvar *xvar, char *filename, int *width,
			int *height, t_img **out_img);
char	*mlx_int_strjoin(const char *s1, const char *s2);
size_t	mlx_int_strlen(const char *s);
int		mlx_int_atoi(const char *s);
char	*mlx_int_strdup(const char *s);
void	*mlx_int_calloc(size_t count, size_t size);
int		mlx_int_put_pixel(t_img *img, int x, int y, unsigned int color);

#endif
