/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:00:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/04 18:00:00 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int g_exit_status = 0;

void	free_shell_after_execution(t_shell *shell)
{
	if (!shell)
		return ;
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

static void	handle_input(t_shell *shell, char *input)
{
	if (!input)
	{
		if (shell->prompt)
		{
			free(shell->prompt);
			shell->prompt = NULL;
		}
		null_input(shell);
	}
	if (input && *input)
		process_input(shell, input);
	else
		free(input);
}

static void	update_prompt(t_shell *shell, char *cwd_buffer)
{
	if (shell->prompt)
	{
		free(shell->prompt);
		shell->prompt = NULL;
	}
	if (getcwd(cwd_buffer, 1024) != NULL)
		shell->prompt = format_cwd(cwd_buffer);
	else
		shell->prompt = ft_strdup("Minishell$ ");
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	*shell;
	char	*input;
	char	cwd_buffer[1024];

	(void)argc;
	(void)argv;
	shell = (t_shell *)malloc(sizeof(t_shell));
	if (!shell)
	{
		ft_putstr_fd("Error: malloc failed for shell\n", STDERR_FILENO);
		return (1);
	}
	init_shell(shell, envp);
	init_signals();
	while (1)
	{
		update_prompt(shell, cwd_buffer);
		input = readline(shell->prompt);
		handle_input(shell, input);
	}
	cleanup_shell(shell);
	free(shell);
	return (0);
}
