# PLAN DE ACCIÓN DETALLADO - IMPLEMENTACIÓN COMPLETA

**Objetivo:** Arreglar todos los problemas identificados en orden de prioridad
**Tiempo total estimado:** 3-4 horas
**Estado:** Listo para comenzar

---

## 📋 FASES DE IMPLEMENTACIÓN

### FASE 1: PREPARACIÓN (10 minutos)

#### 1.1 Crear backup
```bash
cd /home/axgimene/gg
git status  # Verificar estado
git add -A
git commit -m "backup: antes de fixes de memory leaks"
```

#### 1.2 Verificar compilación base
```bash
make clean
make 2>&1 | head -20
# Debe compilar sin errores
```

#### 1.3 Baseline de memory leaks
```bash
make debug
valgrind --leak-check=full --quiet ./minishell <<< "exit" 2>&1 | grep -E "(definitely|indirectly)"
# Anotar el número de leaks actuales
```

---

### FASE 2: FIXES CRÍTICOS (20 minutos)

#### 2.1 Eliminar doble free en cleanup - `main.c`
**Tiempo:** 5 min | **Archivos:** main.c, utils/errors.c | **Riesgo:** BAJO

**Pasos:**
1. Abrir `main.c`
2. Encontrar función `cleanup_on_exit()` (línea ~7)
3. Reemplazar TODO el código interno con una sola línea: `cleanup_shell(g_shell);`
4. Abrir `utils/errors.c`
5. Encontrar función `null_input()` (línea ~45)
6. Reemplazar bloque de cleanup con: `cleanup_shell(g_shell);`

**Verificación:**
```bash
make clean && make
# Debe compilar sin errores
```

---

#### 2.2 Liberar prompt anterior en loop - `main.c`
**Tiempo:** 3 min | **Archivo:** main.c línea 147 | **Riesgo:** BAJO

**Pasos:**
1. En el loop `while(1)` en `main()`
2. ANTES de `if (getcwd(...))`, agregar:
```c
if (shell.prompt)
{
    free(shell.prompt);
    shell.prompt = NULL;
}
```

**Verificación:**
```bash
make clean && make
# Ejecutar: ./minishell
# Escribir varios comandos
# Verificar que los prompts se cambien correctamente
```

---

#### 2.3 Limpiar después de cada input - `main.c`
**Tiempo:** 2 min | **Archivo:** main.c línea 130 | **Riesgo:** BAJO

Verificar que `process_input()` tenga:
```c
// ANTES de procesar
free_shell_after_execution(shell);

// DESPUÉS de procesar
free_shell_after_execution(shell);
free(input);
```

**Verificación:**
```bash
make clean && make asan
ASAN_OPTIONS=detect_leaks=1 timeout 3 ./minishell <<< "echo test"
# Debe no reportar leaks nuevos
```

---

### FASE 3: MEMORY LEAKS MAYORES (35 minutos)

#### 3.1 Liberar path_env en parent - `execute/execution_single_command.c`
**Tiempo:** 10 min | **Archivos:** execution_single_command.c, execution_utils.c, minishell.h | **Riesgo:** MEDIO

**Pasos:**

A) Crear función helper en `execution_utils.c`:
```c
void	free_path_env(char **path_env)
{
    int	i;

    if (!path_env)
        return ;
    i = 0;
    while (path_env[i])
    {
        free(path_env[i]);
        i++;
    }
    free(path_env);
}
```

B) Declarar en `minishell.h`:
```c
void	free_path_env(char **path_env);
```

