/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:35:15 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 15:42:09 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	is_pwd(t_shell *shell, int i)
{
	char	*new_pwd;
	char	*current_dir;

	current_dir = getcwd(NULL, 0);
	if (current_dir)
	{
		new_pwd = ft_strjoin("PWD=", current_dir);
		free(current_dir);
	}
	else if (shell->logical_pwd)
	{
		new_pwd = ft_strjoin("PWD=", shell->logical_pwd);
	}
	else
	{
		return ;
	}
	if (new_pwd)
	{
		free(shell->env[i]);
		shell->env[i] = new_pwd;
	}
}

static void	is_oldpwd(t_shell *shell, int i, char	*old_pwd)
{
	char	*new_oldpwd;

	new_oldpwd = ft_strjoin("OLDPWD=", old_pwd);
	if (new_oldpwd)
	{
		free(shell->env[i]);
		shell->env[i] = new_oldpwd;
	}
}

void	update_envs(t_shell *shell)
{
	int		i;
	char	*old_pwd;

	old_pwd = NULL;
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], "PWD=", 4) == 0)
		{
			old_pwd = ft_strdup(shell->env[i] + 4);
			is_pwd(shell, i);
			break ;
		}
		i++;
	}
	process_export_arg(shell, "OLDPWD=");
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], "OLDPWD=", 7) == 0 && old_pwd)
			is_oldpwd(shell, i, old_pwd);
		i++;
	}
	if (old_pwd)
		free(old_pwd);
}
