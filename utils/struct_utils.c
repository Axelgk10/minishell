/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:43 by gguardam          #+#    #+#             */
/*   Updated: 2025/11/28 13:40:44 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	setup_envs(t_shell *shell, char **envp, int env_count)
{
	int	i;
	int	j;

	i = 0;
	while (i < env_count)
	{
		shell->env[i] = ft_strdup(envp[i]);
		if (!shell->env[i])
		{
			j = 0;
			while (j < i)
			{
				free(shell->env[j]);
				j++;
			}
			free(shell->env);
			shell->env = NULL;
			ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
			exit(1);
		}
		i++;
	}
	shell->env[env_count] = NULL;
}

void	init_shell(t_shell *shell, char **envp)
{
	int	env_count;

	if (!shell)
		return ;
	shell->env = NULL;
	shell->local_vars = NULL;
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->prompt = NULL;
	shell->exit_status = 0;
	shell->stdin_copy = STDIN_FILENO;
	shell->stdout_copy = STDOUT_FILENO;
	env_count = 0;
	while (envp && envp[env_count])
		env_count++;
	shell->env = malloc(env_count * sizeof(char *));
	if (!shell->env)
	{
		ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
		exit(1);
	}
	setup_envs(shell, envp, env_count);
}

void	cleanup_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->prompt)
	{
		free(shell->prompt);
		shell->prompt = NULL;
	}
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
	free_envs(shell);
}

void	check_struct(t_shell *shell)
{
	int i;
	t_cmd *current;

	i = 0;
	printf("Stdin: %d\n", shell->stdin_copy);
	printf("Stdout: %d\n", shell->stdout_copy);
	printf("Exit Status: %d\n", shell->exit_status);
	printf("/////////////////////Commands///////////////////////////\n");
	current = shell->commands;
	while(current)
	{
		printf("FDdin: %d\n", current->in_fd);
		printf("FDout: %d\n", current->out_fd);
		printf("Pipe 0: %d\n", current->pipe[0]);
		printf("Pipe 1: %d\n", current->pipe[1]);
		printf("Is Built in: %d\n", current->is_builtin);
		i = 0;
		if (current->av)
		{
			while(current->av[i])
			{
				printf("As %d: %s\n", i, current->av[i]);
				i++;
			}
		}
		printf("////////////////////////////////////////////////\n");
		current = current->next;
	}
}