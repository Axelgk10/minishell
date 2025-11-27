#include "../minishell.h"

extern t_shell *g_shell;
extern int g_exit_status;

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
    int	exit_code;

    ft_putstr_fd("exit\n", STDOUT_FILENO);
    exit_code = 0;
    if (shell->commands && shell->commands->av && shell->commands->av[1])
    {
        if (!is_numeric(shell->commands->av[1]))
        {
            write_error_message(STDERR_FILENO, "exit", shell->commands->av[1], "numeric argument required");
            exit_code = 2;
        }
        else
            exit_code = ft_atoi(shell->commands->av[1]);
    }
    g_exit_status = exit_code;
    // ✅ NO LLAMES A exit() - solo establece el código
}