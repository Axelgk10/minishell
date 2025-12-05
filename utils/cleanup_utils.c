/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 18:10:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 18:04:30 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	cleanup_env(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		env[i] = NULL;
		i++;
	}
	free(env);
}

static void	cleanup_simple_pointers(t_shell *shell)
{
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
}

static void	cleanup_commands_and_tokens(t_shell *shell)
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

void	cleanup_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->env)
	{
		cleanup_env(shell->env);
		shell->env = NULL;
	}
	cleanup_simple_pointers(shell);
	cleanup_commands_and_tokens(shell);
	if (shell->local_vars)
	{
		cleanup_env(shell->local_vars);
		shell->local_vars = NULL;
	}
}
