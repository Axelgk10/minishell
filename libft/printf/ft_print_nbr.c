/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_nbr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 18:13:22 by axgimene          #+#    #+#             */
/*   Updated: 2025/05/09 18:34:29 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_print_nbr(int num)
{
	int	cont;

	cont = 0;
	if (num == -2147483648)
	{
		cont += write (1, "-2147483648", 11);
		return (cont);
	}
	if (num < 0)
	{
		cont += write (1, "-", 1);
		num = -num;
	}
	if (num >= 10)
	{
		cont += ft_print_nbr (num / 10);
	}
	cont += ft_print_char((num % 10) + '0');
	return (cont);
}
