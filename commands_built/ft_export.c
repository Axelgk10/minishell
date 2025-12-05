/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:35:25 by gguardam          #+#    #+#             */
/*   Updated: 2025/12/05 15:39:55 by axgimene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

static void	print_sorted_var(char *var, int out_fd)
{
	char	*equals_pos;

	equals_pos = ft_strchr(var, '=');
	ft_putstr_fd("declare -x ", out_fd);
	if (equals_pos)
	{
		*equals_pos = '\0';
		ft_putstr_fd(var, out_fd);
		ft_putstr_fd("=\"", out_fd);
		ft_putstr_fd(equals_pos + 1, out_fd);
		ft_putstr_fd("\"\n", out_fd);
		*equals_pos = '=';
	}
	else
	{
		ft_putstr_fd(var, out_fd);
		ft_putstr_fd("\n", out_fd);
	}
}

void	ft_export_env(t_shell *shell)
{
	int		i;
	int		count;
	char	**sorted_vars;

	count = 0;
	while (shell->env[count])
		count++;
	sorted_vars = malloc(sizeof(char *) * (count + 1));
	if (!sorted_vars)
		return ;
	i = -1;
	while (++i < count)
		sorted_vars[i] = shell->env[i];
	sorted_vars[count] = NULL;
	sort_env_vars(sorted_vars, count);
	i = -1;
	while (++i < count)
		print_sorted_var(sorted_vars[i], shell->commands->out_fd);
	free(sorted_vars);
}
