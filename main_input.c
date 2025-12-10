/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_input.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:00:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/04 18:00:00 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_empty_command(t_cmd *cmd)
{
	if (!cmd || !cmd->av || !cmd->av[0] || cmd->av[0][0] == '\0')
		return (1);
	return (0);
}

static void	evaluate_struct(t_shell *shell)
{
	if (!shell || !shell->commands)
		return ;
	if (is_empty_command(shell->commands))
		return ;
	if (!shell->commands->next)
	{
		if (shell->commands->is_builtin)
			execute_builtin(shell);
		else
		{
			just_execute_it_man(shell);
			shell->exit_status = g_exit_status;
		}
	}
	else if (shell->commands->next)
		{
			execute_pipeline(shell, shell->commands);
			shell->exit_status = g_exit_status;
		}
}

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

static void	process_tokens(t_shell *shell)
{
	if (shell->tokens)
	{
		expand_variables(shell, shell->tokens);
		shell->commands = parse_tokens(shell->tokens);
		if (shell->commands)
			evaluate_struct(shell);
		else
			shell->exit_status = 2;
	}
}

void	process_input(t_shell *shell, char *input)
{
	if (check_unclosed_quotes(input))
	{
		free(input);
		return ;
	}
	add_history(input);
	cleanup_shell_state(shell);
	shell->tokens = tokenize(input);
	if (check_pipe_at_start(shell, input))
		return ;
	process_tokens(shell);
	cleanup_shell_state(shell);
	free(input);
}
