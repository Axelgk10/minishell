# SOLUCIONES IMPLEMENTABLES - MINISHELL

## 1. FUNCIÓN HELPER PARA LIBERAR PATH_ENV

**Crear archivo:** `utils/path_utils.c`

```c
#include "../minishell.h"

void free_path_env(char **path_env)
{
    int i;

    if (!path_env)
        return;
    i = 0;
    while (path_env[i])
    {
        free(path_env[i]);
        path_env[i] = NULL;
        i++;
    }
    free(path_env);
}
```

**Agregar a minishell.h:**
```c
void free_path_env(char **path_env);
```

---

## 2. CORRECCIÓN: execution_single_command.c

**Reemplazar función `just_execute_it_man()`:**

```c
void just_execute_it_man(t_shell *shell)
{
    char    **path_env;
    char    *bin_path;
    int     status;
    pid_t   pid;

    status = 0;
    pid = fork();
    if (pid == -1)
        exit(1);
    else if (pid == 0)
    {
        fd_checker(shell);
        path_env = get_path_values(shell->env, "PATH");
        bin_path = find_binary(shell->commands->av[0], path_env);
        if (!bin_path)
        {
            if (!got_path(shell))
            {
                write_error_message(STDERR_FILENO, shell->commands->av[0], "",
                                   "No such file or directory");
            }
            else
            {
                write_error_message(STDERR_FILENO, shell->commands->av[0], "",
                                   "command not found");
            }
            free_path_env(path_env);  // ✅ AGREGADO
            exit(127);
        }
        if (execve(bin_path, shell->commands->av, shell->env) == -1)
        {
            free(bin_path);
            free_path_env(path_env);  // ✅ AGREGADO
            error_executing(2, shell->env, shell->commands->av);
        }
    }
    else if (pid > 0)
        status_wait(pid, status);
}
```

---

## 3. CORRECCIÓN: execution_piped.c - execute_child_process()

**Reemplazar función:**

```c
static int execute_child_process(t_shell *shell, t_cmd *cmd, int prev_pipe_out, int *pipe_fd)
{
    char    **path_env;
    char    *bin_path;

    setup_child_input(prev_pipe_out, cmd);
    setup_child_output(cmd, pipe_fd);
    if (cmd->is_builtin)
        exit(execute_builtin_in_pipeline(shell, cmd));
    path_env = get_path_values(shell->env, "PATH");
    bin_path = find_binary(cmd->av[0], path_env);
    if (!bin_path)
    {
        if (!got_path(shell))
        {
            write_error_message(STDERR_FILENO, cmd->av[0], "",
                               "No such file or directory");
        }
        else
        {
            write_error_message(STDERR_FILENO, cmd->av[0], "",
                               "command not found");
        }
        free_path_env(path_env);  // ✅ AGREGADO
        exit(127);
    }
    if (execve(bin_path, cmd->av, shell->env) == -1)
    {
        free(bin_path);
        free_path_env(path_env);  // ✅ AGREGADO
        error_executing(2, shell->env, cmd->av);
    }
    exit(127);
}
```

---

## 4. CORRECCIÓN: parser_tokenizer0.c - tokenize()

**Reemplazar función:**

```c
t_token *tokenize(char *input)
{
    t_token *head;
    int     i;

    head = NULL;
    i = 0;
    while (input[i] != '\0')
    {
        // ✅ CORRECCIÓN: usar input[i] en lugar de input[0]
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n')
        {
            i++;
            continue;
        }
        process_token(input, &i, &head);
    }
    return (head);
}
```

---

## 5. CORRECCIÓN: ft_echo.c - Validaciones

**Reemplazar función:**

```c
int ft_echo(t_cmd *cmd)
{
    int i;
    int newline;

    // ✅ AGREGADO: Validaciones iniciales
    if (!cmd || !cmd->av || !cmd->av[0])
        return (1);
    
    i = 1;
    // ✅ AGREGADO: Validar que av[1] existe
    if (cmd->av[i] && cmd->av[i][0] == '-' && cmd->av[i][1] == 'n')
    {
        newline = echo_newline(cmd);
        i = 1;
        // Saltar todas las opciones -n
        while (cmd->av[i] && cmd->av[i][0] == '-' && cmd->av[i][1] == 'n')
            i++;
    }
    else
    {
        newline = 1;
    }
    
    while (cmd->av[i])
    {
        ft_putstr_fd(cmd->av[i], cmd->out_fd);
        if (cmd->av[i + 1])
            ft_putchar_fd(' ', cmd->out_fd);
        i++;
    }
    if (newline)
        ft_putchar_fd('\n', cmd->out_fd);
    return (0);
}

static int echo_newline(t_cmd *cmd)
{
    int i;
    int newline;
    int j;

    // ✅ AGREGADO: Validación inicial
    if (!cmd || !cmd->av || !cmd->av[1])
        return (1);
    
    i = 1;
    newline = 1;
    while (cmd->av[i])
    {
        if (cmd->av[i][0] == '-' && cmd->av[i][1] == 'n')
        {
            j = 1;
            while (cmd->av[i][j] == 'n')
                j++;
            if (cmd->av[i][j] == '\0')
            {
                newline = 0;
                i++;
                continue;
            }
        }
        break;
    }
    return (newline);
}
```

