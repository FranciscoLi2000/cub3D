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
# define X11_DESTROY_NOTIFY 17
# define X11_CLIENT_MESSAGE 33
# define X11_STRUCTURE_NOTIFY_MASK (1L << 17)

typedef struct s_app
{
	void	*mlx;
	void	*win;
	int		width;
	int		height;
	int		running;
}	t_app;

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
int		scene_validate_path(const char *path);
int		app_init(t_app *app);
int		app_run(t_app *app);
int		app_close(t_app *app);
int		app_key_hook(int keycode, void *param);
int		app_close_hook(void *param);

#endif
