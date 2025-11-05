/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parser_main4.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 09:26:02 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/05 13:15:04 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_redirection_token(t_token_type type)
{
	return (type >= T_REDIR_IN && type <= T_HEREDOC);
}

static void	handle_redirection_token(t_token **current_token, t_cmd **current_cmd)
{
	if(!handle_redirection(current_token, *current_cmd))
	{
		free_commands(current_cmd);
		*current_cmd = NULL;
	}
}

static void	handle_pipe_token(t_token **current_token, t_cmd **current_cmd)
{
	set_builtin_flag(*current_cmd);
	(*current_cmd)->next = create_command();
	*current_cmd = (*current_cmd)->next;
	*current_token = (*current_token)->next;
}

static void	process_token_in_parser(t_token **current_token, t_cmd **current_cmd)
{
	if ((*current_token)->type == T_WORD)
	{
		add_arg_to_command(*current_cmd, (*current_token)->value);
		*current_token = (*current_token)->next;
	}
	else if((*current_token)->type == T_PIPE)
		handle_pipe_token(current_token, current_cmd);
	else if (is_redirection_token((*current_token)->type))
		handle_redirection_token(current_token, current_cmd);
	else
		*current_token = (*current_token)->next;
}

t_cmd	*parse_tokens(t_token **tokens)
{
	t_cmd	*head;
	t_cmd	*current_cmd;
	t_token	*current_token;

	head = NULL;
	current_cmd = NULL;
	current_token = *tokens;
	while (current_token && current_cmd != (t_cmd *)-1)
	{
		if (!current_cmd)
			init_first_command(&head, &current_cmd);
		process_token_in_parser(&current_token, &current_cmd);
	}
	if (current_cmd && current_cmd != (t_cmd *) -1)
		set_builtin_flag(current_cmd);
	return (head);
}