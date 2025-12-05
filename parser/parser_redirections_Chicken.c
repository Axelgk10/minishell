/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections_Chicken.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 13:50:28 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 14:46:45 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	handle_redirection_mid(t_token_type type,
	char *filename, t_cmd *cmd)
{
	if (type == T_REDIR_IN)
	{
		if (cmd->in_fd >= 3)
			close(cmd->in_fd);
		cmd->in_fd = handle_input_redirection(filename);
	}
	else if (type == T_REDIR_OUT)
	{
		if (cmd->out_fd >= 3)
			close(cmd->out_fd);
		cmd->out_fd = handle_output_redirection(filename);
	}
	else if (type == T_APPEND)
	{
		if (cmd->out_fd >= 3)
			close(cmd->out_fd);
		cmd->out_fd = handle_append_redirection(filename);
	}
	else if (type == T_HEREDOC)
	{
		if (cmd->in_fd >= 3)
			close(cmd->in_fd);
		cmd->in_fd = handle_heredoc(filename);
	}
}

int	handle_redirection(t_token **tokens, t_cmd *cmd)
{
	t_token_type	type;
	char			*filename;
	t_token			*filename_token;

	if (!tokens || !*tokens || !cmd)
		return (0);
	type = (*tokens)->type;
	filename_token = (*tokens)->next;
	if (!filename_token || filename_token->type != T_WORD)
		return (0);
	filename = filename_token->value;
	handle_redirection_mid(type, filename, cmd);
	return (1);
}