C) Modificar `just_execute_it_man()` en `execution_single_command.c`:
```c
void	just_execute_it_man(t_shell *shell)
{
    char	**path_env;
    char	*bin_path;
    int		status;
    pid_t	pid;

    status = 0;
    path_env = get_path_values(shell->env, "PATH");  // Guardar AQUÍ
    
    pid = fork();
    if (pid == -1)
    {
        free_path_env(path_env);  // Si fork falla
        exit(1);
    }
    else if (pid == 0)  // HIJO
    {
        fd_checker(shell);
        bin_path = find_binary(shell->commands->av[0], path_env);
        if (!bin_path)
        {
            if (!got_path(shell))
                write_error_message(STDERR_FILENO, shell->commands->av[0], "", "No such file or directory");
            else
                write_error_message(STDERR_FILENO, shell->commands->av[0], "", "command not found");
            free_path_env(path_env);  // Liberar en hijo
            exit(127);
        }
        if (execve(bin_path, shell->commands->av, shell->env) == -1)
        {
            free(bin_path);
            free_path_env(path_env);  // Liberar en hijo
            error_executing(2, shell->env, shell->commands->av);
        }
    }
    else if (pid > 0)  // PADRE
    {
        free_path_env(path_env);  // ✨ NUEVO: Liberar en padre
        status_wait(pid, status);
    }
}
```

**Verificación:**
```bash
make clean && make asan
ASAN_OPTIONS=detect_leaks=1 timeout 3 ./minishell <<< "invalidcommand"
# Antes: Reporte de leaks
# Después: Sin leaks nuevos
```

---

#### 3.2 Copiar environment - `utils/struct_utils.c`
**Tiempo:** 15 min | **Archivo:** struct_utils.c | **Riesgo:** MEDIO

**Pasos:**

A) Reemplazar `init_shell()` completamente:
```c
void	init_shell(t_shell *shell, char **envp)
{
    int		env_count;
    int		i;

    if (!shell)
        return;
    
    // Copiar variables de entorno
    env_count = 0;
    while (envp && envp[env_count])
        env_count++;
    
    shell->env = malloc((env_count + 1) * sizeof(char *));
    if (!shell->env)
    {
        ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
        exit(1);
    }
    
    i = 0;
    while (i < env_count)
    {
        shell->env[i] = ft_strdup(envp[i]);
        if (!shell->env[i])
        {
            ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
            exit(1);
        }
        i++;
    }
    shell->env[env_count] = NULL;
    
    // Inicializar resto
    shell->local_vars = NULL;
    shell->tokens = NULL;
    shell->commands = NULL;
    shell->prompt = NULL;
    shell->exit_status = 0;
    shell->stdin_copy = STDIN_FILENO;
    shell->stdout_copy = STDOUT_FILENO;
}
```

