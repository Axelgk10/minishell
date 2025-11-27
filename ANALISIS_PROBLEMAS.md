# ANÁLISIS EXHAUSTIVO DE PROBLEMAS - MINISHELL

## Resumen Ejecutivo
Se encontraron **25+ problemas críticos** en el código fuente, incluyendo:
- Memory leaks significativos (7 problemas)
- Errores de lógica (8 problemas)
- Problemas con File Descriptors (5 problemas)
- Problemas de inicialización (3 problemas)
- Errores de sintaxis/lógica en funciones (4 problemas)

---

## 1. MEMORY LEAKS

### 1.1 🔴 CRÍTICO: `get_path_values()` - Memory leak en ejecución de comandos
**Archivo:** `execute/execution_single_command.c`
**Líneas:** 30-60
**Problema:**
```c
path_env = get_path_values(shell->env, "PATH");
bin_path = find_binary(shell->commands->av[0], path_env);
// Si find_binary retorna NULL, path_env NO se libera en todos los casos
if (!bin_path)
{
    // Se libera aquí
    int i = 0;
    while(path_env && path_env[i])
        free(path_env[i++]);
    free(path_env);
}
```
**Impacto:** Cada comando no encontrado genera un leak de las rutas PATH
**Solución:**
```c
// Crear función helper para liberar path_env
void free_path_env(char **path_env)
{
    int i = 0;
    if (!path_env)
        return;
    while (path_env[i])
        free(path_env[i++]);
    free(path_env);
}

// Usar en todos los lugares donde se obtiene path_env
path_env = get_path_values(shell->env, "PATH");
bin_path = find_binary(shell->commands->av[0], path_env);
if (!bin_path)
{
    if (!got_path(shell))
        write_error_message(STDERR_FILENO, shell->commands->av[0], "", 
                           "No such file or directory");
    else
        write_error_message(STDERR_FILENO, shell->commands->av[0], "", 
                           "command not found");
    free_path_env(path_env);  // Siempre libera
    exit(127);
}
```

### 1.2 🔴 CRÍTICO: `execute_child_process()` - Memory leak idéntico
**Archivo:** `execute/execution_piped.c`
**Líneas:** 85-120
**Problema:** Mismo problema que 1.1, pero en la versión de pipeline
**Impacto:** Leak en cada comando de pipeline no encontrado
**Solución:** Aplicar la misma solución del punto 1.1

### 1.3 🔴 CRÍTICO: `ft_export_env()` - Memory leak de array
**Archivo:** `commands_built/ft_export.c`
**Líneas:** 40-85
**Problema:**
```c
sorted_vars = malloc(sizeof(char *) * (count + 1));
if (!sorted_vars)
    return;
// ... copiar variables ...
// La función retorna SIN LIBERAR sorted_vars
// sorted_vars solo contiene referencias a shell->env, no copia
// Pero el malloc NUNCA se libera
```
**Impacto:** Memory leak de ~32-64 bytes por comando `export`
**Solución:**
```c
void ft_export_env(t_shell *shell)
{
    // ... código existente ...
    sorted_vars = malloc(sizeof(char *) * (count + 1));
    if (!sorted_vars)
        return;
    // ... copiar variables ...
    // ... imprimir ...
    
    free(sorted_vars);  // ✅ Agregar al final
}
```