---

## 6. CORRECCIÓN: ft_export.c - Memory leak

**Reemplazar función:**

```c
void ft_export_env(t_shell *shell)
{
    int     i;
    int     count;
    char    **sorted_vars;
    char    *equals_pos;

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
    
    // ✅ AGREGADO: Liberar array
    free(sorted_vars);
}
```

---

## 7. CORRECCIÓN: struct_utils.c - cleanup_shell()

**Reemplazar función:**

```c
void cleanup_shell(t_shell *shell)
{
    int i;
    
    if (!shell)
        return;
    
    // Libera el prompt
    if (shell->prompt)
    {
        free(shell->prompt);
        shell->prompt = NULL;
    }
    
    // Libera tokens
    if (shell->tokens)
    {
        free_tokens(&shell->tokens);
        shell->tokens = NULL;
    }
    
    // Libera commands
    if (shell->commands)
    {
        free_commands(&shell->commands);
        shell->commands = NULL;
    }
    
    // ✅ AGREGADO: Liberar env si es copia
    if (shell->env)
    {
        i = 0;
        while (shell->env[i])
        {
            free(shell->env[i]);
            shell->env[i] = NULL;
            i++;
        }
        free(shell->env);
        shell->env = NULL;
    }
    
    // Libera local_vars
    if (shell->local_vars)
    {
        i = 0;
        while (shell->local_vars[i])
        {
            free(shell->local_vars[i]);
            shell->local_vars[i] = NULL;
            i++;
        }
        free(shell->local_vars);
        shell->local_vars = NULL;
    }
}
```

---

## 8. CORRECCIÓN: parser_parser_init5.c - handle_pipe_in_parse()

**Reemplazar función:**

```c
void handle_pipe_in_parse(t_token **token, t_cmd **cmd)
{
    t_cmd *new_cmd;

    set_builtin_flag(*cmd);
    if (pipe((*cmd)->pipe) == -1)
    {
        perror("pipe");
        free_commands(cmd);
        *cmd = NULL;
        return;
    }
    new_cmd = create_command();
    if (!new_cmd)
    {
        close((*cmd)->pipe[0]);
        close((*cmd)->pipe[1]);
        free_commands(cmd);
        *cmd = NULL;
        return;
    }
    
    // ✅ MEJORADO: Validar out_fd antes de asignar
    if ((*cmd)->out_fd == -1)
    {
        (*cmd)->out_fd = (*cmd)->pipe[1];
    }
    else
    {
        // Si out_fd ya estaba asignado, hay conflicto
        // Cerrar el pipe write que no se usará
        close((*cmd)->pipe[1]);
    }
    
    new_cmd->in_fd = (*cmd)->pipe[0];
    (*cmd)->next = new_cmd;
    *cmd = new_cmd;
    *token = (*token)->next;
}
```

---

## 9. CORRECCIÓN: main.c - init_shell() debe copiar env

**Modificar init_shell() en struct_utils.c:**

```c
void init_shell(t_shell *shell, char **envp)
{
    if (!shell)
        return;
    
    // ✅ CORRECCIÓN: Copiar environment en lugar de usar directamente
    shell->env = copy_env(envp);
    if (!shell->env)
    {
        fprintf(stderr, "Error: Failed to copy environment\n");
        exit(1);
    }
    
    shell->local_vars = NULL;
    shell->tokens = NULL;
    shell->commands = NULL;
    shell->prompt = NULL;
    shell->exit_status = 0;
    shell->stdin_copy = STDIN_FILENO;
    shell->stdout_copy = STDOUT_FILENO;
}
```

---

## 10. CORRECCIÓN: parser_redirections_little_chickens3.c - Limpiar heredoc

**Modificar handle_heredoc():**

```c
int handle_heredoc(char *delimiter)
{
    int     fd;
    char    *line;

    fd = open("/tmp/heredoc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return (perror("heredoc"), -1);
    while (1)
    {
        line = readline("> ");
        if (!line || ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0)
        {
            free(line);
            break;
        }
        write(fd, line, ft_strlen(line));
        write(fd, "\n", 1);
        free(line);
    }
    close(fd);
    fd = open("/tmp/heredoc", O_RDONLY);
    if (fd == -1)
        perror("minishell: heredoc");
    return (fd);
}
```

**Agregar a cleanup_on_exit() en main.c:**

```c
static void cleanup_on_exit(void)
{
    if (g_shell)
    {
        cleanup_shell(g_shell);
        g_shell = NULL;
    }
    
    // ✅ AGREGADO: Limpiar archivo temporal
    unlink("/tmp/heredoc");
    
    rl_clear_history();
}
```

---

## 11. CORRECCIÓN: execution_utils.c - fd_checker() debe validar errores

**Modificar fd_checker():**

