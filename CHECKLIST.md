# CHECKLIST DE PROBLEMAS ENCONTRADOS

## 📋 CHECKLIST DE VERIFICACIÓN Y CORRECCIÓN

### SECTION 1: MEMORY LEAKS (7 problemas)

- [ ] **1.1 - CRÍTICO:** `get_path_values()` leak en execution_single_command.c:30-60
  - Síntoma: "command not found" genera leak de PATH array
  - Solución: Implementar `free_path_env()` helper
  - Archivo: `utils/path_utils.c` (crear nuevo)
  - Esfuerzo: 15 minutos

- [ ] **1.2 - CRÍTICO:** `execute_child_process()` leak idéntico
  - Ubicación: execution_piped.c:85-120
  - Síntoma: Pipes con comando no encontrado generan leak
  - Solución: Usar `free_path_env()` helper
  - Esfuerzo: 5 minutos

- [ ] **1.3 - CRÍTICO:** `ft_export_env()` memory leak de array
  - Ubicación: commands_built/ft_export.c:40-85
  - Síntoma: `export` sin argumentos no libera sorted_vars
  - Solución: Agregar `free(sorted_vars);` antes del return
  - Esfuerzo: 2 minutos

- [ ] **1.4 - IMPORTANTE:** `handle_pipe_in_parse()` leak condicional
  - Ubicación: parser/parser_parser_init5.c:24-50
  - Síntoma: Si create_command() falla después de pipe(), new_cmd puede quedar suelto
  - Solución: Mejorar validaciones y cleanup
  - Esfuerzo: 10 minutos

- [ ] **1.5 - CRÍTICO:** `expand_variables()` error en expand_string()
  - Ubicación: parser/parser_expander_utils7.c:75-110
  - Síntoma: Si process_char_in_expansion() retorna NULL, se pierde tracking
  - Solución: Avanzar índice siempre, validar mejor
  - Esfuerzo: 10 minutos

- [ ] **1.6 - IMPORTANTE:** `add_local_var()` leak si ft_strdup falla
  - Ubicación: commands_built/ft_local_var.c:5-25
  - Síntoma: Si ft_strdup(arg) falla, new_env no se libera
  - Solución: Validar ft_strdup y liberar new_env en error
  - Esfuerzo: 5 minutos

- [ ] **1.7 - IMPORTANTE:** `cleanup_shell()` no libera shell->env
  - Ubicación: utils/struct_utils.c:29-66
  - Síntoma: Si shell->env es copia de envp, hay leak
  - Solución: Liberar shell->env en cleanup
  - Esfuerzo: 10 minutos

### SECTION 2: ERRORES DE LÓGICA (8 problemas)

- [ ] **2.1 - CRÍTICO:** `handle_redirection()` desincronización de token pointer
  - Ubicación: parser/parser_redirections_Chicken.c:44-60
  - Síntoma: Token pointer se avanza antes de validar filename
  - Problema: Si filename falta, token avanza 1 en lugar de 2
  - Solución: No avanzar hasta validar filename
  - Esfuerzo: 15 minutos

- [ ] **2.2 - IMPORTANTE:** `echo_newline()` y `ft_echo()` acceso sin validación
  - Ubicación: commands_built/ft_echo.c:16-60
  - Síntoma: Si `cmd->av[1]` no existe, acceso a memoria inválida
  - Solución: Validar `cmd->av[i]` antes de acceder av[i][0]
  - Esfuerzo: 10 minutos

- [ ] **2.3 - CRÍTICO:** `process_all_tokens()` avance duplicado
  - Ubicación: parser/parser_parser_main4.c:58-72
  - Síntoma: El comentario dice avanza siempre, pero `handle_redirection()` ya avanza
  - Problema: Saltar tokens en input como "echo test > file.txt arg"
  - Solución: Estandarizar si las funciones avanzan o no
  - Esfuerzo: 20 minutos

- [ ] **2.4 - IMPORTANTE:** `unset_variables()` función no encontrada
  - Ubicación: minishell.h:163 y execution_piped.c:31
  - Síntoma: Llamada a función no implementada
  - Solución: Verificar que existe en utils/envs_utils.c o implementar
  - Esfuerzo: 10 minutos

- [ ] **2.5 - CRÍTICO:** Doble liberación en cleanup
  - Ubicación: main.c (cleanup_on_exit) y errors.c (null_input)
  - Síntoma: Ctrl+D o exit provoca double free
  - Problema: `cleanup_shell()` se llama 2 veces
  - Solución: Solo llamar cleanup_shell() una vez
  - Esfuerzo: 5 minutos

- [ ] **2.6 - IMPORTANTE:** `tokenize()` bug input[0] en lugar de input[i]
  - Ubicación: parser/parser_tokenizer0.c:69
  - Síntoma: Tabs no se ignoran correctamente
  - Problema: `if (input[i] == ' ' || input[0] == '\t' || input[i] == '\n')`
  - Solución: Cambiar `input[0]` a `input[i]`
  - Esfuerzo: 1 minuto

