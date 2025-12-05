/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 15:08:19 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 19:16:58 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	copy_strings(char *new_str, char const *s1, char const *s2)
{
	size_t	i;
	size_t	fst_len;

	fst_len = ft_strlen(s1);
	i = 0;
	while (i < fst_len)
	{
		new_str[i] = s1[i];
		i++;
	}
	i = 0;
	while (s2[i])
	{
		new_str[fst_len + i] = s2[i];
		i++;
	}
	new_str[fst_len + i] = '\0';
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	total_len;
	char	*new_str;

	if (!s1 || !s2)
		return (NULL);
	total_len = ft_strlen(s1) + ft_strlen(s2) + 1;
	new_str = malloc(total_len);
	if (!new_str)
		return (NULL);
	copy_strings(new_str, s1, s2);
	return (new_str);
}
