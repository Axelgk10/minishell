/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_chicken.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 19:33:02 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/26 13:10:12 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*append_char_to_result(char *result, char c)
{
	char	*temp;
	char	buffer[2];
	char	*new_result;

	buffer[0] = c;
	buffer[1] = '\0';
	temp = result;
	new_result = ft_strjoin(result, buffer);
	if (!new_result)
	{
		free(temp);
		return (NULL);
	}
	free(temp);
	return (new_result);
}

static char	*process_double_quote_content(t_shell *shell, char *str, int *i)
{
	char	*result;
	char	*expanded;
	char	*temp;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (str[*i] && str[*i] != '"')
	{
		if (str[*i] == '\'')
		{
			temp = append_char_to_result(result, '\'');
			if (!temp)
			{
				free(result);
				return (NULL);
			}
			result = temp;
			(*i)++;
		}
		else if (str[*i] == '$' && str[*i + 1]
			&& !is_dollar_terminator(str[*i + 1]))
		{
			expanded = expand_dollar(shell, str, i);
			if(!expanded)
			{
				free(result);
				return (NULL);
			}
			temp = ft_strjoin(result, expanded);
			free(result);
			free(expanded);
			if (!temp)
				return (NULL);
			result = temp;
		}
		else
		{
			temp = append_char_to_result(result, str[*i]);
			if (!temp)
			{
				free(result);
				return (NULL);
			}
			result = temp;
			(*i)++;
		}
	}
	if(str[*i] == '"')
		(*i)++;
	return (result);
}

char	*handle_double_quotes(t_shell *shell, char *str, int *i)
{
	char	*result;

	++(*i);
	result = process_double_quote_content(shell, str, i);
	if (str[*i] == '"')
		(*i)++;
	return (result);
}
