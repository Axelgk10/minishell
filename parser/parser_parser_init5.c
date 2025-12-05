/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parser_init5.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:01:22 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 13:25:15 by axgimene         ###   ########.fr       */
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
	t_cmd	*new_cmd;

	set_builtin_flag(*cmd);
	if (pipe((*cmd)->pipe) == -1)
	{
		perror("pipe");
		free_commands(cmd);
		*cmd = NULL;
		return ;
	}
	new_cmd = create_command();
	if (!new_cmd)
	{
		close((*cmd)->pipe[0]);
		close((*cmd)->pipe[1]);
		free_commands(cmd);
		*cmd = NULL;
		return ;
	}
	if ((*cmd)->out_fd == -1)
		(*cmd)->out_fd = (*cmd)->pipe[1];
	new_cmd->in_fd = (*cmd)->pipe[0];
	(*cmd)->next = new_cmd;
	*cmd = new_cmd;
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