```c
int fd_checker(t_shell *shell)
{
    // ✅ CAMBIAR retorno a int para validar errores
    if (!shell || !shell->commands)
        return (1);
    
    if (shell->commands->out_fd != STDOUT_FILENO && shell->commands->out_fd > 0)
    {
        if (dup2(shell->commands->out_fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            return (0);  // ✅ Retorna error
        }
        close(shell->commands->out_fd);
    }
    if (shell->commands->in_fd != STDIN_FILENO && shell->commands->in_fd > 0)
    {
        if (dup2(shell->commands->in_fd, STDIN_FILENO) == -1)
        {
            perror("dup2");
            return (0);  // ✅ Retorna error
        }
        close(shell->commands->in_fd);
    }
    return (1);  // ✅ Retorna éxito
}
```

**Actualizar llamada en execution_single_command.c:**

```c
else if (pid == 0)
{
    if (!fd_checker(shell))  // ✅ Validar retorno
    {
        exit(1);
    }
    // ... resto del código
}
```

---

## 12. CORRECCIÓN: ft_local_var.c - add_local_var()

**Reemplazar función:**

```c
static void add_local_var(char ***local_env, char *arg)
{
    int     i;
    char    **new_env;
    int     j;
    char    *dup_arg;

    i = 0;
    while ((*local_env)[i])
        i++;
    
    new_env = malloc(sizeof(char *) * (i + 2));
    if (!new_env)
        return;
    
    j = 0;
    while (j < i)
    {
        new_env[j] = (*local_env)[j];
        j++;
    }
    
    // ✅ AGREGADO: Validar ft_strdup
    dup_arg = ft_strdup(arg);
    if (!dup_arg)
    {
        free(new_env);
        return;
    }
    
    new_env[i] = dup_arg;
    new_env[i + 1] = NULL;
    free(*local_env);
    *local_env = new_env;
}
```

---

## 13. SCRIPT DE VALIDACIÓN CON VALGRIND

**Crear archivo:** `test_valgrind.sh`

```bash
#!/bin/bash

# Colores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== Compilando con flags de debug ===${NC}"
make clean > /dev/null 2>&1
make CFLAGS="-g -O0 -Wall -Wextra -Werror" 2>&1 | head -20

if [ ! -f minishell ]; then
    echo -e "${RED}Error: Compilación fallida${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Compilación exitosa${NC}\n"

# Función para ejecutar test con valgrind
test_command() {
    local desc=$1
    local cmd=$2
    local logfile=$3
    
    echo -e "${YELLOW}Testando: $desc${NC}"
    echo "$cmd" | valgrind --leak-check=full --show-leak-kinds=all \
                           --track-origins=yes --log-file="$logfile" \
                           ./minishell > /dev/null 2>&1
    
    # Analizar resultados
    if grep -q "ERROR SUMMARY: 0 errors" "$logfile"; then
        echo -e "${GREEN}✓ Sin errores de memoria${NC}"
    else
        echo -e "${RED}✗ Posibles leaks detectados${NC}"
        grep "SUMMARY\|definitely lost\|indirectly lost" "$logfile"
    fi
    echo ""
}

# Tests
test_command "exit" "exit" "valgrind_exit.log"
test_command "pwd" "pwd\nexit" "valgrind_pwd.log"
test_command "echo con -n" "echo -n test\nexit" "valgrind_echo.log"
test_command "export" "export VAR=value\nexit" "valgrind_export.log"
test_command "command not found" "invalidcommand\nexit" "valgrind_notfound.log"
test_command "redirect output" "echo test > /tmp/test.txt\nexit" "valgrind_redir.log"
test_command "pipe simple" "echo test | cat\nexit" "valgrind_pipe.log"

echo -e "${YELLOW}=== Resumen de logs ===${NC}"
echo "Archivos de log creados:"
ls -lh valgrind_*.log 2>/dev/null | awk '{print $9, "(" $5 ")"}'
```

**Uso:**
```bash
chmod +x test_valgrind.sh
./test_valgrind.sh
```

---

## 14. MAKEFILE - Agregar target para debugging

**Agregar a Makefile:**

```makefile
# Debug build
debug: CFLAGS = -g -O0 -Wall -Wextra -Werror -fsanitize=address,undefined
debug: $(NAME)

# Valgrind target
valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all \
	         --track-origins=yes ./$(NAME)

# Memory check
memcheck: $(NAME)
	./test_valgrind.sh
```

---

## RESUMEN DE CAMBIOS

| Archivo | Cambio | Tipo |
|---------|--------|------|
| execution_single_command.c | Agregar free_path_env() | Memory Leak |
| execution_piped.c | Agregar free_path_env() | Memory Leak |
| parser_tokenizer0.c | Cambiar input[0] a input[i] | Bug Lógica |
| ft_echo.c | Agregar validaciones | Buffer Overflow |
| ft_export.c | Liberar sorted_vars | Memory Leak |
| struct_utils.c | Liberar shell->env | Memory Leak |
| parser_parser_init5.c | Mejorar validaciones | Edge Case |
| main.c | Llamar copy_env() | Memory |
| execution_utils.c | Retornar int de fd_checker | Error Handling |
| Crear path_utils.c | free_path_env() | Refactor |

**Total de cambios:** 10 archivos modificados / 1 archivo nuevo

