/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_hex_upper.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 11:35:52 by axgimene          #+#    #+#             */
/*   Updated: 2025/05/09 16:39:13 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_print_hex_upper(unsigned int num)
{
	char	buffer[8];
	char	*digits;
	int		i;
	int		count;

	digits = "0123456789ABCDEF";
	i = 0;
	count = 0;
	if (num == 0)
	{
		count += write (1, "0", 1);
		return (count);
	}
	while (num != 0)
	{
		buffer[i++] = digits[num % 16];
		num /= 16;
	}
	while (i > 0)
		count += write (1, &buffer[--i], 1);
	return (count);
}