- [ ] **2.7 - IMPORTANTE:** `update_envs()` modificación de environment
  - Ubicación: commands_built/ft_cd.c:32-57
  - Síntoma: Environment se modifica in-place
  - Solución: Verificar que shell->env es copia de envp
  - Esfuerzo: 5 minutos (verificación)

- [ ] **2.8 - IMPORTANTE:** `set_builtin_flag()` no implementada
  - Ubicación: Usada en parser/parser_parser_init5.c:26
  - Síntoma: Función se llama pero no existe definición
  - Solución: Buscar definición o implementar
  - Esfuerzo: 15 minutos

### SECTION 3: FILE DESCRIPTORS (5 problemas)

- [ ] **3.1 - CRÍTICO:** Validar FD checker retorna error
  - Ubicación: execute/execution_utils.c:1-15
  - Síntoma: Si dup2 falla, no se reporta
  - Solución: Cambiar fd_checker a int y validar retorno
  - Esfuerzo: 10 minutos

- [ ] **3.2 - IMPORTANTE:** /tmp/heredoc no se elimina
  - Ubicación: parser/parser_redirections_little_chickens3.c:12-35
  - Síntoma: Archivo basura se acumula en /tmp/
  - Solución: Agregar `unlink("/tmp/heredoc")` en cleanup
  - Esfuerzo: 5 minutos

- [ ] **3.3 - IMPORTANTE:** Validación de redirection con FDs
  - Ubicación: execute/execution_single_command.c:26-60
  - Síntoma: Si FD redirection falla, comando ejecuta con FDs incorrectos
  - Solución: Validar fd_checker() retorna éxito
  - Esfuerzo: 5 minutos

- [ ] **3.4 - CRÍTICO:** FD en find_binary() (ya está OK)
  - Ubicación: execute/execution_utils.c:24-45
  - Análisis: find_binary() retorna full_path, pero se libera en caller
  - Estado: ✅ CORRECTO - No necesita cambios

- [ ] **3.5 - IMPORTANTE:** Close pipes en orden correcto
  - Ubicación: execute/execution_piped.c:54-80
  - Análisis: Los pipes se cierran en el orden correcto
  - Estado: ✅ CORRECTO - No necesita cambios

### SECTION 4: INICIALIZACIÓN (3 problemas)

- [ ] **4.1 - CRÍTICO:** `shell->env` debe ser copia de envp
  - Ubicación: utils/struct_utils.c:16-26
  - Síntoma: Si no es copia, modification affect SO environment
  - Solución: Llamar `copy_env()` en init_shell()
  - Esfuerzo: 10 minutos

- [ ] **4.2 - IMPORTANTE:** `cleanup_shell()` no libera env
  - Ubicación: utils/struct_utils.c:29-66
  - Síntoma: Si env es copia, hay leak
  - Solución: Agregar liberación de env array
  - Esfuerzo: 10 minutos

- [ ] **4.3 - IMPORTANTE:** Variables inicializadas en create_command()
  - Ubicación: parser/parser_parser_args2.c:18-28
  - Análisis: `cmd->ac = 0` está inicializado correctamente
  - Estado: ✅ CORRECTO - No necesita cambios

### SECTION 5: SINTAXIS/FUNCIONES (4 problemas)

- [ ] **5.1 - IMPORTANTE:** `validate_final_command()` no implementada
  - Ubicación: Referenciada en parser/parser_parser_main4.c:100
  - Síntoma: Función se usa pero no se encontró definición
  - Solución: Buscar o implementar correctamente
  - Esfuerzo: 15 minutos

- [ ] **5.2 - IMPORTANTE:** `process_token_in_parser()` inconsistencia
  - Ubicación: parser/parser_parser_main4.c:45-55
  - Síntoma: Algunas funciones avanzan puntero, otras no
  - Solución: Estandarizar comportamiento
  - Esfuerzo: 20 minutos

- [ ] **5.3 - CRÍTICO:** Token pointer handling en redirection
  - Ubicación: parser/parser_redirections_Chicken.c:44-60
  - Síntoma: Ya revisado en sección 2.1
  - Estado: Incluido en checklist

- [ ] **5.4 - IMPORTANTE:** Copy_env() debe ser una verdadera copia
  - Ubicación: utils/main_utils.c o similar
  - Síntoma: Si no copia, environment se modifica
  - Solución: Implementar copy_env() correctamente
  - Esfuerzo: 15 minutos

---

## 📊 RESUMEN POR PRIORIDAD

### 🔴 CRÍTICOS (Máxima prioridad) - 7 problemas
- 1.1, 1.2, 1.3, 1.5 (Memory Leaks)
- 2.1, 2.3, 2.5 (Lógica)
- 3.1 (FD)
- Esfuerzo total: ~60 minutos