B) Actualizar `cleanup_shell()` para liberar env:
```c
void	cleanup_shell(t_shell *shell)
{
    int i;
    
    if (!shell)
        return;
    
    // Liberar environment
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
    
    // Liberar prompt
    if (shell->prompt)
    {
        free(shell->prompt);
        shell->prompt = NULL;
    }
    
    // Liberar tokens
    if (shell->tokens)
    {
        free_tokens(&shell->tokens);
        shell->tokens = NULL;
    }
    
    // Liberar commands
    if (shell->commands)
    {
        free_commands(&shell->commands);
        shell->commands = NULL;
    }
    
    // Liberar local_vars
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

**Verificación:**
```bash
make clean && make
# En minishell: export TEST=123
# En minishell: echo $TEST → debe mostrar 123
# Salir y verificar que el SO no tiene la variable TEST
env | grep TEST  # Debe estar vacío
```

---

#### 3.3 Consolidar cleanup - `utils/struct_utils.c`
**Tiempo:** 10 min | **Archivo:** struct_utils.c | **Riesgo:** BAJO

Verificar que `cleanup_shell()` sea idempotente (se puede llamar múltiples veces):
- [ ] Todos los pointers se ponen a NULL después de free
- [ ] Todas las funciones revisan `if (!shell)` al inicio
- [ ] No hay código duplicado en `cleanup_on_exit()` y `null_input()`

**Verificación:**
```bash
# El código debe verse limpio sin duplicación
grep -n "cleanup_shell" main.c utils/errors.c
# Solo debe haber 2 llamadas (atexit y en null_input)
```

---

### FASE 4: FILE DESCRIPTORS (20 minutos)

#### 4.1 Restaurar FDs después de redirecciones - `execute/execution_single_command.c` o utils
**Tiempo:** 10 min | **Riesgo:** MEDIO

**Pasos:**

Después de cada comando, restaurar STDIN/STDOUT:

```c
// Función helper
void	restore_fds(t_shell *shell)
{
    if (shell->stdin_copy != STDIN_FILENO && shell->stdin_copy > 2)
    {
        dup2(shell->stdin_copy, STDIN_FILENO);
        close(shell->stdin_copy);
        shell->stdin_copy = STDIN_FILENO;
    }
    
    if (shell->stdout_copy != STDOUT_FILENO && shell->stdout_copy > 2)
    {
        dup2(shell->stdout_copy, STDOUT_FILENO);
        close(shell->stdout_copy);
        shell->stdout_copy = STDOUT_FILENO;
    }
}
```

Llamar en `main()` después de procesar cada input:
```c
process_input(&shell, input);
restore_fds(&shell);  // ← NUEVA LÍNEA
```

**Verificación:**
```bash
./minishell
# Dentro del shell:
# echo test > file.txt
# cat < file.txt
# Las redirecciones deben funcionar correctamente
```

---

#### 4.2 Cerrar pipes en parent - `execute/execution_piped.c`
**Tiempo:** 10 min | **Riesgo:** MEDIO

**Pasos:**

En `execute_pipeline()`, después de fork en el padre:

```c
// Cerrar FDs en padre que se usan en hijo
while (current)
{
    if (current->pipe[0] > 2)
    {
        close(current->pipe[0]);
        current->pipe[0] = -1;
    }
    if (current->pipe[1] > 2)
    {
        close(current->pipe[1]);
        current->pipe[1] = -1;
    }
    current = current->next;
}
```

**Verificación:**
```bash
./minishell
# echo hello | cat | cat
# echo "test\nmore" | wc -l
# Los pipes deben funcionar sin problemas
```

---

### FASE 5: VALIDACIONES (15 minutos)

#### 5.1 Validar av no sea NULL
**Tiempo:** 5 min | **Riesgo:** BAJO

Agregar validaciones en todas las funciones que acceden a `av`:

```c
// Patrón:
if (!shell->commands->av || !shell->commands->av[0])
    return;
```

Buscar en:
- [ ] `execute_builtin()` - ya tiene
- [ ] `execute_command()` - ya tiene
- [ ] `manage_exit()` - verificar
- [ ] `change_directory()` - verificar

**Verificación:**
```bash
make clean && make
# Debe compilar sin warnings
```

---

#### 5.2 Verificar exit status
**Tiempo:** 10 min | **Riesgo:** BAJO

Buscar `status_wait()` e implementar correctamente:

```c
void	status_wait(pid_t pid, int status)
{
    int	ret;

    ret = waitpid(pid, &status, 0);
    if (ret == -1)
    {
        perror("waitpid");
        return ;
    }
    
    if (WIFEXITED(status))
        g_shell->exit_status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        g_shell->exit_status = 128 + WTERMSIG(status);
}
```

**Verificación:**
```bash
./minishell
# exit 42
# echo $?  → debe mostrar 42
```

---

### FASE 6: TESTING CON MEMORY TOOLS (Variable)

#### 6.1 Compilar con AddressSanitizer
```bash
make asan
ASAN_OPTIONS=detect_leaks=1:abort_on_error=0 ./minishell <<< "echo test; exit"
# Debe reportar ZERO leaks o memoria "still reachable"
```

#### 6.2 Compilar con Valgrind
```bash
make debug
valgrind --leak-check=full \
  --show-leak-kinds=all \
  --suppressions=readline_leaks.supp \
  --quiet ./minishell <<< "exit"