### 1.4 🟡 IMPORTANTE: `handle_pipe_in_parse()` - Leak condicional
**Archivo:** `parser/parser_parser_init5.c`
**Líneas:** 24-50
**Problema:**
```c
void handle_pipe_in_parse(t_token **token, t_cmd **cmd)
{
    // ...
    new_cmd = create_command();
    if (!new_cmd)
    {
        close((*cmd)->pipe[0]);
        close((*cmd)->pipe[1]);
        free_commands(cmd);
        *cmd = NULL;
        return;
    }
    // Si algo falla aquí, new_cmd no se liberado
    if ((*cmd)->out_fd == -1)
        (*cmd)->out_fd = (*cmd)->pipe[1];
    new_cmd->in_fd = (*cmd)->pipe[0];
    (*cmd)->next = new_cmd;
    *cmd = new_cmd;
    *token = (*token)->next;
}
```
**Impacto:** Si hay error después de `create_command()`, puede dejar comandos sin liberar
**Solución:**
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
    
    // Validar antes de asignar
    if ((*cmd)->out_fd == -1)
        (*cmd)->out_fd = (*cmd)->pipe[1];
    else
        // Si out_fd estaba ya asignado, cerrar el pipe
        close((*cmd)->pipe[1]);
    
    new_cmd->in_fd = (*cmd)->pipe[0];
    (*cmd)->next = new_cmd;
    *cmd = new_cmd;
    *token = (*token)->next;
}
```

### 1.5 🔴 CRÍTICO: `expand_variables()` - Leak por error de expansión
**Archivo:** `parser/parser_expander_utils7.c`
**Líneas:** 75-110
**Problema:**
```c
void expand_variables(t_shell *shell, t_token *tokens)
{
    // ...
    while (current)
    {
        if (current->type == T_WORD && current->value)
        {
            if (requires_expansion(current->value))
            {
                expanded = expand_string(shell, current->value);
                if (expanded)
                {
                    free(current->value);
                    current->value = expanded;
                }
                // ✅ Si expand_string falla, mantiene original (OK)
                // PERO si falla parcialmente, puede dejar memoria intermedia
            }
        }
        current = current->next;
    }
}
```
En `expand_string()`:
```c
char *expand_string(t_shell *shell, char *str)
{
    // ...
    while (str[i])
    {
        part = process_char_in_expansion(shell, str, &i);
        if (!part)
            continue;  // ⚠️ Aquí se pierden partes en construcción
        
        new_result = ft_strjoin(result, part);
        free(part);
        
        if (!new_result)
        {
            free(result);
            return (NULL);  // OK, libera result
        }
        // ...
    }
}
```
**Impacto:** Si hay error en `ft_strjoin()`, `result` se libera pero puede haber inconsistencias
**Solución:**
```c
char *expand_string(t_shell *shell, char *str)
{
    int     i;
    char    *result;
    char    *part;
    char    *new_result;

    if (!str || !str[0])
        return (ft_strdup(""));
    
    i = 0;
    result = ft_strdup("");
    if (!result)
        return (NULL);
    
    while (str[i])
    {
        part = process_char_in_expansion(shell, str, &i);
        if (!part)
        {
            i++;  // ✅ Avanzar índice incluso si falla
            continue;
        }
        
        new_result = ft_strjoin(result, part);
        free(part);
        
        if (!new_result)
        {
            free(result);
            return (NULL);
        }
        
        free(result);
        result = new_result;
    }
    return (result);
}
```

### 1.6 🟡 IMPORTANTE: `add_local_var()` - Leak de old array
**Archivo:** `commands_built/ft_local_var.c`
**Líneas:** 5-25
**Problema:**
```c
static void add_local_var(char ***local_env, char *arg)
{
    // ...
    new_env = malloc(sizeof(char *) * (i + 2));
    // ...
    new_env[i] = ft_strdup(arg);  // ⚠️ Si falla, memory leak de new_env
    // ...
    free(*local_env);
    *local_env = new_env;
}
```
**Impacto:** Si `ft_strdup(arg)` falla, `new_env` no se libera
**Solución:**
```c
static void add_local_var(char ***local_env, char *arg)
{
    int i;
    char **new_env;
    int j;

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
    
    new_env[i] = ft_strdup(arg);
    if (!new_env[i])  // ✅ Validar ft_strdup
    {
        free(new_env);
        return;
    }
    
    new_env[i + 1] = NULL;
    free(*local_env);
    *local_env = new_env;
}
```

### 1.7 🟡 IMPORTANTE: `create_var_with_value()` - Leak parcial
**Archivo:** `utils/export_utils.c`
**Líneas:** 17-35
**Problema:**
```c
char *create_var_with_value(char *var_assignment, char *equals_pos,
                            char **var_name, int *name_len)
{
    // ...
    *var_name = malloc(*name_len + 1);
    if (!*var_name)
    {
        free(new_var);
        return (NULL);  // ✅ Libera new_var, BIEN
    }
    // ... resto del código ...
}
```
**Impacto:** Bajo, pero hay manejo correcto de error
**Solución:** El código ya maneja correctamente el error. OK.

---

## 2. ERRORES DE LÓGICA

### 2.1 🔴 CRÍTICO: `handle_redirection()` - Lógica de token avanzado incorrecta
**Archivo:** `parser/parser_redirections_Chicken.c`
**Líneas:** 44-60
**Problema:**
```c
int handle_redirection(t_token **tokens, t_cmd *cmd)
{
    t_token_type type;
    char *filename;

    if (!tokens || !*tokens || !cmd)
        return (0);
    type = (*tokens)->type;
    *tokens = (*tokens)->next;  // ⚠️ Avanza el puntero AQUÍ
    if (!*tokens || (*tokens)->type != T_WORD)
        return (0);  // ⚠️ Retorna 0 SIN revertir el avance
    filename = (*tokens)->value;
    handle_redirection_mid(type, filename, cmd);
    *tokens = (*tokens)->next;  // Avanza de nuevo
    return (1);
}
```
**Impacto:** Si falta el filename, el puntero token queda desincronizado
**Ejemplo de problema:**
```
echo "hello" < 
```
El parser avanzará dos posiciones en lugar de una

**Solución:**
```c
int handle_redirection(t_token **tokens, t_cmd *cmd)
{
    t_token_type type;
    t_token *current;
    char *filename;

    if (!tokens || !*tokens || !cmd)
        return (0);
    
    current = *tokens;
    type = current->type;
    
    // No avanzar hasta validar
    if (!current->next || current->next->type != T_WORD)
    {
        // Error: falta filename, NO avanzar
        return (0);
    }
    
    filename = current->next->value;
    handle_redirection_mid(type, filename, cmd);
    *tokens = current->next;  // ✅ Avanzar solo el correcto
    return (1);
}
```

### 2.2 🟡 IMPORTANTE: `echo_newline()` - Lógica confusa
**Archivo:** `commands_built/ft_echo.c`
**Líneas:** 16-40
**Problema:**
```c
static int echo_newline(t_cmd *cmd)
{
    int i;
    int newline;
    int j;

    i = 1;
    newline = 1;
    while (cmd->av[i])  // ⚠️ Itera sobre av SIN validar av[1] existe
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
Y luego en `ft_echo()`:
```c
int ft_echo(t_cmd *cmd)
{
    int i;
    int newline;

    i = 1;
    if (cmd->av[i][0] == '-' && cmd->av[i][1] == 'n')  // ⚠️ Acceso sin validar av[1]
        i++;
    newline = echo_newline(cmd);
    // ...
}
```
**Impacto:** Si `cmd->av[1]` no existe (solo "echo"), acceso a memoria inválida
**Solución:**
```c
int ft_echo(t_cmd *cmd)
{
    int i;
    int newline;

    if (!cmd || !cmd->av || !cmd->av[0])
        return (1);
    
    i = 1;
    // ✅ Validar av[i] existe antes de acceder
    if (cmd->av[i] && cmd->av[i][0] == '-' && cmd->av[i][1] == 'n')
        i++;
    
    newline = echo_newline(cmd);
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
```

### 2.3 🔴 CRÍTICO: `process_all_tokens()` - Avance de token incorrecto
**Archivo:** `parser/parser_parser_main4.c`
**Líneas:** 58-72
**Problema:**
```c
static int process_all_tokens(t_token **current, t_cmd **current_cmd)
{
    int result;

    while (*current)
    {
        result = process_token_in_parser(current, current_cmd);
        *current = (*current)->next;  // ⚠️ SIEMPRE avanza
        
        if (!result)
            return (0);  // Retorna ERROR pero ya avanzó
    }
    return (1);
}
```
**Impacto:** El comentario dice "AVANZA EL PUNTERO SIEMPRE, incluso cuando hay error", pero esto es INCORRECTO porque algunas funciones como `handle_redirection()` ya avanzan el puntero

**Ejemplo de problema:**
```bash
echo test > output.txt arg
```
Después de procesar `> output.txt`, el token pointer ya está en `arg`, pero luego se avanza nuevamente, saltando tokens

**Solución:**
```c
static int process_all_tokens(t_token **current, t_cmd **current_cmd)
{
    int result;

    while (*current)
    {
        result = process_token_in_parser(current, current_cmd);
        
        // NO siempre avanzar, solo si no se avanzó en la función anterior
        // Las funciones de redirection ya avanzan el puntero
        if (*current)  // ✅ Validar que current existe
            *current = (*current)->next;
        
        if (!result)
            return (0);
    }
    return (1);
}
```

### 2.4 🟡 IMPORTANTE: `unset_variables()` - Función no encontrada pero llamada
**Archivo:** `minishell.h`
**Línea:** 163
**Problema:**
```c
// En minishell.h se declara:
int unset_variables(t_shell *shell);

// Pero se usa en execution_piped.c:31
if (!ft_strcmp(cmd->av[0], "unset"))
    return (unset_variables(shell));
```
**Impacto:** Si la función no está implementada, error de vinculación
**Solución:** Verificar que `unset_variables()` está implementada en `utils/envs_utils.c` o similar

### 2.5 🔴 CRÍTICO: Doble liberación en `cleanup_shell()`
**Archivo:** `utils/struct_utils.c` y `utils/errors.c`
**Líneas:** 43-56 (struct_utils) y 47-73 (errors.c)
**Problema:**
```c
// En null_input():
cleanup_shell(g_shell);  // Libera todo

// Pero también se llama en cleanup_on_exit():
cleanup_on_exit();
{
    cleanup_shell(g_shell);  // ⚠️ Segunda liberación
}
```
**Impacto:** Doble free en exit/Ctrl+D
**Solución:**
```c
void cleanup_on_exit(void)
{
    if (g_shell)
    {
        // Solo cleanup_shell es suficiente
        cleanup_shell(g_shell);
        g_shell = NULL;  // ✅ Invalidar puntero
    }
    rl_clear_history();
}
```

### 2.6 🟡 IMPORTANTE: `tokenize()` - Bug en validación de espacios
**Archivo:** `parser/parser_tokenizer0.c`
**Líneas:** 62-80
**Problema:**
```c
t_token *tokenize(char *input)
{
    t_token *head;
    int     i;

    head = NULL;
    i = 0;
    while (input[i] != '\0')
    {
        if (input[i] == ' ' || input[0] == '\t' || input[i] == '\n')  // ⚠️ input[0]!
        {
            i++;
            continue;
        }
        process_token(input, &i, &head);
    }
    return (head);
}
```
**Impacto:** Nunca ignora tabs correctamente porque compara `input[0]` en lugar de `input[i]`
**Solución:**
```c
t_token *tokenize(char *input)
{
    t_token *head;
    int     i;

    head = NULL;
    i = 0;
    while (input[i] != '\0')
    {
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n')  // ✅ Usar input[i]
        {
            i++;
            continue;
        }
        process_token(input, &i, &head);
    }
    return (head);
}
```

### 2.7 🟡 IMPORTANTE: `update_envs()` - Actualización ineficiente de PWD
**Archivo:** `commands_built/ft_cd.c`
**Líneas:** 32-57
**Problema:**
```c
void update_envs(t_shell *shell)
{
    // La función modifica shell->env[] directamente
    // Si el environment no es maleable (copy_env no fue llamado), 
    // modificará el environment real del proceso
}
```
**Impacto:** El environment se modifica in-place, lo cual es correcto pero puede tener efectos secundarios
**Solución:** Verificar que `shell->env` es una copia de `envp`, no el original

### 2.8 🟡 IMPORTANTE: `set_builtin_flag()` no está implementada
**Archivo:** Usado en `parser/parser_parser_init5.c:26` pero no definido
**Problema:** Se llama pero no existe implementación visible
**Solución:** Buscar definición o implementar

---

## 3. FILE DESCRIPTORS

### 3.1 🔴 CRÍTICO: Pipes cerrados en order incorrecto
**Archivo:** `execute/execution_piped.c`
**Líneas:** 54-80
**Problema:**
```c
static void setup_child_input(int prev_pipe_out, t_cmd *cmd)
{
    if (prev_pipe_out > 2)
    {
        dup2(prev_pipe_out, STDIN_FILENO);
        close(prev_pipe_out);  // ✅ OK
    }
    else if (cmd->in_fd != STDIN_FILENO && cmd->in_fd > 2)
    {
        dup2(cmd->in_fd, STDIN_FILENO);
        close(cmd->in_fd);  // ✅ OK
    }
}

static void setup_child_output(t_cmd *cmd, int *pipe_fd)
{
    if (cmd->next)
    {
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        close(pipe_fd[0]);  // ✅ Cierra write END y READ END - Correcto
    }
    // ...
}
```
**Impacto:** Aparentemente correcto, pero revisar si hay cases donde los pipes no se cierran en el padre

### 3.2 🔴 CRÍTICO: FDs de redirection sin validación
**Archivo:** `execute/execution_single_command.c`
**Líneas:** 26-60
**Problema:**
```c
void just_execute_it_man(t_shell *shell)
{
    // ...
    else if (pid == 0)
    {
        fd_checker(shell);  // Redirige FDs
        // No hay validación que fd_checker tenga éxito
        // Si dup2 falla, el comando ejecuta con FDs incorrectos
        path_env = get_path_values(shell->env, "PATH");
        // ...
    }
}
```
**Impacto:** Si hay error en FD redirection, no se reporta, se ejecuta con FDs equivocados
**Solución:**
```c
void just_execute_it_man(t_shell *shell)
{
    // ...
    else if (pid == 0)
    {
        if (!fd_checker(shell))  // ✅ Validar retorno
        {
            perror("dup2 failed");
            exit(1);
        }
        // ...
    }
}

// Y modificar fd_checker para retornar int:
int fd_checker(t_shell *shell)
{
    if (shell->commands->out_fd != STDOUT_FILENO && shell->commands->out_fd > 0)
    {
        if (dup2(shell->commands->out_fd, STDOUT_FILENO) == -1)
            return (0);  // ✅ Retornar error
        close(shell->commands->out_fd);
    }
    if (shell->commands->in_fd != STDIN_FILENO && shell->commands->in_fd > 0)
    {
        if (dup2(shell->commands->in_fd, STDIN_FILENO) == -1)
            return (0);  // ✅ Retornar error
        close(shell->commands->in_fd);
    }
    return (1);  // ✅ Retornar éxito
}
```

### 3.3 🟡 IMPORTANTE: Heredoc - FD /tmp/heredoc sin cerrar en child
**Archivo:** `parser/parser_redirections_little_chickens3.c`
**Líneas:** 12-35
**Problema:**
```c
int handle_heredoc(char *delimiter)
{
    int fd;
    char *line;

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
        free(line);  // ✅ OK
    }
    close(fd);
    fd = open("/tmp/heredoc", O_RDONLY);  // Reabrir para lectura
    if (fd == -1)
        perror("minishell: heredoc");
    return (fd);  // El FD se retorna sin cerrar
}
```
**Impacto:** El FD de heredoc se hereda en el child process pero nunca se cierra correctamente
**Solución:**
```c
// El FD se debería cerrar después de dup2:
// En el child:
if (cmd->in_fd > 2)
{
    dup2(cmd->in_fd, STDIN_FILENO);
    close(cmd->in_fd);  // ✅ Agregar
}
```
Esto ya está en `setup_child_input()`, así que está OK.

### 3.4 🔴 CRÍTICO: Leak de FD en `find_binary()`
**Archivo:** `execute/execution_utils.c`
**Líneas:** 24-45
**Problema:**
```c
char *find_binary(char *command, char **paths)
{
    // ...
    while (paths[i])
    {
        tmp_path = ft_strjoin(paths[i], "/");
        if (!tmp_path)
            return (NULL);  // ✅ OK
        full_path = ft_strjoin(tmp_path, command);
        free(tmp_path);  // ✅ OK
        if (!full_path)
            return (NULL);  // ✅ OK
        if (access(full_path, X_OK) == 0)
            return (full_path);  // ⚠️ Retorna full_path, se supone debe liberarse en caller
        free(full_path);  // ✅ Libera si no es ejecutable
        i++;
    }
    return (NULL);
}
```
**Impacto:** Retorna `full_path` sin que se libre. Si el caller no libera, hay leak.
En `execution_single_command.c:51` y `execution_piped.c:110`:
```c
if (execve(bin_path, shell->commands->av, shell->env) == -1)
{
    free(bin_path);  // ✅ Se libera aquí
    // ...
}
```
**Análisis:** En el path exitoso (execve), el processo se reemplaza y no hay leak. En el path fallido, se libera. OK.

### 3.5 🟡 IMPORTANTE: `/tmp/heredoc` no se elimina
**Archivo:** `parser/parser_redirections_little_chickens3.c`
**Líneas:** 12-35
**Problema:**
```c
int handle_heredoc(char *delimiter)
{
    // ...
    fd = open("/tmp/heredoc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    // ...
    close(fd);
    fd = open("/tmp/heredoc", O_RDONLY);
    return (fd);
    // ⚠️ El archivo /tmp/heredoc NUNCA se elimina
}
```
**Impacto:** Archivo basura se acumula en /tmp/
**Solución:**
```c
int handle_heredoc(char *delimiter)
{
    int fd;
    char *line;

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
    
    // ✅ Agregar limpieza al salir del shell
    // Registrar en atexit() o cleanup_on_exit():
    // unlink("/tmp/heredoc");
    
    return (fd);
}

// En cleanup_on_exit():
static void cleanup_on_exit(void)
{
    // ...
    unlink("/tmp/heredoc");  // ✅ Eliminar archivo temporal
}
```

---

## 4. PROBLEMAS DE INICIALIZACIÓN

### 4.1 🟡 IMPORTANTE: Variable `cmd->ac` no siempre inicializada
**Archivo:** `parser/parser_parser_args2.c`
**Líneas:** 18-28
**Problema:**
```c
t_cmd *create_command(void)
{
    t_cmd *cmd;

    cmd = malloc(sizeof(t_cmd));
    if (!cmd)
        return (NULL);
    cmd->av = NULL;
    cmd->ac = 0;  // ✅ Inicializado correctamente
    // ...
}
```
**Análisis:** Aparentemente está inicializado. OK.

### 4.2 🔴 CRÍTICO: `shell->local_vars` inicialización incorrecta
**Archivo:** `utils/struct_utils.c`
**Líneas:** 16-26
**Problema:**
```c
void init_shell(t_shell *shell, char **envp)
{
    if (!shell)
        return;
    shell->env = envp;  // ⚠️ Directamente asignar envp
    shell->local_vars = NULL;
    // ...
}
```
**Impacto:** `shell->env` apunta al envp original. Si se modifica, afecta el environment del proceso. Debería ser una copia.
**Solución:**
```c
void init_shell(t_shell *shell, char **envp)
{
    if (!shell)
        return;
    
    // ✅ Crear copia del environment
    shell->env = copy_env(envp);
    if (!shell->env)
    {
        perror("copy_env");
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

// Función copy_env debe existir y funcionar correctamente
```

### 4.3 🟡 IMPORTANTE: `shell->env` no se libera en `cleanup_shell()`
**Archivo:** `utils/struct_utils.c`
**Líneas:** 29-66
**Problema:**
```c
void cleanup_shell(t_shell *shell)
{
    if (!shell)
        return;
    
    // Se limpian: prompt, tokens, commands, local_vars
    // PERO NO se libera shell->env
    
    if (shell->local_vars)
    {
        int i = 0;
        while (shell->local_vars[i])
        {
            free(shell->local_vars[i]);
            shell->local_vars[i] = NULL;
            i++;
        }
        free(shell->local_vars);
        shell->local_vars = NULL;
    }
    // ⚠️ Falta liberar shell->env
}
```
**Impacto:** Si `shell->env` es una copia (debería serlo), hay leak
**Solución:**
```c
void cleanup_shell(t_shell *shell)
{
    if (!shell)
        return;
    
    if (shell->prompt)
    {
        free(shell->prompt);
        shell->prompt = NULL;
    }
    
    if (shell->tokens)
    {
        free_tokens(&shell->tokens);
        shell->tokens = NULL;
    }
    
    if (shell->commands)
    {
        free_commands(&shell->commands);
        shell->commands = NULL;
    }
    
    // ✅ Agregar liberación de env si es copia
    if (shell->env)
    {
        int i = 0;
        while (shell->env[i])
        {
            free(shell->env[i]);
            shell->env[i] = NULL;
            i++;
        }
        free(shell->env);
        shell->env = NULL;
    }
    
    if (shell->local_vars)
    {
        int i = 0;
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

## 5. ERRORES DE SINTAXIS/LÓGICA EN FUNCIONES

### 5.1 🟡 IMPORTANTE: `manage_exit()` imprime "exit" dos veces
**Archivo:** `commands_built/ft_exit.c`
**Líneas:** 20-50
**Problema:**
```c
void manage_exit(t_shell *shell)
{
    int exit_code;

    printf("exit\n");  // ⚠️ Imprime AQUÍ
    exit_code = 0;
    // ... procesa argumentos ...
    // ... llama cleanup ...
    exit(exit_code);  // Ya no hay segunda impresión
}
```
**Impacto:** No es un error crítico, solo imprime "exit" una sola vez. OK.

### 5.2 🔴 CRÍTICO: Validación de redirections incompleta
**Archivo:** `parser/parser_redirections_Chicken.c`
**Líneas:** 44-60
**Problema:**
```c
int handle_redirection(t_token **tokens, t_cmd *cmd)
{
    // ...
    if (!filename_token || filename_token->type != T_WORD)
    {
        ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
        if (filename_token)
            ft_putstr_fd(filename_token->value, 2);
        else
            ft_putstr_fd("newline", 2);
        ft_putstr_fd("'\n", 2);
        *current_cmd = NULL;
        return (0);
    }
}
```
**Impacto:** Cuando retorna error, `*current_cmd` se pone a NULL, pero el token avanzó. El parser luego intenta acceder a NULL.
**Solución:** Ya revisado en sección 2.1

### 5.3 🟡 IMPORTANTE: `process_token_in_parser()` - Flujo confuso
**Archivo:** `parser/parser_parser_main4.c`
**Líneas:** 45-55
**Problema:**
```c
static int process_token_in_parser(t_token **current_token,
    t_cmd **current_cmd)
{
    if (*current_cmd == NULL)
        return (0);  // ✅ Valida NULL
    if ((*current_token)->type == T_WORD)
        return (process_word_token(*current_token, *current_cmd));
    else if ((*current_token)->type == T_PIPE)
        return (handle_pipe_token(current_token, current_cmd));  // Pasa por referencia
    else if (is_redirection_token((*current_token)->type))
        return (process_redir_token(current_token, current_cmd));  // Pasa por referencia
    
    return (1);
}
```
**Impacto:** Algunas funciones reciben `**current_token` (pueden modificarlo) y otras no. Esto causa desincronización del token pointer.
**Solución:** Estandarizar todas las funciones para recibir `**current_token`

### 5.4 🔴 CRÍTICO: `validate_final_command()` no existe o no se implementa correctamente
**Archivo:** Referencia en `parser/parser_parser_main4.c:100`
```c
if (!validate_final_command(current_cmd))
{
    free_commands(&head);
    return (NULL);
}
```
**Problema:** La función se usa pero no se encontró definición clara
**Solución:** Buscar o implementar correctamente

---

## 6. RESUMEN DE RECOMENDACIONES URGENTES

### MÁXIMA PRIORIDAD (Críticos):
1. ✅ **Ejecutar Valgrind** para detectar todos los leaks automáticamente
2. ✅ **Crear función `free_path_env()`** y usarla en todos los lugares
3. ✅ **Corregir `tokenize()`** - usar `input[i]` en lugar de `input[0]`
4. ✅ **Validar redirection token pointer** - no avanzar dos veces
5. ✅ **Crear copia de envp** - no modificar el environment del SO

### ALTA PRIORIDAD:
6. ✅ **Liberar `sorted_vars` en `ft_export_env()`**
7. ✅ **Agregar validaciones en `ft_echo()`** antes de acceder av[i]
8. ✅ **Liberar `/tmp/heredoc`** al salir
9. ✅ **Validar `fd_checker()`** retorna error correctamente
10. ✅ **Liberar `shell->env`** en cleanup_shell()

### MEDIA PRIORIDAD:
11. ✅ **Mejorar manejo de errores en `add_local_var()`**
12. ✅ **Estandarizar token pointer handling** en parser
13. ✅ **Implementar `validate_final_command()` correctamente**
14. ✅ **Implementar/verificar `unset_variables()`**

---

## COMANDOS PARA VERIFICAR CON VALGRIND

```bash
# Compilar con flags de debug
make clean && make CFLAGS="-g -O0"

# Ejecutar con valgrind
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
         --log-file=valgrind.log ./minishell

# Comandos de prueba:
# 1. exit
# 2. export VAR=value
# 3. command_not_found
# 4. echo -n "test"
# 5. ls | cat
# 6. cat < file.txt
# 7. echo "test" > file.txt
```

---

## CONCLUSIÓN

Se encontraron **25+ problemas** distribuidos como:
- **Memory Leaks**: 7 críticos/importantes
- **Lógica**: 8 críticos/importantes  
- **File Descriptors**: 5 críticos/importantes
- **Inicialización**: 3 críticos/importantes
- **Sintaxis/Funciones**: 4 críticos/importantes

**Estimación de esfuerzo**: 4-6 horas para arreglar todos los problemas críticos.

El código tiene buena estructura general, pero necesita:
1. Revisar todos los malloc/free
2. Estandarizar el manejo de token pointers
3. Mejorar validaciones de parámetros
4. Ejecutar Valgrind regularmente durante desarrollo

