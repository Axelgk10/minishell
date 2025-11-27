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

int	change_directory(char *path)
{
	char	*expanded_path;
	char	*oldpwd;
	char	cwd_buffer[4096];

	if (!path)
	{
		path = getenv("HOME");
		if (!path)
		{
			printf("cd: HOME not set\n");
			return (1);
		}
		oldpwd = getcwd(cwd_buffer, sizeof(cwd_buffer));
		if (oldpwd)
			setenv("OLDPWD", oldpwd, 1);
		chdir(path);
		getcwd(cwd_buffer, sizeof(cwd_buffer));
		setenv("PWD", cwd_buffer, 1);
	}
	else if (ft_strncmp(path, "-", 1) == 0 && path[1] == '\0')
	{
		oldpwd = getenv("OLDPWD");
		if (!oldpwd)
		{
			printf("cd: OLDPWD not set\n");
			return (1);
		}
		printf("%s\n", oldpwd);
		setenv("OLDPWD", getenv("PWD"), 1);
		chdir(oldpwd);
		getcwd(cwd_buffer, sizeof(cwd_buffer));
		setenv("PWD", cwd_buffer, 1);
	}
	else if (path[0] == '~' && (path[1] == '\0' || path[1] == '/'))
	{
		expanded_path = ft_strjoin(getenv("HOME"), path + 1);
		if (!expanded_path)
			return (1);
		oldpwd = getcwd(cwd_buffer, sizeof(cwd_buffer));
		if (oldpwd)
			setenv("OLDPWD", oldpwd, 1);
		if (chdir(expanded_path) != 0)
		{
			perror("cd");
			free(expanded_path);
			return (1);
		}
		getcwd(cwd_buffer, sizeof(cwd_buffer));
		setenv("PWD", cwd_buffer, 1);
		free(expanded_path);
	}
	else
	{
		oldpwd = getcwd(cwd_buffer, sizeof(cwd_buffer));
		if (oldpwd)
			setenv("OLDPWD", oldpwd, 1);
		if (chdir(path) != 0)
		{
			perror("cd");
			return (1);
		}
		getcwd(cwd_buffer, sizeof(cwd_buffer));
		setenv("PWD", cwd_buffer, 1);
	}
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
