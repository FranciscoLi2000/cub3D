#include "cub3d.h"

size_t	ft_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (s && s[len])
		len++;
	return (len);
}

int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*dst;
	size_t			i;

	dst = (unsigned char *)s;
	i = 0;
	while (i < n)
		dst[i++] = (unsigned char)c;
	return (s);
}

void	*ft_memcpy(void *dst, const void *src, size_t n)
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

void	*ft_calloc(size_t count, size_t size)
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

char	*ft_strdup(const char *s)
{
	char	*dup;
	size_t	len;

	len = ft_strlen(s);
	dup = malloc(len + 1);
	if (!dup)
		return (NULL);
	ft_memcpy(dup, s, len + 1);
	return (dup);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && (s1[i] || s2[i]))
	{
		if ((unsigned char)s1[i] != (unsigned char)s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}

char	*ft_strchr(const char *s, int c)
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

int	ft_putstr_fd(const char *s, int fd)
{
	if (!s)
		return (0);
	write(fd, s, ft_strlen(s));
	return (0);
}

int	ft_atoi(const char *s)
{
	int	sign;
	int	n;

	sign = 1;
	n = 0;
	while (*s == ' ' || (*s >= 9 && *s <= 13))
		s++;
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			sign = -1;
		s++;
	}
	while (ft_isdigit(*s))
		n = n * 10 + (*s++ - '0');
	return (n * sign);
}

char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*out;
	size_t	len1;
	size_t	len2;

	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	out = malloc(len1 + len2 + 1);
	if (!out)
		return (NULL);
	ft_memcpy(out, s1, len1);
	ft_memcpy(out + len1, s2, len2 + 1);
	return (out);
}

char	*ft_substr(const char *s, unsigned int start, size_t len)
{
	char	*out;
	size_t	s_len;

	s_len = ft_strlen(s);
	if (start >= s_len)
		return (ft_strdup(""));
	if (len > s_len - start)
		len = s_len - start;
	out = malloc(len + 1);
	if (!out)
		return (NULL);
	ft_memcpy(out, s + start, len);
	out[len] = '\0';
	return (out);
}

static size_t	ft_word_count(const char *s, char c)
{
	size_t	count;
	int		in_word;

	count = 0;
	in_word = 0;
	while (*s)
	{
		if (*s != c && !in_word)
		{
			in_word = 1;
			count++;
		}
		else if (*s == c)
			in_word = 0;
		s++;
	}
	return (count);
}

char	**ft_split(const char *s, char c)
{
	char	**out;
	size_t	i;
	size_t	start;
	size_t	j;
	size_t	words;

	if (!s)
		return (NULL);
	words = ft_word_count(s, c);
	out = ft_calloc(words + 1, sizeof(char *));
	if (!out)
		return (NULL);
	i = 0;
	j = 0;
	while (s[i])
	{
		while (s[i] == c)
			i++;
		start = i;
		while (s[i] && s[i] != c)
			i++;
		if (i > start)
		{
			out[j] = ft_substr(s, (unsigned int)start, i - start);
			if (!out[j])
			{
				ft_free_split(out);
				return (NULL);
			}
			j++;
		}
	}
	return (out);
}

void	ft_free_split(char **split)
{
	size_t	i;

	if (!split)
		return ;
	i = 0;
	while (split[i])
		free(split[i++]);
	free(split);
}

char	*ft_strtrim(const char *s)
{
	size_t	start;
	size_t	end;

	if (!s)
		return (NULL);
	start = 0;
	while (s[start] == ' ' || s[start] == '\t' || s[start] == '\n'
		|| s[start] == '\r' || s[start] == '\v' || s[start] == '\f')
		start++;
	end = ft_strlen(s);
	while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t'
			|| s[end - 1] == '\n' || s[end - 1] == '\r'
			|| s[end - 1] == '\v' || s[end - 1] == '\f'))
		end--;
	return (ft_substr(s, (unsigned int)start, end - start));
}
