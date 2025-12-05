/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:43 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 18:06:42 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	init_shell_vars(t_shell *shell)
{
	shell->env = NULL;
	shell->env_count = 0;
	shell->local_vars = NULL;
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->prompt = NULL;
	shell->logical_pwd = getcwd(NULL, 0);
	shell->exit_status = 0;
	shell->stdin_copy = STDIN_FILENO;
	shell->stdout_copy = STDOUT_FILENO;
}

static void	cleanup_env_on_error(char **env, int count)
{
	int	j;

	j = 0;
	while (j < count)
	{
		free(env[j]);
		j++;
	}
	free(env);
}

static int	alloc_env_array(t_shell *shell, int env_count)
{
	shell->env = malloc((env_count + 101) * sizeof(char *));
	if (!shell->env)
	{
		ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
		exit(1);
	}
	shell->env_count = env_count;
	return (1);
}

static int	copy_env_vars(t_shell *shell, char **envp, int env_count)
{
	int	i;

	i = 0;
	while (i < env_count)
	{
		shell->env[i] = ft_strdup(envp[i]);
		if (!shell->env[i])
		{
			cleanup_env_on_error(shell->env, i);
			shell->env = NULL;
			shell->env_count = 0;
			ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
			exit(1);
		}
		i++;
	}
	shell->env[env_count] = NULL;
	return (1);
}

void	init_shell(t_shell *shell, char **envp)
{
	int	env_count;

	if (!shell)
		return ;
	init_shell_vars(shell);
	env_count = 0;
	while (envp && envp[env_count])
		env_count++;
	alloc_env_array(shell, env_count);
	copy_env_vars(shell, envp, env_count);
}
