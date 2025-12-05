/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envs_export.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 18:00:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 18:15:04 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	check_append_flag(char *str)
{
	int	j;

	j = 0;
	while (str[j])
	{
		if ((str[j] == '=') && (str[j - 1] == '+'))
			return (1);
		j++;
	}
	return (0);
}

int	var_exists(t_shell *shell, char *var_name, int name_len)
{
	if (find_variable_index(shell->env, var_name, name_len) != -1)
		return (1);
	if (shell->local_vars && \
find_variable_index(shell->local_vars, var_name, name_len) != -1)
		return (1);
	return (0);
}

char	*process_plus_append(char *var_name, char *arg)
{
	char	*var_value;
	char	*new_var;

	if (!ft_strchr(var_name, '+'))
		return (NULL);
	var_name[ft_strlen(var_name) - 1] = '=';
	var_name[ft_strlen(var_name)] = '\0';
	var_value = extract_var_value(arg);
	new_var = ft_strjoin(var_name, var_value);
	free(var_value);
	return (new_var);
}

void	cleanup_export_vars(char *var_name, char *new_var, \
char *local_var, char *arg)
{
	if (var_name && var_name != arg)
		free(var_name);
	if (new_var)
		free(new_var);
	if (local_var)
		free(local_var);
}

int	add_or_append(t_shell *shell, int i)
{
	char	*var_name;
	char	*equal_pos;
	int		flag;
	int		result;

	flag = check_append_flag(shell->commands->av[i]);
	equal_pos = ft_strchr(shell->commands->av[i], '=');
	var_name = extract_var_name(shell->commands->av[i], equal_pos);
	if (ft_strchr(var_name, '+'))
		var_name[ft_strlen(var_name) - 1] = '\0';
	if (var_exists(shell, var_name, ft_strlen(var_name)))
		result = flag;
	else
		result = 0;
	if (var_name != shell->commands->av[i])
		free(var_name);
	return (result);
}
