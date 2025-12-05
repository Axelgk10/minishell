/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:38:00 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 19:14:58 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	fd_checker(t_shell *shell)
{
	if (shell->commands->out_fd != STDOUT_FILENO && shell->commands->out_fd > 0)
	{
		dup2(shell->commands->out_fd, STDOUT_FILENO);
		close(shell->commands->out_fd);
	}
	if (shell->commands->in_fd != STDIN_FILENO && shell->commands->in_fd > 0)
	{
		dup2(shell->commands->in_fd, STDIN_FILENO);
		close(shell->commands->in_fd);
	}
}

void	status_wait(pid_t pid, int status)
{
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		g_exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		g_exit_status = 128 + WTERMSIG(status);
	else
		g_exit_status = 1;
}
