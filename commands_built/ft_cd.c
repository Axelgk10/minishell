#include "../minishell.h"

static void	is_pwd(t_shell *shell, int i)
{
	char	*new_pwd;
	char	*current_dir;

	current_dir = getcwd(NULL, 0);
	if (!current_dir)
		return ;
	new_pwd = ft_strjoin("PWD=", current_dir);
	if (new_pwd)
	{
		free(shell->env[i]);
		shell->env[i] = new_pwd;
	}
	free(current_dir);
}

static void	is_oldpwd(t_shell *shell, int i, char	*old_pwd)
{
	char	*new_oldpwd;

	new_oldpwd = ft_strjoin("OLDPWD=", old_pwd);
	if (new_oldpwd)
	{
		free(shell->env[i]);
		shell->env[i] = new_oldpwd;
	}
}

void	update_envs(t_shell *shell)
{
	int		i;
	char	*old_pwd;

	old_pwd = NULL;
	i = 0;
	while(shell->env[i])
	{
		if (ft_strncmp(shell->env[i], "PWD=", 4) == 0)
		{
			old_pwd = ft_strdup(shell->env[i] + 4);
			break;
		}
		i++;
	}
	i = 0;
	while(shell->env[i])
	{
		if (ft_strncmp(shell->env[i], "PWD=", 4) == 0)
			is_pwd(shell, i);
		else if (ft_strncmp(shell->env[i], "OLDPWD=", 7) == 0 && old_pwd)
			is_oldpwd(shell, i, old_pwd);
		i++;
	}
	if (old_pwd)
		free(old_pwd);
}
