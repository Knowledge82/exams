#include <unistd.h>
#include <stdlib.h>
//#include <stdio.h>
//#include <fcntl.h>
//#define BUFFER_SIZE 42

int	ft_strlen(const char *str)
{
	int	len;

	len = 0;
	while (str[len])
		len++;
	return (len);
}

/*
** ft_strchr - находит первое вхождение символа `c` в строке `s`.
**
** @param s: указатель на строку, в которой ищем.
** @param c: символ, который ищем (преобразуется в `char`).
** @return: указатель на найденный символ в строке или NULL, если его нет.
*/
char	*ft_strchr(const char *str, int c)
{
	char	ch;

	ch = (char)c;
	while (*str)
	{
		if (*str == ch)
			return ((char *)str);
		str++;
	}
	if (ch == '\0')
		return ((char *)str);
	return (NULL);
}

/*
** ft_strjoin - объединяет две строки в одну новую строку.
**
** @param s1: первая строка.
** @param s2: вторая строка.
** @return: указатель на новую строку или NULL, если выделение памяти не удалось.
*/
char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*new_str;
	size_t	i;
	size_t	j;
	size_t	len1;
	size_t	len2;

	if (!s1 || !s2)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	new_str = (char *)malloc(sizeof(char) * (len1 + len2 + 1));
	if (!new_str)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		new_str[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2[j])
	{
		new_str[i + j] = s2[j];
		j++;
	}
	new_str[i + j] = '\0';
	return (new_str);
}

/*
** ft_substr - создаёт новую строку, содержащую копию части строки `str`.
**
** str: исходная строка.
** start: начальная позиция в `str`, откуда копировать.
** len: количество символов для копирования.
** @return: указатель на новую строку или NULL, если выделение памяти не удалось.
*/
char	*ft_substr(char const *str, unsigned int start, size_t len)
{
	char	*substr;
	size_t	i;
	size_t	str_len;

	if (!str)
		return (NULL);
	str_len = ft_strlen(str);
	if (start >= str_len)
	{
		substr = (char *)malloc(sizeof(char) * 1);
		if (!substr)
			return (NULL);
		substr[0] = '\0';
		return (substr);
	}
	if (len > str_len - start)
		len = str_len - start;
	substr = (char *)malloc(sizeof(char) * (len + 1));
	if (!substr)
		return (NULL);
	 i = 0;
	 while (i < len)
	 {
		substr[i] = str[start + i];
		i++;
	 }
	 substr[i] = '\0';
	 return (substr);
}


char	*read_buffer(int fd)
{
	char	*buffer;
	ssize_t bytes_read;

	buffer = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buffer)
		return (NULL);
	bytes_read = read(fd, buffer, BUFFER_SIZE);
	if (bytes_read <= 0)
	{
		free(buffer);
		return (NULL);
	}
	buffer[bytes_read] = '\0';
	return (buffer);
}

char	*store_buffer(int fd, char *stored)
{
	char	*buffer;
	char	*temp;

	while (!stored || !ft_strchr(stored, '\n'))
	{
		buffer = read_buffer(fd);
		if (!buffer)
		{
			if (!stored || !*stored)
			{
				free(stored);
				return (NULL);
			}
			return (stored);
		}
		if (!stored)
			return (buffer);
		temp = ft_strjoin(stored, buffer);
		free(stored);
		free(buffer);
		if (!temp)
			return (NULL);
		stored = temp;
	}
	return (stored);
}

char	*extract_line(char *stored)
{
	int	i;
	char	*line;

	if (!stored || !stored[0])
		return (NULL);
	i = 0;
	while (stored[i] && stored[i] != '\n')
		i++;
	if (stored[i] == '\n')
		line = ft_substr(stored, 0, i + 1);
	else
		line = ft_substr(stored, 0, i);
	return (line);
}

char	*update_stored(char *stored)
{
	char	*new_stored;
	int	i;

	i = 0;
	while (stored[i] && stored[i] != '\n')
		i++;
	if (!stored[i])
	{
		free(stored);
		return (NULL);
	}
	new_stored = ft_substr(stored, i + 1, ft_strlen(stored) - i - 1);
	free(stored);
	if (!new_stored)
		return (NULL);
	return (new_stored);
}

char	*get_next_line(int fd)
{
	static char	*stored;
	char	*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	stored = store_buffer(fd, stored);
	if (!stored)
		return (NULL);
	line = extract_line(stored);
	stored = update_stored(stored);
	return (line);
}
/*
void test_stdin()
{
    char *line;
    int i = 1;

    printf("\n⌨️ Введите строки (Ctrl+D для выхода):\n");
    while ((line = get_next_line(0)))
    {
        printf("💬 Введено %d: %s", i, line);
        free(line);
        i++;
    }
    printf("✅ Чтение из stdin завершено.\n");
}

int main()
{
	test_stdin();
}*/

