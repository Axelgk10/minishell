/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_validation.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 00:00:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/10 12:13:28 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	cleanup_shell_state(t_shell *shell)
{
	if (shell->tokens)
	{
		free_tokens(&shell->tokens);
		shell->tokens = NULL;
	}
	if (shell->commands)
	{
		free_commands(&shell->commands);
		shell->commands = NULL;
	}
}

int	check_pipe_at_start(t_shell *shell, char *input)
{
	if (shell->tokens && shell->tokens->type == T_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near ", 2);
		ft_putstr_fd("unexpected token `|'\n", 2);
		shell->exit_status = 2;
		cleanup_shell_state(shell);
		free(input);
		return (1);
	}
	return (0);
}