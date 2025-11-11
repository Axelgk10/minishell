/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parser_main4.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 09:26:02 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/11 16:24:49 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	process_word_token(t_token **current_token, t_cmd *current_cmd)
{
	add_arg_to_command(current_cmd, (*current_token)->value);
	*current_token = (*current_token)->next;
	return (1);
}

static int	process_redir_token(t_token **current_token, t_cmd **current_cmd)
{
	if (!is_valid_redir_sequence(*current_token))
	{
		*current_cmd = NULL;
		return (0);
	}
	if (!handle_redirection(current_token, *current_cmd))
		*current_cmd = NULL;
	return (*current_cmd != NULL);
}

static int	process_token_in_parser(t_token **current_token,
	t_cmd **current_cmd)
{
	if (*current_cmd == NULL)
		return (0);
	if ((*current_token)->type == T_WORD)
		return (process_word_token(current_token, *current_cmd));
	else if ((*current_token)->type == T_PIPE)
		return (handle_pipe_token(current_token, current_cmd));
	else if (is_redirection_token((*current_token)->type))
		return (process_redir_token(current_token, current_cmd));
	else
	{
		*current_token = (*current_token)->next;
		return (1);
	}
}

static int	process_all_tokens(t_token **current, t_cmd **current_cmd,
	t_cmd **head)
{
	while (*current)
	{
		if (!process_token_in_parser(current, current_cmd))
		{
			free_commands(head);
			return (0);
		}
	}
	return (1);
}

t_cmd	*parse_tokens(t_token **tokens)
{
	t_cmd	*head;
	t_cmd	*current_cmd;
	t_token	*current;

	if (!tokens || !*tokens)
		return (NULL);
	current = *tokens;
	head = NULL;
	init_first_command(&head, &current_cmd);
	if (!current_cmd)
		return (NULL);
	if (!process_all_tokens(&current, &current_cmd, &head))
		return (NULL);
	if (!validate_final_command(current_cmd))
	{
		free_commands(&head);
		return (NULL);
	}
	set_builtin_flag(current_cmd);
	return (head);
}
