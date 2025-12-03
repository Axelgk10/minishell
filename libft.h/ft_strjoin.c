/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 15:08:19 by gguardam          #+#    #+#             */
/*   Updated: 2025/11/24 18:27:30 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	total_len;
	size_t	fst_len;
	size_t	i;
	char	*new_str;

	if (!s1 || !s2)
		return (NULL);
	fst_len = ft_strlen(s1);
	total_len = fst_len + ft_strlen(s2) + 1;
	new_str = malloc(total_len);
	if (!new_str)
		return (NULL);  // ✅ Si malloc falla, retorna NULL sin memory leak
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
	return (new_str);
}
