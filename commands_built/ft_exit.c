#include "../minishell.h"

extern t_shell *g_shell;

static int	is_numeric(char *str)
{
	int	i;

	i = 0;
	if (str[0] == '-' || str[0] == '+')
		i = 1;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

void	manage_exit(t_shell *shell)
{
	printf("exit\n");
	shell->exit_status = 0;
	if (shell->commands->av[1])
	{
		if (!is_numeric(shell->commands->av[1]))
		{
			write_error_message(STDERR_FILENO, "exit", shell->commands->av[1], "numeric argument required");
			shell->exit_status = 2;
		}
		else
			shell->exit_status = ft_atoi(shell->commands->av[1]);
	}
	if (shell)
		cleanup_shell(shell);
	rl_clear_history();
	exit(shell->exit_status);
}
