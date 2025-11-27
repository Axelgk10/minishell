/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:36:24 by gguardam          #+#    #+#             */
/*   Updated: 2025/05/12 17:06:20 by gguardam         ###   ########.fr       */
/*                                res[i] = mal                                */
/* ************************************************************************** */

#include "libft.h"

static int	ft_count_words(const char *s, char c)
{
	int	i;
	int	words;

	i = 0;
	words = 0;
	while (s[i] != '\0')
	{
		while (s[i] == c)
			i++;
		if (s[i] != '\0')
		{
			words++;
			while (s[i] != '\0' && s[i] != c)
				i++;
		}
	}
	return (words);
}

static void	free_result(char **result, int i)
{
	int	j;

	j = 0;
	while (j < i)
	{
		free(result[j]);
		j++;
	}
	free(result);
}

static char	*allocating(char **res, const char *start, const char *end, int i)
{
	int	len;

	len = end - start;
	res[i] = (char *) malloc((len + 1) * sizeof(char));
	if (!res[i])
	{
		free_result(res, i);
		return (NULL);
	}
	ft_strlcpy(res[i], start, len + 1);
	return (res[i]);
}

static char	**process_string(const char *s, char c, char **result)
{
	int			i;
	const char	*start;

	i = 0;
	while (*s)
	{
		while (*s == c)
			s++;
		start = s;
		while (*s && *s != c)
			s++;
		if (s > start)
		{
			if (!allocating(result, start, s, i))
				return (NULL);
			i++;
		}
	}
	result[i] = NULL;
	return (result);
}

char	**ft_split(const char *s, char c)
{
	int			words;
	char		**result;

	if (!s)
		return (NULL);
	words = ft_count_words(s, c);
	result = (char **) malloc((words + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	return (process_string(s, c, result));
}
