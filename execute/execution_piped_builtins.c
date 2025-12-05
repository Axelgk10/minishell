/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_piped_builtins.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:00:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/04 18:00:00 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	execute_builtin_cd_pwd_exit(t_shell *shell, t_cmd *cmd)
{
	if (!ft_strcmp(cmd->av[0], "cd"))
		return (change_directory(shell, cmd->av[1]));
	if (!ft_strcmp(cmd->av[0], "pwd"))
		return (ft_pwd(shell, cmd));
	if (!ft_strcmp(cmd->av[0], "exit"))
	{
		manage_exit(shell);
		return (0);
	}
	return (-1);
}

int	execute_builtin_env_echo_export(t_shell *shell, t_cmd *cmd)
{
	if (!ft_strcmp(cmd->av[0], "env"))
		return (ft_env(shell));
	if (!ft_strcmp(cmd->av[0], "echo"))
		return (ft_echo(cmd));
	if (!ft_strcmp(cmd->av[0], "export"))
		return (export_variables(shell));
	if (!ft_strcmp(cmd->av[0], "unset"))
		return (unset_variables(shell));
	if (ft_strchr(cmd->av[0], '='))
		return (set_local_var(shell));
	return (-1);
}

int	execute_builtin_in_pipeline(t_shell *shell, t_cmd *cmd)
{
	int	result;

	if (!shell || !cmd || !cmd->av || !cmd->av[0])
		return (1);
	result = execute_builtin_cd_pwd_exit(shell, cmd);
	if (result != -1)
		return (result);
	result = execute_builtin_env_echo_export(shell, cmd);
	if (result != -1)
		return (result);
	return (write_error_message(STDERR_FILENO, cmd->av[0],
			"", "command not found"));
}
