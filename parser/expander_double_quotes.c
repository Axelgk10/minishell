/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_double_quotes.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 13:30:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 13:26:13 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*append_char_to_result(char *result, char c)
{
	char	*new_result;
	char	buffer[2];

	buffer[0] = c;
	buffer[1] = '\0';
	new_result = ft_strjoin(result, buffer);
	if (!new_result)
		return (NULL);
	free(result);
	return (new_result);
}

static char	*append_expansion_to_result(char *result, char *expanded)
{
	char	*temp;
	char	*new_result;

	if (!expanded)
		return (result);
	temp = result;
	new_result = ft_strjoin(result, expanded);
	free(temp);
	free(expanded);
	if (!new_result)
		return (NULL);
	return (new_result);
}

char	*handle_single_quote_in_double(char *result, int *i)
{
	char	*temp;

	temp = append_char_to_result(result, '\'');
	if (!temp)
	{
		free(result);
		return (NULL);
	}
	(*i)++;
	return (temp);
}

char	*handle_dollar_in_double(t_shell *shell, char *result,
		char *str, int *i)
{
	char	*expanded;
	char	*temp;

	expanded = expand_dollar(shell, str, i);
	temp = append_expansion_to_result(result, expanded);
	if (!temp)
	{
		free(result);
		return (NULL);
	}
	return (temp);
}

char	*handle_regular_in_double(char *result, char *str, int *i)
{
	char	*temp;

	temp = append_char_to_result(result, str[*i]);
	if (!temp)
	{
		free(result);
		return (NULL);
	}
	(*i)++;
	return (temp);
}
