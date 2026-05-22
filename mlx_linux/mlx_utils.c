#include "mlx_int.h"

static void	*mlx_int_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char	*d;
	const unsigned char	*s;
	size_t			i;

	d = (unsigned char *)dst;
	s = (const unsigned char *)src;
	i = 0;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return (dst);
}

static void	*mlx_int_memset(void *s, int c, size_t n)
{
	unsigned char	*dst;
	size_t			i;

	dst = (unsigned char *)s;
	i = 0;
	while (i < n)
		dst[i++] = (unsigned char)c;
	return (s);
}

int	mlx_int_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

char	*mlx_int_strchr(const char *s, int c)
{
	while (*s)
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if (c == '\0')
		return ((char *)s);
	return (NULL);
}

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
	mlx_int_memcpy(dup, s, len + 1);
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
	mlx_int_memset(ptr, 0, count * size);
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
	while (mlx_int_isdigit(*s))
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
	mlx_int_memcpy(out, s1, len1);
	mlx_int_memcpy(out + len1, s2, len2 + 1);
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
