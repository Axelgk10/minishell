#include "../minishell.h"

char	*get_home_shortcut(char *cwd)
{
    char	*home;
    char	*result;
    size_t	home_len;
    size_t	cwd_len;

    if (!cwd)
        return (ft_strdup("Minishell$ "));
    
    home = getenv("HOME");
    if (!home)
        return (ft_strdup(cwd));
    
    home_len = ft_strlen(home);
    cwd_len = ft_strlen(cwd);
    
    // ✅ Si el cwd empieza con HOME, reemplazarlo con ~
    if (ft_strncmp(cwd, home, home_len) == 0)
    {
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
    }
    
    // ✅ Si no es HOME, retorna el cwd sin cambios
    return (ft_strdup(cwd));
}

static char	*get_env_value_from_shell(t_shell *shell, char *var_name)
{
	int		i;
	size_t	len;

	if (!shell || !shell->env || !var_name)
		return (NULL);
	len = ft_strlen(var_name);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], var_name, len) == 0
			&& shell->env[i][len] == '=')
			return (shell->env[i] + len + 1);
		i++;
	}
	return (NULL);
}

int	change_directory(t_shell *shell, char *path)
{
	char	*expanded_path;
	char	*oldpwd;

	if (!path)
	{
		path = get_env_value_from_shell(shell, "HOME");
		if (!path)
			return (write(2, "cd: HOME not set\n", 17), 1);
		if (chdir(path) != 0)
			return (perror("cd"), 1);
	}
	else if (ft_strncmp(path, "-", 1) == 0 && path[1] == '\0')
	{
		oldpwd = get_env_value_from_shell(shell, "OLDPWD");
		if (!oldpwd)
			return (write(2, "cd: OLDPWD not set\n", 19), 1);
		write(1, oldpwd, ft_strlen(oldpwd));
		write(1, "\n", 1);
		if (chdir(oldpwd) != 0)
			return (perror("cd"), 1);
	}
	else if (path[0] == '~' && (path[1] == '\0' || path[1] == '/'))
	{
		expanded_path = ft_strjoin(get_env_value_from_shell(shell, "HOME"),
				path + 1);
		if (!expanded_path)
			return (1);
		if (chdir(expanded_path) != 0)
			return (perror("cd"), free(expanded_path), 1);
		free(expanded_path);
	}
	else
	{
		if (chdir(path) != 0)
			return (perror("cd"), 1);
	}
	update_envs(shell);
	return (0);
}

char	*format_cwd(char *cwd)
{
    char	*processed_cwd;
    char	*result;
    size_t	len;

    if (!cwd)
        return (ft_strdup("Minishell$ "));
    
    processed_cwd = get_home_shortcut(cwd);
    if (!processed_cwd)
        return (ft_strdup("Minishell$ "));
    
    len = ft_strlen(processed_cwd) + 3;
    result = malloc(len);
    if (!result)
    {
        free(processed_cwd);
        return (ft_strdup("Minishell$ "));
    }
    ft_strlcpy(result, processed_cwd, len);
    ft_strlcat(result, "$ ", len);
    free(processed_cwd);
    return (result);
}
