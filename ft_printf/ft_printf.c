#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>

static void	ft_putstr(char *str, int *len)
{
	int	i;

	i = 0;
	if (!str)
		str = "(null)";
	while (str[i])
	{
		write(1, &str[i], 1);
		i++;
		(*len)++;
	}
}

static void ft_putnum(long long n, unsigned int base, int *len)
{
	if (n < 0)
	{
		n = -n;
		write(1, "-", 1);
		(*len)++;
	}
	if (n >= base)
		ft_putnum(n / base, base, len);
	write(1, &"0123456789abcdef"[n % base], 1);
	(*len)++;
}

int	ft_printf(const char *format, ...)
{
	va_list	args;
	int	i;
	int	len;

	va_start(args, format);
	i = 0;
	len = 0;
	while (format[i])
	{
		if (format[i] == '%' && ((format[i + 1] == 's') || (format[i + 1] == 'd') || (format[i + 1] == 'x')))
		{
			i++;
			if (format[i] == 's')
				ft_putstr(va_arg(args, char *), &len);
			else if (format[i] == 'd')
				ft_putnum(va_arg(args, int), 10, &len);
			else if (format[i] == 'x')
				ft_putnum(va_arg(args, unsigned int), 16, &len);
		}
		else
		{
			write(1, &format[i], 1);
			len++;
		}
		i++;
	}
	va_end(args);
	return (len);
}

int	main()
{
	int	len1, len2;

	//STR
	len1 = ft_printf("Qwerty? %s\n", "Yes, QWERTY!");
	len2 = printf("Qwerty? %s\n", "Yes, QWERTY!");
	printf("ft_printf: %d chars\n", len1);
	printf("printf: %d chars\n", len2);
	//INT
	len1 = ft_printf("How much qwertys? %d\n", 42);
	len2 = printf("How much qwertys? %d\n", 42);
	printf("ft_printf: %d chars\n", len1);
	printf("printf: %d chars\n", len2);
	//HEX
	len1 = ft_printf("255 in hex is %x\n", 255);
	len2 = printf("255 in hex is %x\n", 255);
	printf("ft_printf: %d chars\n", len1);
	printf("printf: %d chars\n", len2);


}
