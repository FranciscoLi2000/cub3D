#include "mlx_int.h"

size_t	mlx_int_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (s && s[len])
		len++;
	return (len);
}

char	*mlx_int_strdup(const char *s)
{
	char	*dup;
	size_t	len;

	len = mlx_int_strlen(s);
	dup = malloc(len + 1);
	if (!dup)
		return (NULL);
	ft_memcpy(dup, s, len + 1);
	return (dup);
}

void	*mlx_int_calloc(size_t count, size_t size)
{
	void	*ptr;

	if (size != 0 && count > ((size_t)-1) / size)
		return (NULL);
	ptr = malloc(count * size);
	if (!ptr)
		return (NULL);
	ft_memset(ptr, 0, count * size);
	return (ptr);
}

int	mlx_int_atoi(const char *s)
{
	int	sign;
	int	n;

	sign = 1;
	n = 0;
	while (*s == ' ' || (*s >= 9 && *s <= 13))
		s++;
	if (*s == '+' || *s == '-')
		if (*s++ == '-')
			sign = -1;
	while (ft_isdigit(*s))
		n = n * 10 + (*s++ - '0');
	return (n * sign);
}

char	*mlx_int_strjoin(const char *s1, const char *s2)
{
	char	*out;
	size_t	len1;
	size_t	len2;

	len1 = mlx_int_strlen(s1);
	len2 = mlx_int_strlen(s2);
	out = malloc(len1 + len2 + 1);
	if (!out)
		return (NULL);
	ft_memcpy(out, s1, len1);
	ft_memcpy(out + len1, s2, len2 + 1);
	return (out);
}

int	mlx_int_put_pixel(t_img *img, int x, int y, unsigned int color)
{
	char	*dst;

	if (!img || x < 0 || y < 0 || x >= img->width || y >= img->height)
		return (0);
	dst = img->data + y * img->size_line + x * (img->bpp / 8);
	*(unsigned int *)dst = color;
	return (0);
}
