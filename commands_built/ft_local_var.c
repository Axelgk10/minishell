/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_local_var.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 11:22:14 by gguardam          #+#    #+#             */
/*   Updated: 2025/11/26 11:28:43 by gguardam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void add_local_var(char ***local_env, char *arg)
{
	int i;
	char **new_env;
	int j;

	i = 0;
	while ((*local_env)[i])
		i++;
	new_env = malloc(sizeof(char *) * (i + 2));
	j = 0;
	while (j < i)
	{
		new_env[j] = (*local_env)[j];
		j++;
	}
	new_env[i] = ft_strdup(arg);
	new_env[i + 1] = NULL;
	free(*local_env);
	*local_env = new_env;
}

static void inspect_local_vars(char ***local_env, char *arg)
{
	int	i;
	char	*name_var;
	char	*equals_pos;

	i = 0;
	equals_pos = ft_strchr(arg, '=');
	name_var = extract_var_name(arg, equals_pos);
	if(!*local_env)
	{
		*local_env = malloc(sizeof(char *) * 2);
		(*local_env)[0] = ft_strdup(arg);
		(*local_env)[1] = NULL;
		return ;
	}
	while((*local_env)[i])
	{
		if(!ft_strncmp((*local_env)[i], name_var, ft_strlen(name_var)))
		{
			free((*local_env)[i]);
			(*local_env)[i] = ft_strdup(arg);
			return ;
		}
		i++;
	}
	add_local_var(local_env, arg);
	if(name_var)
		free(name_var);
}

int	set_local_var(t_shell *shell)
{
	int		i;
	int		j;
	char	*equals_pos;
	char	*var_name;

	i = 0;
	while(shell->commands->av[i])
	{
		j = 0;
		while(shell->commands->av[i][j] != '=')
			j++;
		if(j > 0)
		{
			equals_pos = ft_strchr(shell->commands->av[i], '=');
			var_name = extract_var_name(shell->commands->av[i], equals_pos);
			if (!var_name)
				return (1);
			if (is_valid_var_name(var_name))
				inspect_local_vars(&shell->local_vars, shell->commands->av[i]);
			else
				write_error_message(shell->commands->out_fd, "export", shell->commands->av[i], \
"not a valid identifier");
			if (var_name != shell->commands->av[i])
				free(var_name);
		}
		else
			return (write_error_message(STDERR_FILENO, shell->commands->av[0], "", "command not found"));
		i++;
	}
	return (0);
}
