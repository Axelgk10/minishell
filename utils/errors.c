/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:37 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 18:10:11 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	error_executing(int site_of_error, char **env, char **cmd_params)
{
	(void)env;
	(void)cmd_params;
	if (site_of_error == 0)
		exit((perror("command not found"), 127));
	else if (site_of_error == 1)
		exit((perror("bin not found"), 127));
	else
		exit((perror("execve"), 127));
}

int	write_error_message(int fd, char *cmd, char *arg, char *error_msg)
{
	ft_putstr_fd(cmd, fd);
	ft_putstr_fd(": ", fd);
	if (arg && arg[0] != '\0')
	{
		ft_putstr_fd(arg, fd);
		ft_putstr_fd(": ", fd);
	}
	ft_putstr_fd(error_msg, fd);
	ft_putstr_fd("\n", fd);
	return (127);
}

void	null_input(t_shell *shell)
{
	printf("exit\n");
	if (shell)
		cleanup_shell(shell);
	rl_clear_history();
	exit(0);
}
