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
    int	exit_code;

    printf("exit\n");
    exit_code = 0;
    if (shell->commands->av[1])
    {
        if (!is_numeric(shell->commands->av[1]))
        {
            write_error_message(STDERR_FILENO, "exit", shell->commands->av[1], "numeric argument required");
            exit_code = 2;
        }
        else
            exit_code = ft_atoi(shell->commands->av[1]);
    }
    
    // ✅ LIBERA COMPLETAMENTE ANTES DE SALIR
    if (g_shell)
    {
        if (g_shell->tokens)
        {
            free_tokens(&g_shell->tokens);
            g_shell->tokens = NULL;
        }
        if (g_shell->commands)
        {
            free_commands(&g_shell->commands);
            g_shell->commands = NULL;
        }
        if (g_shell->prompt)
        {
            free(g_shell->prompt);
            g_shell->prompt = NULL;
        }
        cleanup_shell(g_shell);
    }
    
    // ✅ Libera readline history
    rl_clear_history();
    
    exit(exit_code);
}