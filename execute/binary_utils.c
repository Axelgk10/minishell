/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   binary_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 19:15:00 by axgimene          #+#    #+#             */
/*   Updated: 2025/12/05 19:14:45 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*check_direct_access(char *command)
{
	struct stat	path_stat;

	if (access(command, X_OK) == 0 && stat(command, &path_stat) == 0
		&& S_ISREG(path_stat.st_mode))
		return (ft_strdup(command));
	return (NULL);
}

static char	*search_in_paths(char *command, char **paths)
{
	char			*tmp_path;
	char			*full_path;
	int				i;
	struct stat		path_stat;

	i = 0;
	while (paths[i])
	{
		tmp_path = ft_strjoin(paths[i], "/");
		if (!tmp_path)
			return (NULL);
		full_path = ft_strjoin(tmp_path, command);
		free(tmp_path);
		if (!full_path)
			return (NULL);
		if (access(full_path, X_OK) == 0 && stat(full_path, &path_stat) == 0
			&& S_ISREG(path_stat.st_mode))
			return (full_path);
		free(full_path);
		i++;
	}
	return (NULL);
}

char	*find_binary(char *command, char **paths)
{
	char	*result;

	result = check_direct_access(command);
	if (result)
		return (result);
	if (!command || !paths)
		return (NULL);
	return (search_in_paths(command, paths));
}

int	got_path(t_shell *shell)
{
	int	i;

	if (!shell || !shell->env)
		return (0);
	i = 0;
	while (i < shell->env_count && shell->env[i])
	{
		if (ft_strncmp(shell->env[i], "PATH=", 5) == 0)
			return (1);
		i++;
	}
	return (0);
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
