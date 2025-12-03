#include "../minishell.h"

char	*get_home_shortcut(char *cwd)
{
	char	*home;
	char	*result;
	size_t	cwd_len;

	if (!cwd)
		return (ft_strdup("Minishell$ "));
	home = getenv("HOME");
	if (!home)
		return (ft_strdup(cwd));
	cwd_len = ft_strlen(cwd);
	if (ft_strncmp(cwd, home, ft_strlen(home)) == 0)
	{
		if (cwd[ft_strlen(home)] == '\0')
			return (ft_strdup("~"));
		else if (cwd[ft_strlen(home)] == '/')
		{
			result = malloc(cwd_len - ft_strlen(home) + 2);
			if (!result)
				return (ft_strdup(cwd));
			result[0] = '~';
			ft_strlcpy(result + 1, cwd + ft_strlen(home), cwd_len - ft_strlen(home) + 1);
			return (result);
		}
	}
	return (ft_strdup(cwd));
}

static int	going_home(char *path, t_shell *shell)
{
	char	*expanded_path;

	expanded_path = ft_strjoin(get_env_values(shell->env, "HOME"), path + 1);
	if (!expanded_path)
		return (shell->exit_status = 1);
	if (chdir(expanded_path) != 0)
	{
		perror("cd");
		free(expanded_path);
		return (shell->exit_status = 1);
	}
	free(expanded_path);
	return (shell->exit_status);
}

int	change_directory(char *path, t_shell *shell)
{
	char	*oldpwd;

	if (!path)
	{
		path = get_env_values(shell->env, "HOME");
		if (!path)
			return (perror("cd: HOME not set\n"), 1);
	}
	else if (ft_strncmp(path, "-", 1) == 0 && path[1] == '\0')
	{
		oldpwd = get_env_values(shell->env, "OLDPWD");
		if (!oldpwd)
			return (perror("cd: OLDPWD not set\n"), 1);
		ft_putstr_fd(oldpwd, shell->commands->out_fd);
		chdir(oldpwd);
	}
	else if (path[0] == '~' && (path[1] == '\0' || path[1] == '/'))
		going_home(path, shell);
	else
	{
		if (chdir(path) != 0)
			return (perror("cd"), 1);
	}
	return (0);
}

char	*format_cwd(char *cwd)
{
	char	*processed_cwd;
	char	*result;
	size_t	len;

	if (!cwd)
		return (ft_strdup("Minishell$ ")); //Esto esta mal, porque si falla se deberia romper y no seguir.
	processed_cwd = get_home_shortcut(cwd);
	if (!processed_cwd)
		return (ft_strdup("Minishell$ ")); //Esto esta mal, porque si falla se deberia romper y no seguir.
	len = ft_strlen(processed_cwd) + 3;
	result = malloc(len * sizeof(char));
	if (!result)
	{
		free(processed_cwd);
		return (ft_strdup("Minishell$ ")); //Esto esta mal, porque si falla se deberia romper y no seguir.
	}
	ft_strlcpy(result, processed_cwd, len);
	ft_strlcat(result, "$ ", len);
	free(processed_cwd);
	return (result);
}
