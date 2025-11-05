/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 15:40:16 by axgimene          #+#    #+#             */
/*   Updated: 2025/05/09 18:25:15 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_handle_format(va_list args, char specifier)
{
	int	count;

	count = 0;
	if (specifier == 'c')
		count = ft_print_char(va_arg(args, int));
	else if (specifier == 's')
		count = ft_print_str(va_arg(args, char *));
	else if (specifier == 'p')
		count = ft_print_ptr(va_arg(args, void *));
	else if (specifier == 'd' || specifier == 'i')
		count = ft_print_nbr(va_arg(args, int));
	else if (specifier == 'u')
		count = ft_print_unsigned(va_arg(args, unsigned int));
	else if (specifier == 'x')
		count = ft_print_hex_lower(va_arg(args, unsigned int));
	else if (specifier == 'X')
		count = ft_print_hex_upper(va_arg(args, unsigned int));
	else if (specifier == '%')
		count = ft_print_percent();
	return (count);
}

int	ft_printf(const char *format, ...)
{
	va_list	args;
	int		count;
	int		i;

	va_start(args, format);
	count = 0;
	i = 0;
	while (format[i])
	{
		if (format[i] == '%')
		{
			i++;
			count += ft_handle_format(args, format[i]);
		}
		else
			count += write(1, &format[i], 1);
		i++;
	}
	va_end(args);
	return (count);
}
