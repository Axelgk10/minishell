/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_chicken.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 19:33:02 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 13:26:18 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*process_double_quote_content(t_shell *shell, char *str, int *i)
{
	char	*result;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (str[*i] && str[*i] != '"')
	{
		if (str[*i] == '\'')
			result = handle_single_quote_in_double(result, i);
		else if (str[*i] == '$' && str[*i + 1]
			&& !is_dollar_terminator(str[*i + 1]))
			result = handle_dollar_in_double(shell, result, str, i);
		else
			result = handle_regular_in_double(result, str, i);
		if (!result)
			return (NULL);
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
