/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_hex_lower.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 10:44:53 by axgimene          #+#    #+#             */
/*   Updated: 2025/05/09 16:48:37 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_print_hex_lower(unsigned int num)
{
	char	buffer[8];
	char	*digits;
	int		count;
	int		i;

	i = 0;
	digits = "0123456789abcdef";
	count = 0;
	if (num == 0)
	{
		count += write (1, "0", 1);
		return (count);
	}
	while (num != 0)
	{
		buffer[i++] = digits [num % 16];
		num /= 16;
	}
	while (i > 0)
	{
		count += write (1, &buffer[--i], 1);
	}
	return (count);
}
