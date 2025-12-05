/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expander_utils7.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 12:00:50 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 14:43:20 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*handle_regular_char(char *str, int *i)
{
	char	*part;

	part = ft_substr(str, *i, 1);
	(*i)++;
	if (!part)
		return (NULL);
	return (part);
}

static char	*process_char_in_expansion(t_shell *shell, char *str, int *i)
{
	if (str[*i] == '\'')
		return (handle_single_quotes(str, i));
	else if (str[*i] == '"')
		return (handle_double_quotes(shell, str, i));
	else if (str[*i] == '$' && str[*i + 1]
		&& !is_dollar_terminator(str[*i + 1]))
		return (expand_dollar(shell, str, i));
	else
		return (handle_regular_char(str, i));
}

static int	requires_expansion(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] && !is_dollar_terminator(str[i + 1]))
			return (1);
		if (str[i] == '\'' || str[i] == '"')
			return (1);
		i++;
	}
	return (0);
}

void	expand_variables(t_shell *shell, t_token *tokens)
{
	t_token	*current;
	char	*expanded;

	current = tokens;
	while (current)
	{
		if (current->type == T_WORD && current->value)
		{
			if (requires_expansion(current->value))
			{
				expanded = expand_string(shell, current->value);
				if (expanded)
				{
					free(current->value);
					current->value = expanded;
				}
			}
		}
		current = current->next;
	}
}

char	*expand_string(t_shell *shell, char *str)
{
	int		i;
	char	*result;
	char	*part;

	if (!str || !str[0])
		return (ft_strdup(""));
	i = 0;
	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (str[i])
	{
		part = process_char_in_expansion(shell, str, &i);
		if (!part)
			continue ;
		result = join_and_free(result, part);
		if (!result)
			return (NULL);
	}
	return (result);
}
