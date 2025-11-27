/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 16:21:19 by gguardam          #+#    #+#             */
/*   Updated: 2025/05/06 12:17:46 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*string1;
	const unsigned char	*string2;
	size_t				i;

	string1 = s1;
	string2 = s2;
	i = 0;
	if (n == 0)
		return (0);
	while (i < n - 1 && *string1 == *string2)
	{
		string1++;
		string2++;
		i++;
	}
	return (*string1 - *string2);
}