# Debe reportar "definitely lost: 0 bytes"
```

#### 6.3 Comandos de test
```bash
./minishell << 'EOF'
echo "Test 1: Basic echo"
cd /tmp
pwd
export TEST_VAR=hello
echo $TEST_VAR
echo "Test 2: Pipes" | cat | cat
exit 0
EOF
```

#### 6.4 Test interactivo
```bash
./minishell
# Escribir varios comandos
# exit
# Verificar que no hay crashes
```

---

## 📊 TABLA DE SEGUIMIENTO

| Fase | Tarea | Tiempo | Estado | Verificación |
|------|-------|--------|--------|-------------|
| 1 | Preparación | 10 min | ⬜ | `git status` limpio |
| 2.1 | Eliminar doble free | 5 min | ⬜ | Compila sin errores |
| 2.2 | Liberar prompt | 3 min | ⬜ | Prompts correctos |
| 2.3 | Limpiar input | 2 min | ⬜ | ASan sin leaks |
| 3.1 | path_env leak | 10 min | ⬜ | ASan sin leaks |
| 3.2 | Copiar env | 15 min | ⬜ | SO no se modifica |
| 3.3 | Consolidar cleanup | 10 min | ⬜ | Código limpio |
| 4.1 | Restaurar FDs | 10 min | ⬜ | Redirecciones OK |
| 4.2 | Cerrar pipes | 10 min | ⬜ | Pipes OK |
| 5.1 | Validar av | 5 min | ⬜ | Sin warnings |
| 5.2 | Exit status | 10 min | ⬜ | `echo $?` correcto |
| 6 | Testing Memory | Variable | ⬜ | Valgrind clean |
| **TOTAL** | | **90 min** | | |

---

## 🎯 CRITERIOS DE ACEPTACIÓN

### ✅ Debe cumplir TODO esto:

1. **Compilación:**
   - [ ] `make clean && make` sin errores
   - [ ] `make clean && make asan` sin errores
   - [ ] `make clean && make debug` sin errores

2. **Memory Leaks:**
   - [ ] `valgrind --leak-check=full` reporta "definitely lost: 0 bytes"
   - [ ] ASan reporta cero leaks (except readline)
   - [ ] `./minishell <<< "exit"` limpio

3. **Funcionalidad:**
   - [ ] `echo test` funciona
   - [ ] `cd ~` funciona
   - [ ] `export TEST=1` funciona
   - [ ] `|` (pipes) funcionan
   - [ ] `>` y `<` (redirecciones) funcionan
   - [ ] `exit` limpio sin crashes
   - [ ] Ctrl+D limpio sin crashes

4. **Estabilidad:**
   - [ ] No hay crashes en comandos normales
   - [ ] No hay doble free
   - [ ] No hay acceso a memoria liberada

---

## 📝 COMANDOS RÁPIDOS PARA COPIAR

```bash
# Verificar cambios
git diff

# Compilar todas las versiones
make clean && make && make asan && make debug

# Test rápido
./minishell <<< "echo OK; exit"

# Test con ASan
make asan
ASAN_OPTIONS=detect_leaks=1 timeout 3 ./minishell <<< "echo test; exit"

# Test con Valgrind
make debug
valgrind --leak-check=full --quiet ./minishell <<< "exit"

# Commit
git add -A
git commit -m "fix: [descripción corta del fix]"

# Ver leaks específicos
valgrind --leak-check=full --show-leak-kinds=all ./minishell <<< "exit" 2>&1 | grep "definitely"
```

---

## 🚨 CHECKLIST ANTES DE TERMINAR

- [ ] Todos los fixes implementados
- [ ] Código compila sin errores
- [ ] Valgrind reporta cero leaks definitivos
- [ ] Todos los comandos básicos funcionan
- [ ] No hay crashes
- [ ] Git commits creados para cada fase
- [ ] Documento actualizado con resultados

---

**Documento generado automáticamente el 27 de Noviembre 2025**
**Tiempo estimado total: 90-120 minutos de trabajo concentrado**
