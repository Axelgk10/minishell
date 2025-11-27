/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_single_command.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 11:20:34 by gguardam          #+#    #+#             */
/*   Updated: 2025/11/27 16:06:45 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	got_path(t_shell *shell)
{
	int	i;

	i = 0;
	while(shell->env[i])
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
	pid = fork();
	if (pid == -1)
		exit (1);
	else if (pid == 0)
	{
		fd_checker(shell);
		path_env = get_path_values(shell->env, "PATH");
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
			// ✅ Libera path_env
			int i = 0;
			while(path_env && path_env[i])
				free(path_env[i++]);
			free(path_env);
			exit(127);
		}
		if (execve(bin_path, shell->commands->av, shell->env) == -1)
		{
			free(bin_path);
			int i = 0;
			while(path_env && path_env[i])
				free(path_env[i++]);
			free(path_env);
			error_executing(2, shell->env, shell->commands->av);
		}
	}
	else if (pid > 0)
		status_wait(pid, status);
}

void	execute_builtin(t_shell *shell)
{
    if (!shell || !shell->commands || !shell->commands->av || !shell->commands->av[0])
        return;
    if (!ft_strcmp(shell->commands->av[0], "cd"))
    {
        shell->exit_status = change_directory(shell->commands->av[1]);
        if (shell->exit_status == 0)
            update_envs(shell);
    }
    else if (!ft_strcmp(shell->commands->av[0], "pwd"))
        shell->exit_status = ft_pwd(shell->commands);
    else if (!ft_strcmp(shell->commands->av[0], "exit"))
        manage_exit(shell);
    else if (!ft_strcmp(shell->commands->av[0], "env"))
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
