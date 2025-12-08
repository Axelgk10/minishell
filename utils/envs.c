/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:33 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 18:17:20 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	process_export_arg(t_shell *shell, char *arg)
{
	char	*equals_pos;
	char	*var_name;
	char	*new_var;

	equals_pos = ft_strchr(arg, '=');
	var_name = NULL;
	new_var = NULL;
	if (equals_pos)
	{
		var_name = extract_var_name(arg, equals_pos);
		new_var = process_plus_append(var_name, arg);
	}
	else
		var_name = arg;
	if (is_valid_var_name(var_name) || is_valid_var_name(arg))
	{
		add_var_to_env(shell, arg, var_name, new_var);
		if (var_name && var_name != arg)
			free(var_name);
		return (0);
	}
	cleanup_export_vars(var_name, new_var, NULL, arg);
	return (write_error_message(shell->commands->out_fd, "export", arg, \
"not a valid identifier"));
}

static int	append_arg(t_shell *shell, char *arg)
{
	int		i;
	char	*mod_env_name;
	char	*equal_pos;
	char	*local_var;

	equal_pos = ft_strchr(arg, '=');
	mod_env_name = extract_var_name(arg, equal_pos);
	if (!mod_env_name)
		return (1);
	mod_env_name[ft_strlen(mod_env_name) - 1] = '\0';
	local_var = move_local_var_to_env(shell, mod_env_name);
	if (local_var && (add_or_modify_var(shell->env, local_var), 1))
		free(local_var);
	i = find_env_index(shell->env, mod_env_name);
	if (i == -1 && (free(mod_env_name), 1))
		return (1);
	return (do_append(shell, arg, mod_env_name, i));
}

static int	process_export_loop(t_shell *shell, int i)
{
	if (!add_or_append(shell, i))
	{
		if (process_export_arg(shell, shell->commands->av[i]))
			return (1);
	}
	else
	{
		if (append_arg(shell, shell->commands->av[i]))
			return (1);
	}
	return (0);
}

int	export_variables(t_shell *shell)
{
	int	i;

	if (!shell || !shell->commands || !shell->commands->av)
		return (1);
	if (!shell->commands->av[1])
	{
		ft_export_env(shell);
		return (0);
	}
	i = 1;
	while (shell->commands->av[i])
	{
		if (process_export_loop(shell, i))
			return (1);
		i++;
	}
	return (0);
}

int	unset_variables(t_shell *shell)
{
	int		i;
	char	*original;

	if (!shell || !shell->commands || !shell->commands->av)
		return (1);
	i = 1;
	while (shell->commands->av[i])
	{
		if (is_valid_var_name(shell->commands->av[i]))
		{
			original = shell->commands->av[1];
			shell->commands->av[1] = shell->commands->av[i];
			del_var(shell);
			shell->commands->av[1] = original;
		}
		i++;
	}
	return (0);
}
