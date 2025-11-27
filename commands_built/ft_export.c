#include "../minishell.h"

//Mostrar "declare -x VAR="var", en orden alfabetico y los valores enter ""

static int	compare_env_vars(char *var1, char *var2)
{
	int	i;

	i = 0;
	while (var1[i] && var2[i] && var1[i] == var2[i])
		i++;
	
	return ((unsigned char)var1[i] - (unsigned char)var2[i]);
}

static void	sort_env_vars(char **vars, int count)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - 1 - i)
		{
			if (compare_env_vars(vars[j], vars[j + 1]) > 0)
			{
				temp = vars[j];
				vars[j] = vars[j + 1];
				vars[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

void	ft_export_env(t_shell *shell)
{
	int		i;
	int		count;
	char	**sorted_vars;
	char	*equals_pos;

	count = 0;
	while (shell->env[count])
		count++;
	sorted_vars = malloc(sizeof(char *) * (count + 1));
	if (!sorted_vars)
		return;
	i = 0;
	while (i < count)
	{
		sorted_vars[i] = shell->env[i];
		i++;
	}
	sorted_vars[count] = NULL;
	sort_env_vars(sorted_vars, count);
	i = 0;
	while (i < count)
	{
		equals_pos = ft_strchr(sorted_vars[i], '=');
		ft_putstr_fd("declare -x ", shell->commands->out_fd);
		if (equals_pos)
		{
			*equals_pos = '\0';
			ft_putstr_fd(sorted_vars[i], shell->commands->out_fd);
			ft_putstr_fd("=\"", shell->commands->out_fd);
			ft_putstr_fd(equals_pos + 1, shell->commands->out_fd);
			ft_putstr_fd("\"\n", shell->commands->out_fd);
			*equals_pos = '=';
		}
		else
		{
			ft_putstr_fd(sorted_vars[i], shell->commands->out_fd);
			ft_putstr_fd("\n", shell->commands->out_fd);
		}
		i++;
	}
	
	free(sorted_vars);
}
