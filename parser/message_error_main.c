/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message_error_main.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 13:18:53 by axgimene          #+#    #+#             */
/*   Updated: 2025/11/24 19:11:48 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_redirection_token(t_token_type type)
{
	return (type >= T_REDIR_IN && type <= T_HEREDOC);
}

int	is_valid_redir_sequence(t_token *token)
{
	if (!token)
		return (1);
	if (is_redirection_token(token->type))
	{
		if (!token->next || token->next->type != T_WORD)
		{
			ft_putstr_fd("minishell: syntax error near unexpected token"
				" `newline'\n", 2);
			return (0);
		}
	}
	return (1);
}

int	setup_pipe_fds(t_cmd *current_cmd, t_cmd *new_cmd)
{
	if (pipe(current_cmd->pipe) == -1)
	{
		perror("pipe");
		return (0);
	}
	if (current_cmd->out_fd == -1)
		current_cmd->out_fd = current_cmd->pipe[1];
	new_cmd->in_fd = current_cmd->pipe[0];
	return (1);
}

int	handle_pipe_token(t_token **current_token, t_cmd **current_cmd)
{
    t_cmd	*new_cmd;

    if (!(*current_cmd)->av || !(*current_cmd)->av[0])
    {
        ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
        return (0);
    }
    set_builtin_flag(*current_cmd);
    *current_token = (*current_token)->next;
    new_cmd = create_command();
    if (!new_cmd)
    {
        // Si la creación del nuevo comando falla, no podemos continuar.
        // Retornamos error. El llamador (parse_tokens) limpiará la lista 'head'.
        return (0);
    }
    if (!setup_pipe_fds(*current_cmd, new_cmd))
    {
        free(new_cmd); // ✅ Libera el comando recién creado si el pipe falla.
        return (0);
    }
    (*current_cmd)->next = new_cmd;
    *current_cmd = new_cmd;
    return (1);
}

int	validate_final_command(t_cmd *current_cmd)
{
	if (!current_cmd || !current_cmd->av || !current_cmd->av[0])
	{
		ft_putstr_fd("minishell: syntax error near unexpected token"
			" `newline'\n", 2);
		return (0);
	}
	return (1);
}
