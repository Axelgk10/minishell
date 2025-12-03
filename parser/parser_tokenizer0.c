/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_tokenizer0.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 12:38:04 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/06 13:06:06 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_metachar(char c)
{
	return (c == '|' || c == '<' || c == '>' \
|| c == ' ' || c == '\t' || c == '\n');
}

t_token_type	get_token_type(char *str)
{
	if (str[0] == '|')
		return (T_PIPE);
	else if (str[0] == '<' && str[1] == '<')
		return (T_HEREDOC);
	else if (str[0] == '>' && str[1] == '>')
		return (T_APPEND);
	else if (str[0] == '<')
		return (T_REDIR_IN);
	else if (str[0] == '>')
		return (T_REDIR_OUT);
	else
		return (T_WORD);
}

char	*extract_metachar(char *input, int *i)
{
	t_token_type	type;
	char			*value;

	type = get_token_type(&input[*i]);
	if (type == T_HEREDOC || type == T_APPEND)
	{
		value = ft_substr(input, *i, 2);
		*i += 2;
	}
	else
	{
		value = ft_substr(input, *i, 1);
		(*i)++;
	}
	return (value);
}

void	handle_quotes_in_token(char *input, int *i, char *quote)
{
	if (!*quote && (input[*i] == '\'' || input[*i] == '"'))
	{
		*quote = input[*i];
		return ;
	}
	else if (*quote && input[*i] == *quote)
	{
		*quote = 0;
		return ;
	}
}
