/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_local_var.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:35:30 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 15:56:09 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	add_local_var(char ***local_env, char *arg)
{
	int		i;
	char	**new_env;

	i = 0;
	while ((*local_env)[i])
		i++;
	new_env = malloc(sizeof(char *) * (i + 2));
	while (i-- > 0)
		new_env[i] = (*local_env)[i];
	i = 0;
	while ((*local_env)[i])
		i++;
	new_env[i] = ft_strdup(arg);
	new_env[i + 1] = NULL;
	free(*local_env);
	*local_env = new_env;
}

static void	handle_existing_var(char ***local_env, char *arg, char *name_var)
{
	int		i;
	char	*new_var;
	char	*var_value;
	int		name_len;

	name_len = ft_strlen(name_var);
	i = 0;
	while ((*local_env)[i])
	{
		if (!ft_strncmp((*local_env)[i], name_var, name_len) \
&& ((*local_env)[i][name_len] == '=' || \
(*local_env)[i][name_len] == '\0'))
		{
			var_value = extract_var_value(arg);
			if (should_append(arg))
				new_var = ft_strjoin((*local_env)[i], var_value);
			else
				new_var = ft_strjoin(name_var, ft_strchr(arg, '='));
			free((*local_env)[i]);
			free(var_value);
			(*local_env)[i] = new_var;
			return ;
		}
		i++;
	}
	add_local_var(local_env, arg);
}

static void	inspect_local_vars(char ***local_env, char *arg)
{
	char	*name_var;
	char	*equals_pos;
	char	*processed_arg;

	equals_pos = ft_strchr(arg, '=');
	name_var = extract_var_name(arg, equals_pos);
	if (!*local_env)
	{
		*local_env = malloc(sizeof(char *) * 2);
		if (should_append(arg))
		{
			processed_arg = create_assignment_from_append(arg);
			(*local_env)[0] = ft_strdup(processed_arg);
			free(processed_arg);
		}
		else
			(*local_env)[0] = ft_strdup(arg);
		(*local_env)[1] = NULL;
		if (name_var != arg)
			free(name_var);
		return ;
	}
	handle_existing_var(local_env, arg, name_var);
	if (name_var != arg)
		free(name_var);
}

static int	process_var(t_shell *shell, char *arg)
{
	char	*equals_pos;
	char	*var_name;

	equals_pos = ft_strchr(arg, '=');
	var_name = extract_var_name(arg, equals_pos);
	if (!equals_pos || !var_name)
		return (1);
	if (is_valid_var_name(var_name))
		inspect_local_vars(&shell->local_vars, arg);
	else
		write_error_message(shell->commands->out_fd, "export",
			arg, "not a valid identifier");
	if ((var_name != arg))
		free(var_name);
	return (0);
}

int	set_local_var(t_shell *shell)
{
	int		i;

	i = -1;
	while (shell->commands->av[++i])
	{
		if (!ft_strchr(shell->commands->av[i], '='))
			continue ;
		if (process_var(shell, shell->commands->av[i]))
			return (1);
	}
	return (0);
}
