/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_pollo.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 19:33:02 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/11 17:06:29 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*append_char_to_result(char *result, char c)
{
	char	*temp;
	char	buffer[2];

	buffer[0] = c;
	buffer[1] = '\0';
	temp = result;
	result = ft_strjoin(result, buffer);
	free(temp);
	return (result);
}

static char	*append_expansion_to_result(char *result, char *expanded)
{
	char	*temp;

	temp = result;
	result = ft_strjoin(result, expanded);
	free(temp);
	free(expanded);
	return (result);
}

static char	*process_double_quote_content(t_shell *shell, char *str, int *i)
{
	char	*result;
	char	*expanded;

	result = ft_strdup("");
	while (str[*i] && str[*i] != '"')
	{
		if (str[*i] == '\'')
		{
			result = append_char_to_result(result, '\'');
			(*i)++;
		}
		else if (str[*i] == '$' && str[*i + 1]
			&& !is_dollar_terminator(str[*i + 1]))
		{
			expanded = expand_dollar(shell, str, i);
			result = append_expansion_to_result(result, expanded);
		}
		else
		{
			result = append_char_to_result(result, str[*i]);
			(*i)++;
		}
	}
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
