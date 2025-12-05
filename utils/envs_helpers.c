/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envs_helpers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 18:15:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 18:17:20 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	remove_from_array(char **array, int index)
{
	char	*temp;

	temp = array[index];
	while (array[index + 1])
	{
		array[index] = array[index + 1];
		index++;
	}
	array[index] = NULL;
	free(temp);
}

int	var_matches(char *env_var, char *var_name, int name_len)
{
	if (ft_strncmp(env_var, var_name, name_len) == 0 && \
(env_var[name_len] == '=' || env_var[name_len] == '\0'))
		return (1);
	return (0);
}

int	do_append(t_shell *shell, char *arg, char *mod_env_name, int i)
{
	char	*equal_pos;
	char	*var_value;
	char	*mod_env;

	equal_pos = ft_strchr(arg, '=');
	if (!equal_pos && (free(mod_env_name), 1))
		return (1);
	var_value = extract_var_value(arg);
	if (!var_value && (free(mod_env_name), 1))
		return (1);
	mod_env = ft_strjoin(shell->env[i], var_value);
	free(var_value);
	if (!mod_env && (free(mod_env_name), 1))
		return (1);
	free(shell->env[i]);
	shell->env[i] = mod_env;
	free(mod_env_name);
	return (0);
}

int	find_env_index(char **env, char *mod_env_name)
{
	int		i;
	char	*equal_pos;
	char	*env_var;

	i = 0;
	while (env[i])
	{
		equal_pos = ft_strchr(env[i], '=');
		env_var = extract_var_name(env[i], equal_pos);
		if (!ft_strcmp(env_var, mod_env_name))
		{
			if (env_var != env[i])
				free(env_var);
			return (i);
		}
		if (env_var != env[i])
			free(env_var);
		i++;
	}
	return (-1);
}

void	add_var_to_env(t_shell *shell, char *arg, char *var_name, \
char *new_var)
{
	char	*local_var;

	local_var = move_local_var_to_env(shell, var_name);
	if (new_var)
		add_or_modify_var(shell->env, new_var);
	else if (local_var && (add_or_modify_var(shell->env, local_var), 1))
		free(local_var);
	else
		add_or_modify_var(shell->env, arg);
}
