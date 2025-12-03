/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:36:36 by gguardam          #+#    #+#             */
/*   Updated: 2025/05/12 14:36:37 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned
int, char))
{
	int		i;
	char	*stringy;
	int		len;

	if (!s || !f)
		return (NULL);
	len = 0;
	while (s[len])
		len++;
	stringy = malloc((len + 1) * sizeof(char));
	if (!stringy)
		return (NULL);
	i = 0;
	while (s[i])
	{
		stringy[i] = f(i, s[i]);
		i++;
	}
	stringy[i] = '\0';
	return (stringy);
}
