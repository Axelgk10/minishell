/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envs_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:42 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 18:06:03 by axgimene         ###   ########.fr       */
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

static void	preparing_removal(t_shell *shell, \
char *var_to_delete, int var_name_len)
{
	int	i;

	i = 0;
	while (shell->env && shell->env[i])
	{
		if (var_matches(shell->env[i], var_to_delete, var_name_len))
		{
			remove_from_array(shell->env, i);
			return ;
		}
		i++;
	}
	i = 0;
	while (shell->local_vars && shell->local_vars[i])
	{
		if (var_matches(shell->local_vars[i], var_to_delete, var_name_len))
		{
			remove_from_array(shell->local_vars, i);
			return ;
		}
		i++;
	}
}

void	del_var(t_shell *shell)
{
	int		var_name_len;
	char	*var_to_delete;

	if (!shell || !shell->commands || !shell->commands->av[1])
		return ;
	var_to_delete = shell->commands->av[1];
	var_name_len = ft_strlen(var_to_delete);
	preparing_removal(shell, var_to_delete, var_name_len);
}

char	*move_local_var_to_env(t_shell *shell, char *var_name)
{
	int		i;
	int		var_name_len;
	char	*found_var;

	if (!shell || !shell->local_vars || !var_name)
		return (NULL);
	var_name_len = ft_strlen(var_name);
	i = 0;
	while (shell->local_vars[i])
	{
		if (var_matches(shell->local_vars[i], var_name, var_name_len))
		{
			found_var = ft_strdup(shell->local_vars[i]);
			remove_from_array(shell->local_vars, i);
			return (found_var);
		}
		i++;
	}
	return (NULL);
}
