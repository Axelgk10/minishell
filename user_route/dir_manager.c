/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dir_manager.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:35:40 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 14:57:21 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*get_home_shortcut(char *cwd)
{
	char	*home;
	char	*result;
	size_t	home_len;
	size_t	cwd_len;

	if (!cwd)
		return (ft_strdup("Minishell$ "));
	home = getenv("HOME");
	if (!home)
		return (ft_strdup(cwd));
	home_len = ft_strlen(home);
	cwd_len = ft_strlen(cwd);
	if (ft_strncmp(cwd, home, home_len) == 0)
	{
		result = create_tilde_result(cwd, home, home_len, cwd_len);
		if (result)
			return (result);
	}
	return (ft_strdup(cwd));
}

void	update_pwd_after_cd(t_shell *shell)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (new_pwd)
	{
		if (shell->logical_pwd)
			free(shell->logical_pwd);
		shell->logical_pwd = new_pwd;
	}
	update_envs(shell);
}

int	change_directory(t_shell *shell, char *path)
{
	if (!path)
	{
		if (handle_home_cd(shell) != 0)
			return (1);
	}
	else if (ft_strncmp(path, "-", 1) == 0 && path[1] == '\0')
	{
		if (handle_oldpwd_cd(shell) != 0)
			return (1);
	}
	else if (path[0] == '~' && (path[1] == '\0' || path[1] == '/'))
	{
		if (handle_tilde_cd(shell, path) != 0)
			return (1);
	}
	else
	{
		if (handle_relative_cd(shell, path) != 0)
			return (1);
	}
	update_pwd_after_cd(shell);
	return (0);
}

char	*format_cwd(char *cwd)
{
	char	*processed_cwd;
	char	*result;
	size_t	len;

	if (!cwd)
		return (ft_strdup("Minishell$ "));
	processed_cwd = get_home_shortcut(cwd);
	if (!processed_cwd)
		return (ft_strdup("Minishell$ "));
	len = ft_strlen(processed_cwd) + 3;
	result = malloc(len);
	if (!result)
	{
		free(processed_cwd);
		return (ft_strdup("Minishell$ "));
	}
	ft_strlcpy(result, processed_cwd, len);
	ft_strlcat(result, "$ ", len);
	free(processed_cwd);
	return (result);
}
