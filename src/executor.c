/* *** comentarios *** */

#include "../minishell.h"
#include <sys/wait.h>
#include <signal.h>

// ============================================================================
// BUILTINS
// ============================================================================

int	builtin_echo(t_cmd *cmd)
{
    int	i;
    int	newline;

    if (!cmd || !cmd->as)
        return (0);

    newline = 1;
    i = 1;
    
    // Verificar flag -n
    if (cmd->as[i] && ft_strncmp(cmd->as[i], "-n", 2) == 0)
    {
        newline = 0;
        i++;
    }

    // Imprimir argumentos
    while (cmd->as[i])
    {
        ft_putstr_fd(cmd->as[i], STDOUT_FILENO);
        if (cmd->as[i + 1])
            ft_putstr_fd(" ", STDOUT_FILENO);
        i++;
    }

    if (newline)
        ft_putstr_fd("\n", STDOUT_FILENO);

    return (0);
}

int	builtin_pwd(void)
{
    char	cwd[4096];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("pwd");
        return (1);
    }
    printf("%s\n", cwd);
    return (0);
}

int	builtin_env(t_shell *shell)
{
    int	i;

    if (!shell || !shell->env)
        return (1);

    i = 0;
    while (shell->env[i])
    {
        printf("%s\n", shell->env[i]);
        i++;
    }
    return (0);
}

int	builtin_cd(t_shell *shell, t_cmd *cmd)
{
    char	*path;
    char	cwd[4096];

    (void)shell;
    
    if (!cmd || !cmd->as)
        return (1);

    // Si no hay argumento, ir a HOME
    if (!cmd->as[1])
    {
        path = getenv("HOME");
        if (!path)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return (1);
        }
    }
    else
        path = cmd->as[1];

    if (chdir(path) == -1)
    {
        perror("cd");
        return (1);
    }

    // Actualizar PWD
    if (getcwd(cwd, sizeof(cwd)))
        printf("%s\n", cwd);

    return (0);
}

int	builtin_exit(t_shell *shell, t_cmd *cmd)
{
    int	status;

    status = 0;
    if (cmd->as[1])
        status = ft_atoi(cmd->as[1]);
    else
        status = shell->exit_status;

    cleanup_shell(shell);
    exit(status);
}

// ✅ CORRECCIÓN 1: Eliminar variables no usadas
int	builtin_export(t_shell *shell, t_cmd *cmd)
{
    int	i;

    (void)cmd;
    
    if (!shell || !shell->env)
        return (1);

    // Si no hay argumentos, mostrar entorno
    if (!cmd->as[1])
        return (builtin_env(shell));

    i = 0;
    while (shell->env[i])
        i++;

    return (0);
}

int	builtin_unset(t_shell *shell, t_cmd *cmd)
{
    (void)shell;
    (void)cmd;
    return (0);
}

// ============================================================================
// DISPATCHER DE BUILTINS
// ============================================================================

int	execute_builtin(t_shell *shell, t_cmd *cmd)
{
    if (!cmd || !cmd->as || !cmd->as[0])
        return (1);

    if (ft_strncmp(cmd->as[0], "echo", 4) == 0)
        return (builtin_echo(cmd));
    else if (ft_strncmp(cmd->as[0], "cd", 2) == 0)
        return (builtin_cd(shell, cmd));
    else if (ft_strncmp(cmd->as[0], "pwd", 3) == 0)
        return (builtin_pwd());
    else if (ft_strncmp(cmd->as[0], "env", 3) == 0)
        return (builtin_env(shell));
    else if (ft_strncmp(cmd->as[0], "exit", 4) == 0)
        return (builtin_exit(shell, cmd));
    else if (ft_strncmp(cmd->as[0], "export", 6) == 0)
        return (builtin_export(shell, cmd));
    else if (ft_strncmp(cmd->as[0], "unset", 5) == 0)
        return (builtin_unset(shell, cmd));

    return (1);
}

// ============================================================================
// EJECUTAR COMANDO SIMPLE (SIN PIPELINE)
// ============================================================================

