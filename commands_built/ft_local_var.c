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

static void	inspect_local_vars(char ***local_env, char *arg)
{
	int		i;
	char	*name_var;
	char	*equals_pos;

	equals_pos = ft_strchr(arg, '=');
	name_var = extract_var_name(arg, equals_pos);
	if (!*local_env)
	{
		*local_env = malloc(sizeof(char *) * 2);
		(*local_env)[0] = ft_strdup(arg);
		(*local_env)[1] = NULL;
		return ((name_var != arg) && (free(name_var), 0), (void)0);
	}
	i = -1;
	while ((*local_env)[++i])
	{
		if (!ft_strncmp((*local_env)[i], name_var, ft_strlen(name_var)))
			return (free((*local_env)[i]), (*local_env)[i] = ft_strdup(arg),
				(name_var != arg) && (free(name_var), 0), (void)0);
	}
	add_local_var(local_env, arg);
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
	(var_name != arg) && (free(var_name), 0);
	return (0);
}

int	set_local_var(t_shell *shell)
{
	int		i;
	int		j;

	i = -1;
	while (shell->commands->av[++i])
	{
		j = -1;
		while (shell->commands->av[i][++j] != '=')
			;
		if (j == 0)
			return (write_error_message(STDERR_FILENO, shell->commands->av[0],
					"", "command not found"));
		if (process_var(shell, shell->commands->av[i]))
			return (1);
	}
	return (0);
}
