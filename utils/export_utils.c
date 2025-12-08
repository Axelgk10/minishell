/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:44 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 18:04:30 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

//char	*create_var_without_value(char *var_assignment, int *name_len)
//{
//	char	*new_var;
//	int		total_len;
//	*name_len = ft_strlen(var_assignment);
//	total_len = *name_len + 2;
//	new_var = malloc(total_len);
//	if (!new_var)
//		return (NULL);
//	ft_strlcpy(new_var, var_assignment, *name_len + 1);
//	ft_strlcat(new_var, "=", total_len);
//	return (new_var);
//}

char	*create_var_with_value(char *var_assignment, char *equals_pos, \
char **var_name, int *name_len)
{
	char	*new_var;
	int		total_len;

	*name_len = equals_pos - var_assignment;
	total_len = ft_strlen(var_assignment) + 1;
	new_var = malloc(total_len);
	if (!new_var)
		return (NULL);
	ft_strlcpy(new_var, var_assignment, total_len);
	*var_name = malloc(*name_len + 1);
	if (!*var_name)
	{
		free(new_var);
		return (NULL);
	}
	ft_strlcpy(*var_name, var_assignment, *name_len + 1);
	return (new_var);
}

void	update_env_array(char **env_var, char *new_var, char *var_name, \
int name_len)
{
	int		var_index;
	int		env_count;

	var_index = find_variable_index(env_var, var_name, name_len);
	if (var_index != -1)
	{
		free(env_var[var_index]);
		env_var[var_index] = new_var;
		return ;
	}
	env_count = count_env_vars(env_var);
	env_var[env_count] = new_var;
	env_var[env_count + 1] = NULL;
}

void	add_or_modify_var(char **env_var, char *var_assignment)
{
	char	*equals_pos;
	char	*var_name;
	char	*new_var;
	int		name_len;

	equals_pos = ft_strchr(var_assignment, '=');
	if (!equals_pos)
	{
		name_len = ft_strlen(var_assignment);
		new_var = ft_strdup(var_assignment);
		var_name = var_assignment;
	}
	else
	{
		new_var = create_var_with_value(var_assignment, equals_pos, \
&var_name, &name_len);
	}
	if (new_var)
		update_env_array(env_var, new_var, var_name, name_len);
	if (equals_pos && var_name != var_assignment)
		free(var_name);
}

char	*extract_var_name(char *var_assignment, char *equals_pos)
{
	char	*var_name;
	int		name_len;

	if (equals_pos)
	{
		name_len = equals_pos - var_assignment;
		if (name_len > 0 && var_assignment[name_len - 1] == '+')
			name_len--;
		var_name = malloc(name_len + 1);
		if (!var_name)
			return (NULL);
		ft_strlcpy(var_name, var_assignment, name_len + 1);
	}
	else
		var_name = var_assignment;
	return (var_name);
}

char	*extract_var_value(char *var_assignment)
{
	char	*var_value;
	int		value_len;
	int		name_len;
	int		i;

	i = 0;
	while (var_assignment[i] != '=')
		i++;
	name_len = i;
	value_len = ft_strlen(var_assignment) - name_len - 1;
	var_value = malloc(value_len + 1);
	if (!var_value)
		return (NULL);
	i = 0;
	while (i < value_len)
	{
		var_value[i] = var_assignment[name_len + 1 + i];
		i++;
	}
	var_value[i] = '\0';
	return (var_value);
}
