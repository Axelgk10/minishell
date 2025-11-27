/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 12:41:06 by gguardam          #+#    #+#             */
/*   Updated: 2025/08/24 17:13:27 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	white_spaces(char charsy)
{
	if (charsy == ' ' || charsy == '\t' || charsy == '\n' || charsy == '\v')
		return (1);
	else if (charsy == '\f' || charsy == '\r')
		return (1);
	else
		return (0);
}

static int	sign_control(char charsy)
{
	if (charsy == '-')
		return (-1);
	else if (charsy == '+')
		return (1);
	else
		return (0);
}

float	rescursive_translator(char *str, float *res, int *i)
{
	float	decimal;
	int		dot;

	decimal = 0.1;
	dot = 0;
	while ((str[i] && ft_isdigit(str[i])) || (str[i] == '.' && dot == 0))
	{
		if (str[i] == '.')
			dot = 1;
		else if (dot == 0)
			res = res * 10 + (str[i] - '0');
		else if (dot != 0)
		{
			res += (str[i] - '0') * decimal;
			decimal /= 10;
		}
		i++;
	}
	return (res);
}

float	ft_atof(const char *str)
{
	float	res;
	int		sign;
	int		i;

	res = 0;
	sign = 1;
	i = 0;
	while (white_spaces(str[i]))
		i++;
	if (sign_control(str[i]) == -1 || sign_control(str[i]) == 1)
		sign = sign_control(str[i++]);
	res = rescursive_translator(str, dot, res, decimal, i);
	return (res * sign);
}
