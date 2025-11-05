/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 15:53:28 by axgimene          #+#    #+#             */
/*   Updated: 2025/05/06 11:38:01 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(const char *s, char c)
{
	int	count;

	count = 0;
	while (*s)
	{
		while (*s && *s == c)
			s++;
		if (*s && *s != c)
		{
			count++;
			while (*s && *s != c)
				s++;
		}
	}
	return (count);
}

static char	*substr(const char *s, int start, int end)
{
	char	*word;
	int		i;

	word = malloc(end - start + 1);
	i = 0;
	if (!word)
		return (NULL);
	while (start < end)
		word[i++] = s[start++];
	word[i] = '\0';
	return (word);
}

static void	free_all(char **res, int i)
{
	while (--i)
		free (res[i]);
	free (res);
}

static int	fill_split(char **res, const char *s, char c)
{
	int		i;
	int		start;
	int		end;
	int		word_i;

	i = 0;
	word_i = 0;
	while (s[i])
	{
		while (s[i] == c)
			i++;
		start = i;
		while (s[i] && s[i] != c)
			i++;
		end = i;
		if (end > start)
		{
			res[word_i] = substr(s, start, end);
			if (!res[word_i])
				return (free_all(res, word_i), 0);
			word_i++;
		}
	}
	res[word_i] = NULL;
	return (1);
}

char	**ft_split(char const *s, char c)
{
	char	**res;

	res = malloc((count_words(s, c) + 1) * sizeof(char *));
	if (!res)
		return (NULL);
	if (!fill_split(res, s, c))
		return (NULL);
	return (res);
}
