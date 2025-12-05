/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:43 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 14:03:57 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	init_shell(t_shell *shell, char **envp)
{
	int		env_count;
	int		i;

	if (!shell)
		return ;
	
	shell->env = NULL;
	shell->env_count = 0;
	shell->local_vars = NULL;
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->prompt = NULL;
	shell->logical_pwd = getcwd(NULL, 0); // Initialize logical working directory
	shell->exit_status = 0;
	shell->stdin_copy = STDIN_FILENO;
	shell->stdout_copy = STDOUT_FILENO;
	env_count = 0;
	while (envp && envp[env_count])
		env_count++;
	shell->env = malloc((env_count + 101) * sizeof(char *));
	if (!shell->env)
	{
		ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
		exit(1);
	}
	shell->env_count = env_count;
	i = 0;
	while (i < env_count)
	{
		shell->env[i] = ft_strdup(envp[i]);
		if (!shell->env[i])
		{
			int j = 0;
			while (j < i)
			{
				free(shell->env[j]);
				j++;
			}
			free(shell->env);
			shell->env = NULL;
			shell->env_count = 0;
			ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
			exit(1);
		}
		i++;
	}
	shell->env[env_count] = NULL;
}

void	cleanup_shell(t_shell *shell)
{
	int i;
	
	if (!shell)
		return ;
	
	if (shell->env)
	{
		i = 0;
		while (shell->env[i])
		{
			free(shell->env[i]);
			shell->env[i] = NULL;
			i++;
		}
		free(shell->env);
		shell->env = NULL;
	}
	if (shell->prompt)
	{
		free(shell->prompt);
		shell->prompt = NULL;
	}
	if (shell->logical_pwd)
	{
		free(shell->logical_pwd);
		shell->logical_pwd = NULL;
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
	if (shell->local_vars)
	{
		i = 0;
		while (shell->local_vars[i])
		{
			free(shell->local_vars[i]);
			shell->local_vars[i] = NULL;
			i++;
		}
		free(shell->local_vars);
		shell->local_vars = NULL;
	}
}

/*void	check_struct(t_shell *shell)
{
	int i;
	t_cmd *current;

	i = 0;
	printf("Stdin: %d\n", shell->stdin_copy);
	printf("Stdout: %d\n", shell->stdout_copy);
	printf("Exit Status: %d\n", shell->exit_status);
	printf("/////////////////////Commands///////////////////////////\n");
	current = shell->commands;  // Usar una variable temporal
	while(current)
	{
		printf("FDdin: %d\n", current->in_fd);
		printf("FDout: %d\n", current->out_fd);
		printf("Pipe 0: %d\n", current->pipe[0]);
		printf("Pipe 1: %d\n", current->pipe[1]);
		printf("Is Built in: %d\n", current->is_builtin);
		i = 0;
		if (current->av)  // Verificar que av no sea NULL
		{
			while(current->av[i])
			{
				printf("As %d: %s\n", i, current->av[i]);
				i++;
			}
		}
		printf("////////////////////////////////////////////////\n");
		current = current->next;  // Mover solo la variable temporal
	}
}*/