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

static int inspect_local_vars(char ***local_env, char *arg)
{
	int		i;
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
		return (0);
	}
	while((*local_env)[i])
	{
		if(!ft_strncmp((*local_env)[i], name_var, ft_strlen(name_var)))
			return (free((*local_env)[i]), (*local_env)[i] = ft_strdup(arg), 0);
		i++;
	}
	add_local_var(local_env, arg);
	if(name_var)
		free(name_var);
	return (0);
}

static int	find_equals_position(char *str)
{
	int	j;

	j = 0;
	while (str[j] && str[j] != '=')
		j++;
	return (j);
}

static int	validate_and_extract_var(char *arg, char **var_name, char **equals_pos)
{
	*equals_pos = ft_strchr(arg, '=');
	*var_name = extract_var_name(arg, *equals_pos);
	if (!*var_name)
		return (1);
	return (0);
}

static int	process_single_arg(t_shell *shell, char *arg)
{
	char	*equals_pos;
	char	*var_name;
	int		equals_index;

	equals_index = find_equals_position(arg);
	if (equals_index == 0)
		return (write_error_message(STDERR_FILENO, shell->commands->av[0], "", "command not found"));
	if (validate_and_extract_var(arg, &var_name, &equals_pos))
		return (1);
	if (is_valid_var_name(var_name))
		inspect_local_vars(&shell->local_vars, arg);
	else
		write_error_message(shell->commands->out_fd, "export", arg, "not a valid identifier");
	if (var_name != arg)
		free(var_name);
	return (0);
}

int	set_local_var(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->commands->av[i])
	{
		if (process_single_arg(shell, shell->commands->av[i]))
			return (1);
		i++;
	}
	return (0);
}
