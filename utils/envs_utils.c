/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envs_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:42 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 14:21:20 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	count_env_vars(char **env_var)
{
	int	count;

	count = 0;
	while (env_var[count])
		count++;
	return (count);
}

int	find_variable_index(char **env_var, char *var_name, int name_len)
{
	int	i;
	int	j;

	i = 0;
	while (env_var[i])
	{
		j = 0;
		while (j < name_len && env_var[i][j] && var_name[j] && \
env_var[i][j] == var_name[j])
			j++;
		if (j == name_len && (env_var[i][j] == '=' || env_var[i][j] == '\0'))
			return (i);
		i++;
	}
	return (-1);
}

int	is_valid_var_name(char *name)
{
	int	i;

	if (!name || !name[0])
		return (0);
	if (!ft_isalpha(name[0]) && name[0] != '_')
		return (0);
	i = 1;
	while (name[i] && name[i] != '=')
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

char	**get_path_values(char **env, const char *var_name)
{
	int		i;
	int		var_len;

	if (!env || !var_name)
		return (NULL);
	var_len = ft_strlen(var_name);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], var_name, var_len) == 0 \
&& env[i][var_len] == '=')
			return (ft_split((env[i] + var_len + 1), ':'));
		i++;
	}
	return (NULL);
}

void	del_var(t_shell *shell)
{
	int		i;
	int		var_name_len;
	char	*var_to_delete;
	char	*temp;

	if (!shell || !shell->commands || !shell->commands->av[1])
		return ;
	var_to_delete = shell->commands->av[1];
	var_name_len = ft_strlen(var_to_delete);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], var_to_delete, var_name_len) == 0 \
&& (shell->env[i][var_name_len] == '=' || shell->env[i][var_name_len] == '\0'))
		{
			temp = shell->env[i];
			while (shell->env[i + 1])
			{
				shell->env[i] = shell->env[i + 1];
				i++;
			}
			shell->env[i] = NULL;
			free(temp);
			break ;
		}
		else if(shell->local_vars && (ft_strncmp(shell->local_vars[i], var_to_delete, var_name_len) == 0 \
&& (shell->local_vars[i][var_name_len] == '=' || shell->local_vars[i][var_name_len] == '\0')))
		{
			temp = shell->local_vars[i];
			while (shell->local_vars[i + 1])
			{
				shell->local_vars[i] = shell->local_vars[i + 1];
				i++;
			}
			shell->local_vars[i] = NULL;
			free(temp);
			break ;
		}
		i++;
	}
}

char	*move_local_var_to_env(t_shell *shell, char *var_name)
{
	int		i;
	int		var_name_len;
	char	*found_var;
	char	*temp;

	if (!shell || !shell->local_vars || !var_name)
		return (NULL);
	var_name_len = ft_strlen(var_name);
	i = 0;
	while (shell->local_vars[i])
	{
		if (ft_strncmp(shell->local_vars[i], var_name, var_name_len) == 0 \
&& (shell->local_vars[i][var_name_len] == '=' || shell->local_vars[i][var_name_len] == '\0'))
		{
			found_var = ft_strdup(shell->local_vars[i]);
			temp = shell->local_vars[i];
			while (shell->local_vars[i + 1])
			{
				shell->local_vars[i] = shell->local_vars[i + 1];
				i++;
			}
			shell->local_vars[i] = NULL;
			free(temp);
			return (found_var);
		}
		i++;
	}
	return (NULL);
}
