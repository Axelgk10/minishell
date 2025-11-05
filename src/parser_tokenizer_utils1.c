/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_tokenizer_utils1.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 12:29:11 by axgimene          #+#    #+#             */
/*   Updated: 2025/10/31 12:46:42 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = value;
	token->next = NULL;
	return (token);
}

void	add_token_back(t_token **head, t_token *new_token)
{
	t_token	*current;

	if (!*head)
	{
		*head = new_token;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

void	process_token(char *input, int *i, t_token **head)
{
	char			*value;
	t_token			*new_token;
	t_token_type	type;

	if (is_metachar(input[*i]) && input[*i] != ' ' && input[*i] != '\t')
	{
		value = extract_metachar(input, i);
		type = get_token_type(value);
	}
	else
	{
		value = extract_word(input, i);
		type = T_WORD;
	}
	new_token = create_token(type, value);
	if (!new_token)
		return ;
	add_token_back(head, new_token);
}

t_token	*tokenize(char *input)
{
	t_token	*head;
	int		i;

	head = NULL;
	i = 0;
	while (input[i] != '\0')
	{
		if (input[i] == ' ' || input[0] == '\t' || input[i] == '\n')
		{
			i++;
			continue ;
		}
		process_token(input, &i, &head);
		if (!head)
			break ;
	}
	return (head);
}
