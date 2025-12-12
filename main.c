/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:00:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 19:45:27 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_exit_status = 0;

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
	char	*current_dir;

	if (shell->prompt)
	{
		free(shell->prompt);
		shell->prompt = NULL;
	}
	current_dir = getcwd(cwd_buffer, 1024);
	if (current_dir != NULL)
		shell->prompt = format_cwd(cwd_buffer);
	else if (shell->logical_pwd)
		shell->prompt = format_cwd(shell->logical_pwd);
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
		return (ft_putstr_fd(\
"Error: malloc failed for shell\n", STDERR_FILENO), 1);
	init_shell(shell, envp);
	init_signals();
	while (1)
	{
		update_prompt(shell, cwd_buffer);
		input = readline(shell->prompt);
		if (!input)
			handle_input(shell, input);
		set_signals_for_cmd();
		if (input && *input)
			handle_input(shell, input);
		restore_signals();
	}
	cleanup_shell(shell);
	free(shell);
	return (0);
}
