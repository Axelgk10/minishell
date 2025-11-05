/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 13:04:21 by axgimene          #+#    #+#             */
/*   Updated: 2025/04/23 13:20:30 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	const char	*last_ocurrance = NULL;

	while (*s)
	{
		if (*s == (char)c)
			last_ocurrance = s;
		s++;
	}
	if ((char)c == '\0')
		return ((char *)s);
	return ((char *)last_ocurrance);
}
