/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:33 by gguardam          #+#    #+#             */
/*   Updated: 2025/11/18 18:31:34 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	add_or_append(t_shell *shell, int i)
{
	int		j;
	int		flag;
	char	*var_name;
	char	*equal_pos;

	j = 0;
	flag = 0;
	while(shell->commands->av[i][j])
	{
		if((shell->commands->av[i][j] == '=') && (shell->commands->av[i][j - 1] == '+'))
		{
			flag = 1;
			break ;
		}
		j++;
	}
	equal_pos = ft_strchr(shell->commands->av[i], '=');
	var_name = extract_var_name(shell->commands->av[i], equal_pos);
	if(ft_strchr(var_name, '+'))
		var_name[ft_strlen(var_name) - 1] = '\0';
	if(find_variable_index(shell->env, var_name, ft_strlen(var_name)) != -1)
		return (flag);
	else
		return (0);
}

static int	process_export_arg(t_shell *shell, char *arg)
{
	char	*equals_pos;
	char	*var_name;
	char	*var_value;
	char	*new_var;

	var_name = NULL;
	var_value = NULL;
	new_var = NULL;
	equals_pos = ft_strchr(arg, '=');
	if(equals_pos)
	{
		var_name = extract_var_name(arg, equals_pos);
		if(ft_strchr(var_name, '+'))
		{
			var_name[ft_strlen(var_name) - 1] = '=';
			var_name[ft_strlen(var_name)] = '\0';
			var_value = extract_var_value(arg);
			new_var = ft_strjoin(var_name, var_value);
		}
	}
	if(is_valid_var_name(var_name) || is_valid_var_name(arg))
	{
		if(new_var)
			add_or_modify_var(shell->env, new_var);
		else
			add_or_modify_var(shell->env, arg);
		return (0);
	}
	if (var_name != arg)
		free(var_name);
	return (write_error_message(shell->commands->out_fd, "export", arg, \
"not a valid identifier"));
}

static int	append_arg(t_shell *shell, char *arg)
{
	int		i;
	char	*mod_env;
	char	*mod_env_name;
	char	*equal_pos;
	char	*env_var;
	char	*var_value;

	i = 0;
	equal_pos = ft_strchr(arg, '=');
	mod_env_name = extract_var_name(arg, equal_pos);
	if (!mod_env_name)
		return (1);
	mod_env_name[ft_strlen(mod_env_name) - 1] = '\0';
	while(shell->env[i])
	{
		equal_pos = ft_strchr(shell->env[i], '=');
		env_var = extract_var_name(shell->env[i], equal_pos);
		if(!ft_strcmp(env_var, mod_env_name))
		{
			if (env_var != shell->env[i])
				free(env_var);
			break ;
		}
		if (env_var != shell->env[i])
			free(env_var);
		i++;
	}
	equal_pos = ft_strchr(arg, '=');
	if (!equal_pos)
	{
		free(mod_env_name);
		return (1);
	}
	var_value = extract_var_value(arg);
	if (!var_value)
	{
		free(mod_env_name);
		return (1);
	}
	mod_env = ft_strjoin(shell->env[i], var_value);
	free(var_value);
	if(!mod_env)
	{
		free(mod_env_name);
		return (1);
	}
	shell->env[i] = mod_env;
	free(mod_env_name);
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

/*int	unset_variables(t_shell *shell)
//{
//	int		i;
//	char	*original;

//	if (!shell || !shell->commands || !shell->commands->av)
//		return ;
//	i = 1;
//	while (shell->commands->av[i])
//	{
//		if (is_valid_var_name(shell->commands->av[i]))
//		{
//			original = shell->commands->av[1];
//			shell->commands->av[1] = shell->commands->av[i];
//			del_var(shell);
//			shell->commands->av[1] = original;
//		}
//		else
//			write_error_message(shell->commands->out_fd, "unset", \
//shell->commands->av[i], "not a valid identifier");
//		i++;
//	}
//	return (0);
//}*/

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
