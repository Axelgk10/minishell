/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_handlers.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 15:05:00 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 15:06:10 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	handle_home_cd(t_shell *shell)
{
	char	*home_path;

	home_path = get_env_value_from_shell(shell, "HOME");
	if (!home_path)
		return (write(2, "cd: HOME not set\n", 17), 1);
	if (chdir(home_path) != 0)
		return (perror("cd"), 1);
	return (0);
}

int	handle_oldpwd_cd(t_shell *shell)
{
	char	*oldpwd;

	oldpwd = get_env_value_from_shell(shell, "OLDPWD");
	if (!oldpwd)
		return (write(2, "cd: OLDPWD not set\n", 19), 1);
	write(1, oldpwd, ft_strlen(oldpwd));
	write(1, "\n", 1);
	if (chdir(oldpwd) != 0)
		return (perror("cd"), 1);
	return (0);
}

int	handle_tilde_cd(t_shell *shell, char *path)
{
	char	*expanded_path;

	expanded_path = ft_strjoin(get_env_value_from_shell\
(shell, "HOME"), path + 1);
	if (!expanded_path)
		return (1);
	if (chdir(expanded_path) != 0)
		return (perror("cd"), free(expanded_path), 1);
	free(expanded_path);
	return (0);
}

int	handle_getcwd_error(t_shell *shell, char *path)
{
	char	*new_logical_pwd;
	int		result;

	ft_putstr_fd("cd: error retrieving current \
directory: getcwd: ", STDERR_FILENO);
	ft_putstr_fd("cannot access parent directories: \
No such file or directory\n", STDERR_FILENO);
	new_logical_pwd = resolve_logical_path(shell->logical_pwd, path);
	if (new_logical_pwd)
	{
		result = chdir(new_logical_pwd);
		if (result == 0)
		{
			free(shell->logical_pwd);
			shell->logical_pwd = new_logical_pwd;
		}
		else
		{
			free(shell->logical_pwd);
			shell->logical_pwd = new_logical_pwd;
		}
	}
	return (0);
}

int	handle_relative_cd(t_shell *shell, char *path)
{
	char	*current_dir;
	int		result;

	current_dir = getcwd(NULL, 0);
	if (!current_dir && shell->logical_pwd)
		return (handle_getcwd_error(shell, path));
	if (current_dir)
		free(current_dir);
	result = chdir(path);
	if (result != 0)
		return (perror("cd"), 1);
	return (0);
}
