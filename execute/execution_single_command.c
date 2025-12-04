/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_single_command.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:38:00 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/04 18:00:00 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	got_path(t_shell *shell)
{
	int	i;

	if (!shell || !shell->env)
		return (0);
	
	i = 0;
	while (i < shell->env_count && shell->env[i])
	{
		if (ft_strncmp(shell->env[i], "PATH=", 5) == 0)
			return (1);
		i++;
	}
	return (0);
}

void	just_execute_it_man(t_shell *shell)
{
	char	**path_env;
	char	*bin_path;
	int		status;
	pid_t	pid;

	status = 0;
	path_env = get_path_values(shell->env, "PATH");
	pid = fork();
	if (pid == -1)
	{
		int i = 0;
		while(path_env && path_env[i])
			free(path_env[i++]);
		free(path_env);
		_exit(1);
	}
	else if (pid == 0)
	{
		fd_checker(shell);
		bin_path = find_binary(shell->commands->av[0], path_env);
		if (!bin_path)
		{
			// ✅ Valida si PATH existe
			if (!got_path(shell))
			{
				write_error_message(STDERR_FILENO, shell->commands->av[0], "", "No such file or directory");
			}
			else
			{
				write_error_message(STDERR_FILENO, shell->commands->av[0], "", "command not found");
			}
			// ✅ Libera path_env en hijo
			int i = 0;
			while(path_env && path_env[i])
				free(path_env[i++]);
			free(path_env);
			_exit(127);
		}
		if (execve(bin_path, shell->commands->av, shell->env) == -1)
		{
			perror("execve");
			free(bin_path);
			int i = 0;
			while(path_env && path_env[i])
				free(path_env[i++]);
			free(path_env);
			_exit(126);
		}
	}
	else if (pid > 0)
	{
		int i = 0;
		while(path_env && path_env[i])
			free(path_env[i++]);
		free(path_env);
		status_wait(pid, status);
	}
}

void	execute_builtin(t_shell *shell)
{
    if (!shell || !shell->commands || !shell->commands->av || !shell->commands->av[0])
        return;
    if (!ft_strcmp(shell->commands->av[0], "cd"))
        shell->exit_status = change_directory(shell, shell->commands->av[1]);
    else if (!ft_strcmp(shell->commands->av[0], "pwd"))
        shell->exit_status = ft_pwd(shell->commands);
    else if (!ft_strcmp(shell->commands->av[0], "exit"))
        manage_exit(shell);
    else if (!ft_strcmp(shell->commands->av[0], "env"))  // ✅ SIN got_path
        shell->exit_status = ft_env(shell);
    else if (!ft_strcmp(shell->commands->av[0], "echo"))
        shell->exit_status = ft_echo(shell->commands);
    else if (!ft_strcmp(shell->commands->av[0], "export"))
        shell->exit_status = export_variables(shell);
    else if (!ft_strcmp(shell->commands->av[0], "unset"))
        shell->exit_status = unset_variables(shell);
    else if (ft_strchr(shell->commands->av[0], '='))
        shell->exit_status = set_local_var(shell);
    else
        just_execute_it_man(shell);
}

void	execute_command(t_shell *shell)
{
	if (!shell || !shell->commands || !shell->commands->av || !shell->commands->av[0])
		return;
	if (shell->commands->is_builtin)
		execute_builtin(shell);
	else
		just_execute_it_man(shell);
}