### 🟡 IMPORTANTES (Alta prioridad) - 14 problemas
- 1.4, 1.6, 1.7 (Memory Leaks)
- 2.2, 2.4, 2.6, 2.7, 2.8 (Lógica)
- 3.2, 3.3 (FD)
- 4.1, 4.2 (Inicialización)
- 5.1, 5.2, 5.4 (Funciones)
- Esfuerzo total: ~125 minutos

### 📋 VERIFICACIÓN (Bajo riesgo) - 3 problemas
- 3.4, 3.5, 4.3 (Estado: CORRECTO)

---

## 🔧 PLAN DE IMPLEMENTACIÓN

### Fase 1: Correcciones Rápidas (30 minutos)
```
1. Cambiar tokenize() - input[0] a input[i]     [2.6]   1 min
2. Liberar sorted_vars en export                 [1.3]   2 min
3. Agregar unlink /tmp/heredoc                   [3.2]   5 min
4. Validar cmd->av en ft_echo                    [2.2]   10 min
5. Revisar doble free cleanup                    [2.5]   5 min
```

### Fase 2: Memory Leaks (45 minutos)
```
1. Crear path_utils.c con free_path_env()        [1.1,1.2] 15 min
2. Usar free_path_env en 2 archivos              [1.1,1.2] 10 min
3. Liberar env en cleanup_shell()                [1.7,4.2] 10 min
4. Mejorar add_local_var validation              [1.6]     10 min
```

### Fase 3: Lógica y Control de Flujo (60 minutos)
```
1. Arreglar token pointer en handle_redirection  [2.1]    20 min
2. Estandarizar token handling en parser         [2.3,5.2] 30 min
3. Verificar/implementar unset_variables         [2.4]    10 min
```

### Fase 4: Validación (30 minutos)
```
1. Cambiar fd_checker a int y validar           [3.1]    10 min
2. Crear copy_env si no existe                  [4.1,5.4] 15 min
3. Buscar set_builtin_flag y validate_final     [2.8,5.1] 5 min
```

### Fase 5: Testing con Valgrind (sin límite de tiempo)
```
1. Compilar con flags de debug
2. Ejecutar test_valgrind.sh
3. Analizar logs de valgrind
4. Iterar hasta 0 leaks
```

---

## ✅ COMANDOS DE VALIDACIÓN

```bash
# Compilar con debug
make clean && make CFLAGS="-g -O0 -Wall -Wextra"

# Ejecutar tests básicos
echo "exit" | ./minishell
echo "pwd\nexit" | ./minishell
echo "echo test\nexit" | ./minishell
echo "export VAR=value\nexit" | ./minishell
echo "invalidcmd\nexit" | ./minishell

# Valgrind individual
valgrind --leak-check=full ./minishell << EOF
exit
EOF

# Script de validación
./test_valgrind.sh

# Buscar bugs específicos
grep -n "input\[0\]" parser/parser_tokenizer0.c
grep -n "free.*sorted_vars" commands_built/ft_export.c
grep -n "free_path_env" execute/execution_*.c
```

---

## 📈 MÉTRICAS DE ÉXITO

| Métrica | Antes | Objetivo |
|---------|-------|----------|
| Memory Leaks (Valgrind) | 1000+ bytes | 0 bytes |
| Errores de lógica | 8+ | 0 |
| File Descriptors sin validar | 3+ | 0 |
| Doble free | Sí | No |
| Coverage de errores | <50% | >90% |

---

## 📝 NOTAS DE IMPLEMENTACIÓN

### Orden recomendado de corrección:
1. Cambios triviales primero (tokenize, export)
2. Memory leaks (crear helpers, liberar)
3. Lógica compleja (token pointer, parsing)
4. Validaciones y edge cases
5. Testing exhaustivo con Valgrind

### Herramientas útiles:
- `valgrind --leak-check=full`: Detectar leaks
- `grep -n "pattern"`: Buscar código
- `gdb ./minishell`: Debugger
- `strace ./minishell`: Rastrear syscalls
- `cppcheck`: Static analysis

### Recursos recomendados:
- Valgrind User Manual
- GDB Tutorial
- C Memory Management Best Practices
- Minishell 42 Subject (verificar requirements)

---

## 🎯 PRÓXIMOS PASOS

1. ✅ Leer este análisis completo
2. ✅ Revisar los archivos mencionados
3. ✅ Implementar correcciones en orden de prioridad
4. ✅ Compilar sin warnings
5. ✅ Ejecutar con Valgrind
6. ✅ Verificar 0 leaks en todos los tests
7. ✅ Hacer commit con descripción detallada

---

**Documento generado:** 2025-11-27
**Total de problemas encontrados:** 25+
**Esfuerzo estimado de corrección:** 4-6 horas
**Prioridad:** 🔴 ALTA - Se requieren correcciones antes de entregar

