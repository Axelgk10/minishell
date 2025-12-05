/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_piped_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:38:00 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 17:30:41 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	create_pipe_if_needed(t_cmd *current, int *pipe_fd)
{
	if (current->next)
	{
		if (pipe(pipe_fd) == -1)
		{
			perror("pipe");
			return (1);
		}
	}
	return (0);
}

void	wait_for_childs(t_shell *shell)
{
	t_cmd	*current;
	int		status;

	current = shell->commands;
	while (current)
	{
		wait(&status);
		if (WIFEXITED(status))
			g_exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			g_exit_status = 128 + WTERMSIG(status);
		current = current->next;
	}
}

void	cleanup_path_env(char **path_env)
{
	int	i;

	i = 0;
	while (path_env && path_env[i])
	{
		free(path_env[i]);
		i++;
	}
	free(path_env);
}
