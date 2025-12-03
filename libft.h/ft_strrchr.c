/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 12:26:04 by gguardam          #+#    #+#             */
/*   Updated: 2025/05/03 16:54:07 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stddef.h>

char	*ft_strrchr(const char *s, int c)
{
	const char	*last_c;
	char		ch;

	last_c = NULL;
	ch = (char)c;
	while (*s != '\0')
	{
		if (*s == ch)
			last_c = s;
		s++;
	}
	if (ch == '\0')
		return ((char *)s);
	return ((char *)last_c);
}
