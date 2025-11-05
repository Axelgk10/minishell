/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_ptr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 10:44:16 by axgimene          #+#    #+#             */
/*   Updated: 2025/05/09 16:51:06 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_print_hex(unsigned long num, int uppercase)
{
	char	buffer[16];
	char	*digits;
	int		i;
	int		count;

	i = 0;
	count = 0;
	if (uppercase)
		digits = "0123456789ABCDEF";
	else
		digits = "0123456789abcdef";
	if (num == 0)
		write (1, "0", 1);
	while (num > 0)
	{
		buffer[i++] = digits[num % 16];
		num /= 16;
	}
	while (i > 0)
	{
		count += write (1, &buffer[--i], 1);
	}
	return (count);
}

int	ft_print_ptr(void	*ptr)
{
	unsigned long	address;
	int				count;

	count = 0;
	if (!ptr)
	{
		count += write (1, "(nil)", 5);
		return (count);
	}
	address = (unsigned long)ptr;
	count += write (1, "0x", 2);
	count += ft_print_hex(address, 0);
	return (count);
}
