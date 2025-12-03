/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:33 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/03 13:20:27 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int evaluating_if_add_or_append(t_shell *shell, int i)
{
	int	j;
	int	flag;

	flag = 0;
	j = 0;
	while(shell->commands->av[i][j])
	{
		if((shell->commands->av[i][j] == '=') && (shell->commands->av[i][j - 1] == '+'))
		{
			flag = 1;
			break ;
		}
		j++;
	}
}

static int	add_or_append(t_shell *shell, int i)
{
	int		flag;
	char	*var_name;
	char	*equal_pos;
	int		result;

	flag = evaluating_if_add_or_append(shell, i);
	equal_pos = ft_strchr(shell->commands->av[i], '=');
	var_name = extract_var_name(shell->commands->av[i], equal_pos);
	if(ft_strchr(var_name, '+'))
		var_name[ft_strlen(var_name) - 1] = '\0';
	if(find_variable_index(shell->env, var_name, ft_strlen(var_name)) != -1)
		result = flag;
	else
		result = 0;
	if (var_name != shell->commands->av[i])
		free(var_name);
	return (result);
}

static void	do_we_have_equal_position(char *arg, char **var_name, char **var_value, char **new_var)
{
	char	*equals_pos;

	equals_pos = ft_strchr(arg, '=');
	if(equals_pos)
	{
		*var_name = extract_var_name(arg, equals_pos);
		if(ft_strchr(*var_name, '+'))
		{
			(*var_name)[ft_strlen(*var_name) - 1] = '=';
			(*var_name)[ft_strlen(*var_name)] = '\0';
			*var_value = extract_var_value(arg);
			*new_var = ft_strjoin(*var_name, *var_value);
			free(*var_value);
		}
	}
}

static int	process_export_arg(t_shell *shell, char *arg)
{
	char	*var_name;
	char	*var_value;
	char	*new_var;

	var_name = NULL;
	var_value = NULL;
	new_var = NULL;
	do_we_have_equal_position(arg, &var_name, &var_value, &new_var);
	if(is_valid_var_name(var_name) || is_valid_var_name(arg))
	{
		if(new_var)
			add_or_modify_var(shell->env, new_var);
		else
			add_or_modify_var(shell->env, arg);
		if (var_name && var_name != arg)
			free(var_name);
		return (0);
	}
	if (var_name && var_name != arg)
		free(var_name);
	if (new_var)
		free(new_var);
	return (write_error_message(shell->commands->out_fd, "export", arg, \
"not a valid identifier"));
}

static char	*extract_append_var_name(char *arg)
{
	char	*equal_pos;
	char	*mod_env_name;

	equal_pos = ft_strchr(arg, '=');
	mod_env_name = extract_var_name(arg, equal_pos);
	if (!mod_env_name)
		return (NULL);
	mod_env_name[ft_strlen(mod_env_name) - 1] = '\0';
	return (mod_env_name);
}

static int	find_env_var_index(t_shell *shell, char *target_name)
{
	int		i;
	char	*equal_pos;
	char	*env_var;

	i = 0;
	while (shell->env[i])
	{
		equal_pos = ft_strchr(shell->env[i], '=');
		env_var = extract_var_name(shell->env[i], equal_pos);
		if (!ft_strcmp(env_var, target_name))
		{
			if (env_var != shell->env[i])
				free(env_var);
			return (i);
		}
		if (env_var != shell->env[i])
			free(env_var);
		i++;
	}
	return (-1);
}

static int	perform_append_operation(t_shell *shell, int env_index, char *arg)
{
	char	*var_value;
	char	*mod_env;

	var_value = extract_var_value(arg);
	if (!var_value)
		return (1);
	mod_env = ft_strjoin(shell->env[env_index], var_value);
	free(var_value);
	if (!mod_env)
		return (1);
	free(shell->env[env_index]);
	shell->env[env_index] = mod_env;
	return (0);
}

static int	append_arg(t_shell *shell, char *arg)
{
	int		env_index;
	char	*mod_env_name;
	char	*equal_pos;

	equal_pos = ft_strchr(arg, '=');
	if (!equal_pos)
		return (1);
	mod_env_name = extract_append_var_name(arg);
	if (!mod_env_name)
		return (1);
	env_index = find_env_var_index(shell, mod_env_name);
	if (env_index == -1)
	{
		free(mod_env_name);
		return (1);
	}
	if (perform_append_operation(shell, env_index, arg))
	{
		free(mod_env_name);
		return (1);
	}
	free(mod_env_name);
	return (0);
}

int	export_variables(t_shell *shell)
{
	int	i;

	if (!shell || !shell->commands || !shell->commands->av)
		return (1);
	if (!shell->commands->av[1])
		ft_export_env(shell);
	i = 1;
	while (shell->commands->av[i])
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
