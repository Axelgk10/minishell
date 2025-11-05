/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parser_init5.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:01:22 by axgimene          #+#    #+#             */
/*   Updated: 2025/10/31 19:47:55 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	init_first_command(t_cmd **head, t_cmd **current_cmd)
{
	*current_cmd = create_command();
	if (!*current_cmd)
		return ;
	*head = *current_cmd;
}

void	handle_word_token(t_token **token, t_cmd *cmd)
{
	add_arg_to_command(cmd, (*token)->value);
	*token = (*token)->next;
}

void	handle_pipe_in_parse(t_token **token, t_cmd **cmd)
{
	set_builtin_flag(*cmd);
	(*cmd)->next = create_command();
	(*cmd) = (*cmd)->next;
	*token = (*token)->next;
}

void	handle_redir_in_parse(t_token **token, t_cmd **cmd)
{
	if (!handle_redirection(token, *cmd))
	{
		free_commands(cmd);
		*cmd = NULL;
	}
}
