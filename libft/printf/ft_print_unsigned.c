/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_unsigned.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:38:24 by axgimene          #+#    #+#             */
/*   Updated: 2025/05/09 16:39:36 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_print_unsigned(unsigned int num)
{
	char	buffer[10];
	int		i;
	int		count;

	i = 0;
	count = 0;
	if (num == 0)
	{
		write (1, "0", 1);
		return (1);
	}
	while (num > 0)
	{
		buffer[i++] = (num % 10) + '0';
		num = num / 10;
	}
	while (i > 0)
	{
		write (1, &buffer[--i], 1);
		count++;
	}
	return (count);
}
