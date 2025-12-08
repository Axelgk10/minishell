/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   local_vars_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:37:33 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 18:17:20 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*create_assignment_from_append(char *arg)
{
	char	*name_var;
	char	*value;
	char	*result;
	char	*equals_pos;

	equals_pos = ft_strchr(arg, '=');
	name_var = extract_var_name(arg, equals_pos);
	value = extract_var_value(arg);
	result = malloc(ft_strlen(name_var) + 1 + ft_strlen(value) + 1);
	ft_strlcpy(result, name_var, ft_strlen(name_var) + 1);
	ft_strlcat(result, "=", ft_strlen(name_var) + 2);
	ft_strlcat(result, value, ft_strlen(name_var) + 1 + ft_strlen(value) + 1);
	free(value);
	if (name_var != arg)
		free(name_var);
	return (result);
}

int	should_append(char	*arg)
{
	int	i;

	i = 0;
	while (arg[i])
	{
		if (arg[i] == '+' && arg[i + 1] == '=')
			return (1);
		i++;
	}
	return (0);
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
