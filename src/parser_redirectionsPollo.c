/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirectionsPollo.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 13:50:28 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/05 13:57:09 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	handle_redirection(t_token **tokens, t_cmd *cmd)
{
	t_token_type	type;
	char			*filename;

	if (!tokens || !*tokens || !cmd)
		return (0);
	type = (*tokens)->type;
	*tokens = (*tokens)->next;
	if (!*tokens || (*tokens)->type != T_WORD)
		return (0);
	filename = (*tokens)->value;
	if (type == T_REDIR_IN)
	{
		if (cmd->in_fd >= 0)
			close(cmd->in_fd);
		cmd->in_fd = handle_input_redirection(filename);
	}
	else if (type == T_REDIR_OUT)
	{
		if (cmd->out_fd >= 0)
			close(cmd->out_fd);
		cmd->out_fd = handle_output_redirection(filename);
	}
	else if (type == T_APPEND)
	{
		if (cmd->out_fd >= 0)
			close(cmd->out_fd);
		cmd->out_fd = handle_append_redirection(filename);
	}
	else if (type == T_HEREDOC)
	{
		if (cmd->in_fd >= 0)
			close(cmd->in_fd);
		cmd->in_fd = handle_heredoc(filename);
	}
	*tokens = (*tokens)->next;
	return (1);
}