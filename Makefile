NAME		= cub3D
MLX_NAME	= libmlx.a

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -Iincludes -Imlx_linux
AR			= ar rcs
RM			= rm -f

MLX_SRC		= mlx_linux/mlx_init.c \
			  mlx_linux/mlx_window.c \
			  mlx_linux/mlx_image.c \
			  mlx_linux/mlx_events.c \
			  mlx_linux/mlx_loop.c \
			  mlx_linux/mlx_misc.c \
			  mlx_linux/mlx_utils.c

SRC			= srcs/main.c \
			  srcs/app.c \
			  srcs/error.c \
			  srcs/utils.c

OBJ			= $(SRC:.c=.o)
MLX_OBJ		= $(MLX_SRC:.c=.o)

all: $(MLX_NAME) $(NAME)

$(MLX_NAME): $(MLX_OBJ)
	$(AR) $@ $^

$(NAME): $(OBJ) $(MLX_NAME)
	$(CC) $(CFLAGS) $(OBJ) -L. -lmlx \
		-L/lib/x86_64-linux-gnu -l:libX11.so.6 -l:libXext.so.6 -o $(NAME)

clean:
	$(RM) $(OBJ) $(MLX_OBJ)

fclean: clean
	$(RM) $(NAME) $(MLX_NAME)

re: fclean all

bonus: all

.PHONY: all clean fclean re bonus
