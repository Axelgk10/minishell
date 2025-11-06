/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoi_char.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 15:40:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/06 15:34:01 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin_char(char const *s, char c)
{
    char	*result;
    size_t	len;
    size_t	i;

    if (!s)
        return (NULL);
    len = ft_strlen(s);
    result = malloc(sizeof(char) * (len + 2));
    if (!result)
        return (NULL);
    i = 0;
    while (i < len)
    {
        result[i] = s[i];
        i++;
    }
    result[i] = c;
    result[i + 1] = '\0';
    return (result);
}