int	execute_single_command(t_shell *shell, t_cmd *cmd)
{
    pid_t	pid;
    int		status;

    if (!shell || !cmd || !cmd->as || !cmd->as[0])
        return (1);

    // Si es un builtin, ejecutarlo directamente
    if (cmd->is_builtin)
        return (execute_builtin(shell, cmd));

    // Fork para ejecutar comando externo
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return (1);
    }

    // Proceso hijo
    if (pid == 0)
    {
        // ✅ Validar antes de redirigir
        if (cmd->in_fd > 2)
        {
            dup2(cmd->in_fd, STDIN_FILENO);
            close(cmd->in_fd);
        }

        // ✅ Validar antes de redirigir
        if (cmd->out_fd > 2)
        {
            dup2(cmd->out_fd, STDOUT_FILENO);
            close(cmd->out_fd);
        }

        // Ejecutar comando
        execvp(cmd->as[0], cmd->as);
        perror(cmd->as[0]);
        exit(127);
    }

    // Proceso padre - esperar a hijo
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        shell->exit_status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        shell->exit_status = 128 + WTERMSIG(status);

    return (shell->exit_status);
}

// ============================================================================
// EJECUTAR PIPELINE (MÚLTIPLES COMANDOS)
// ============================================================================

int	execute_pipeline(t_shell *shell, t_cmd *commands)
{
    t_cmd	*current;
    pid_t	*pids;
    int		count;
    int		i;
    int		status;
    int		pipe_fd[2];
    int		prev_pipe_out;

    if (!shell || !commands)
        return (1);

    // Contar comandos
    current = commands;
    count = 0;
    while (current)
    {
        count++;
        current = current->next;
    }

    // Asignar array de PIDs
    pids = malloc(sizeof(pid_t) * count);
    if (!pids)
        return (1);

    current = commands;
    i = 0;
    prev_pipe_out = -1;

    // Ejecutar cada comando en el pipeline
    while (current)
    {
        // Crear pipe si no es el último comando
        if (current->next)
        {
            if (pipe(pipe_fd) == -1)
            {
                perror("pipe");
                free(pids);
                return (1);
            }
        }

        pids[i] = fork();
        if (pids[i] == -1)
        {
            perror("fork");
            free(pids);
            return (1);
        }

        // Proceso hijo
        if (pids[i] == 0)
        {
            // Redirigir entrada
            if (i > 0 && prev_pipe_out > 2)
                dup2(prev_pipe_out, STDIN_FILENO);
            else if (current->in_fd != STDIN_FILENO && current->in_fd > 2)
                dup2(current->in_fd, STDIN_FILENO);

            // Redirigir salida
            if (current->next)
                dup2(pipe_fd[1], STDOUT_FILENO);
            else if (current->out_fd != STDOUT_FILENO && current->out_fd > 2)
                dup2(current->out_fd, STDOUT_FILENO);

            // Cerrar file descriptors
            if (prev_pipe_out > 2)
                close(prev_pipe_out);
            if (current->next)
            {
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }

            // Ejecutar comando
            if (current->is_builtin)
                exit(execute_builtin(shell, current));
            execvp(current->as[0], current->as);
            perror(current->as[0]);
            exit(127);
        }

        // Proceso padre
        if (prev_pipe_out > 2)
            close(prev_pipe_out);
        
        if (current->next)
        {
            close(pipe_fd[1]);
            prev_pipe_out = pipe_fd[0];
        }

        current = current->next;
        i++;
    }

    // Esperar a todos los procesos hijos
    for (int j = 0; j < count; j++)
        waitpid(pids[j], &status, 0);

    if (WIFEXITED(status))
        shell->exit_status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        shell->exit_status = 128 + WTERMSIG(status);

    free(pids);
    return (shell->exit_status);
}

// ============================================================================
// ROUTER PRINCIPAL
// ============================================================================

int	execute_commands(t_shell *shell)
{
    if (!shell || !shell->commands)
        return (1);

    // Si hay solo un comando
    if (!shell->commands->next)
        return (execute_single_command(shell, shell->commands));

    // Si hay múltiples comandos (pipeline)
    return (execute_pipeline(shell, shell->commands));
}