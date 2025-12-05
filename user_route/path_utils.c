/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 15:00:00 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 15:52:52 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*create_tilde_result(char *cwd, char *home, \
size_t home_len, size_t cwd_len)
{
	char	*result;

	(void)home;
	if (cwd[home_len] == '\0')
		return (ft_strdup("~"));
	else if (cwd[home_len] == '/')
	{
		result = malloc(cwd_len - home_len + 2);
		if (!result)
			return (ft_strdup(cwd));
		result[0] = '~';
		ft_strlcpy(result + 1, cwd + home_len, cwd_len - home_len + 1);
		return (result);
	}
	return (NULL);
}

char	*process_parent_directory(char *resolved)
{
	char	*last_slash;

	last_slash = ft_strrchr(resolved, '/');
	if (last_slash && last_slash != resolved)
		*last_slash = '\0';
	else if (last_slash == resolved)
		resolved[1] = '\0';
	return (resolved);
}

char	*handle_dotdot_path(char *resolved, char **remaining_path)
{
	if (ft_strncmp(*remaining_path, "../", 3) == 0)
	{
		resolved = process_parent_directory(resolved);
		*remaining_path += 3;
	}
	else if (ft_strncmp(*remaining_path, "..", 2) == 0 && \
((*remaining_path)[2] == '\0' || (*remaining_path)[2] == '/'))
	{
		resolved = process_parent_directory(resolved);
		*remaining_path += 2;
		if (**remaining_path == '/')
			(*remaining_path)++;
	}
	return (resolved);
}

static char	*append_remaining_path(char *resolved, char *remaining_path)
{
	char	*temp;
	char	*result;

	temp = ft_strjoin(resolved, "/");
	free(resolved);
	if (!temp)
		return (NULL);
	result = ft_strjoin(temp, remaining_path);
	free(temp);
	return (result);
}

char	*resolve_logical_path(char *logical_pwd, char *path)
{
	char	*resolved;
	char	*remaining_path;

	if (!logical_pwd || !path)
		return (NULL);
	resolved = ft_strdup(logical_pwd);
	if (!resolved)
		return (NULL);
	remaining_path = path;
	while (remaining_path && *remaining_path)
	{
		if (ft_strncmp(remaining_path, "..", 2) == 0)
			resolved = handle_dotdot_path(resolved, &remaining_path);
		else
		{
			resolved = append_remaining_path(resolved, remaining_path);
			break ;
		}
	}
	return (resolved);
}
