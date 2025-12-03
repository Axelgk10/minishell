# FLUJO DEL PARSER - MINISHELL

## 📋 ÍNDICE
1. [Visión General](#visión-general)
2. [Fase 1: Tokenización](#fase-1-tokenización)
3. [Fase 2: Expansión](#fase-2-expansión)
4. [Fase 3: Parsing](#fase-3-parsing)
5. [Diagrama de Flujo](#diagrama-de-flujo)

---

## VISIÓN GENERAL

El parser transforma el input del usuario en una estructura ejecutable mediante 3 fases:

```
INPUT STRING → [TOKENIZE] → TOKENS → [EXPAND] → TOKENS EXPANDIDOS → [PARSE] → COMANDOS
```

**Ejemplo:**
```bash
echo "hola $USER" | cat > file.txt
```

---

## FASE 1: TOKENIZACIÓN

**Archivo:** `parser/parser_tokenizer0.c`, `parser/parser_tokenizer_utils1.c`, `parser/chicken_token.c`

### 🎯 Objetivo
Convertir el string de entrada en una lista enlazada de tokens

### 📝 Funciones principales

#### `tokenize(char *input)` 
**Ubicación:** `parser_tokenizer_utils1.c`
- **Entrada:** String raw del usuario
- **Salida:** Lista enlazada de tokens (`t_token *`)
- **Proceso:**
  1. Recorre el string carácter por carácter
  2. Ignora espacios/tabs
  3. Llama a `process_token()` para cada elemento

#### `process_token(char *input, int *i, t_token **head)`
**Ubicación:** `parser_tokenizer_utils1.c`
- **Función:** Identifica y crea un token individual
- **Lógica:**
  ```c
  if (es_metacaracter && no_es_espacio)
      extraer_metacaracter()  // |, <, >, <<, >>
  else
      extract_word()          // Palabras/argumentos
  ```
- **Llama a:**
  - `extract_metachar()` → Para pipes y redirections
  - `extract_word()` → Para comandos y argumentos

#### `extract_word(char *input, int *i)`
**Ubicación:** `parser/chicken_token.c`
- **Función:** Extrae una palabra respetando comillas
- **Características:**
  - Maneja comillas simples (`'`) y dobles (`"`)
  - No rompe dentro de comillas
  - Retorna el substring completo

#### `extract_metachar(char *input, int *i)`
**Ubicación:** `parser_tokenizer0.c`
- **Función:** Extrae operadores (`|`, `<`, `>`, `<<`, `>>`)
- **Detecta:** Operadores de 1 o 2 caracteres

#### `get_token_type(char *str)`
**Ubicación:** `parser_tokenizer0.c`
- **Función:** Asigna tipo a cada token
- **Tipos:**
  ```c
  T_PIPE      // |
  T_REDIR_IN  // <
  T_REDIR_OUT // >
  T_APPEND    // >>
  T_HEREDOC   // <<
  T_WORD      // Cualquier otra cosa
  ```

### 📊 Estructura de datos
```c
typedef struct s_token {
    t_token_type    type;      // Tipo de token
    char           *value;     // Contenido del token
    struct s_token *next;      // Siguiente token
} t_token;
```

### 🔍 Ejemplo paso a paso
**Input:** `echo "hola" | cat`

1. `tokenize()` empieza a recorrer
2. Encuentra 'e' → llama `process_token()`
3. `extract_word()` lee hasta espacio → `"echo"`
4. Crea token: `{type: T_WORD, value: "echo"}`
5. Encuentra '"' → `extract_word()` lee hasta cerrar comilla → `"\"hola\""`
6. Crea token: `{type: T_WORD, value: "\"hola\""}`
7. Encuentra '|' → `extract_metachar()` → `"|"`
8. Crea token: `{type: T_PIPE, value: "|"}`
9. Encuentra 'c' → `extract_word()` → `"cat"`
10. Crea token: `{type: T_WORD, value: "cat"}`

**Resultado:**
```
[echo] -> ["hola"] -> [|] -> [cat] -> NULL
 WORD      WORD      PIPE    WORD
```

---

## FASE 2: EXPANSIÓN

**Archivos:** `parser/parser_expander6.c`, `parser/parser_expander_utils7.c`, `parser/expander_chicken.c`, `parser/expander_double_quotes.c`, `parser/expander_helpers.c`

### 🎯 Objetivo
Expandir variables (`$VAR`), eliminar comillas, procesar expansiones

### 📝 Funciones principales

#### `expand_variables(t_shell *shell, t_token *tokens)`
**Ubicación:** `parser_expander_utils7.c`
- **Entrada:** Lista de tokens sin expandir
- **Proceso:**
  1. Recorre cada token de tipo `T_WORD`
  2. Verifica si necesita expansión (`requires_expansion()`)
  3. Si sí, llama a `expand_string()`
  4. Reemplaza el valor antiguo por el expandido

#### `requires_expansion(char *str)`
**Ubicación:** `parser_expander_utils7.c` (static)
- **Función:** Detecta si un string necesita procesamiento
- **Retorna 1 si hay:**
  - Variables: `$USER`, `$PATH`, `$?`
  - Comillas: `'texto'` o `"texto"`

#### `expand_string(t_shell *shell, char *str)`
**Ubicación:** `parser_expander_utils7.c`
- **Función:** Procesa un string completo
- **Proceso:**
  1. Inicializa resultado vacío (`""`)
  2. Recorre carácter por carácter
  3. Según el carácter, llama a:
     - `handle_single_quotes()` → Comillas simples
     - `handle_double_quotes()` → Comillas dobles
     - `expand_dollar()` → Variables
     - `handle_regular_char()` → Caracteres normales
  4. Concatena todas las partes

#### `handle_single_quotes(char *str, int *i)`
**Ubicación:** `parser_expander6.c`
- **Función:** Extrae contenido entre `'...'`
- **Comportamiento:** NO expande nada, retorna literal
- **Ejemplo:** `'$USER'` → `$USER` (sin expandir)

#### `handle_double_quotes(t_shell *shell, char *str, int *i)`
**Ubicación:** `parser/expander_chicken.c`
- **Función:** Procesa contenido entre `"..."`
- **Comportamiento:** SÍ expande variables
- **Llama a:** `process_double_quote_content()`

#### `process_double_quote_content(t_shell *shell, char *str, int *i)`
**Ubicación:** `parser/expander_chicken.c`
- **Función:** Procesa carácter por carácter dentro de `"..."`
- **Maneja:**
  - Comillas simples `'` → Las mantiene literales
  - Variables `$VAR` → Las expande
  - Otros caracteres → Los copia
- **Llama a:**
  - `handle_single_quote_in_double()` → Para `'`
  - `handle_dollar_in_double()` → Para `$`
  - `handle_regular_in_double()` → Para otros

#### `expand_dollar(t_shell *shell, char *str, int *i)`
**Ubicación:** `parser_expander6.c`
- **Función:** Expande una variable `$VAR`
- **Proceso:**
  1. Extrae el nombre de la variable (`extract_variable_name()`)
  2. Busca su valor en el entorno (`get_env_value()`)
  3. Retorna el valor (o `""` si no existe)
- **Casos especiales:**
  - `$?` → Exit status del último comando
  - `$` solo → Retorna `"$"`

#### `get_env_value(t_shell *shell, char *var_name)`
**Ubicación:** `parser_expander6.c` (static)
- **Función:** Busca una variable en el array de entorno
- **Búsqueda:** Compara `VAR=valor` hasta encontrar coincidencia
- **Retorna:** El valor después del `=`

### 🔍 Ejemplo paso a paso
**Input token:** `"hola $USER"`

1. `expand_variables()` detecta comillas → necesita expansión
2. `expand_string()` empieza a procesar
3. Encuentra `"` → llama `handle_double_quotes()`
4. Dentro: `process_double_quote_content()` procesa:
   - `h` → `handle_regular_in_double()` → agrega 'h'
   - `o` → agrega 'o'
   - `l` → agrega 'l'
   - `a` → agrega 'a'
   - ` ` → agrega ' '
   - `$` → `handle_dollar_in_double()`:
     - `extract_variable_name()` → `"USER"`
     - `get_env_value()` → `"axgimene"`
     - Agrega `"axgimene"`
5. Cierra comilla `"`
6. Resultado final: `hola axgimene` (sin comillas)

**Antes:** `["echo"] -> ["\"hola $USER\""] -> ["|"] -> ["cat"]`
**Después:** `["echo"] -> ["hola axgimene"] -> ["|"] -> ["cat"]`

---

## FASE 3: PARSING

**Archivos:** `parser/parser_parser_main4.c`, `parser/parser_parser_args2.c`, `parser/parser_parser_init5.c`, `parser/parser_builtins2.c`, `parser/parser_redirections_Chicken.c`, `parser/parser_redirections_little_chickens3.c`, `parser/message_error_main.c`

### 🎯 Objetivo
Convertir tokens en comandos ejecutables con sus argumentos, pipes y redirections

### 📝 Funciones principales

#### `parse_tokens(t_token *tokens)`
**Ubicación:** `parser_parser_main4.c`
- **Entrada:** Lista de tokens expandidos
- **Salida:** Lista de comandos (`t_cmd *`)
- **Proceso:**
  1. Crea primer comando (`init_first_command()`)
  2. Procesa todos los tokens (`process_all_tokens()`)
  3. Valida comando final (`validate_final_command()`)
  4. Marca si es builtin (`set_builtin_flag()`)

#### `process_all_tokens(t_token **current, t_cmd **current_cmd)`
**Ubicación:** `parser_parser_main4.c` (static)
- **Función:** Loop principal que procesa cada token
- **Proceso:**
  ```c
  while (hay_tokens) {
      process_token_in_parser()  // Procesa 1 token
      avanzar_al_siguiente()
      if (error) break;
  }
  ```

#### `process_token_in_parser(t_token **current_token, t_cmd **current_cmd)`
**Ubicación:** `parser_parser_main4.c` (static)
- **Función:** Dispatcher que decide qué hacer con cada token
- **Lógica:**
  ```c
  if (T_WORD)         → process_word_token()
  if (T_PIPE)         → handle_pipe_token()
  if (T_REDIR_*)      → process_redir_token()
  ```

#### `process_word_token(t_token *current_token, t_cmd *current_cmd)`
**Ubicación:** `parser_parser_main4.c` (static)
- **Función:** Agrega un argumento al comando actual
- **Acción:** `add_arg_to_command(cmd, token->value)`

#### `add_arg_to_command(t_cmd *cmd, char *arg)`
**Ubicación:** `parser_parser_args2.c`
- **Función:** Añade un argumento al array `av[]` del comando
- **Proceso:**
  1. Crea nuevo array de tamaño `ac + 2`
  2. Copia argumentos anteriores
  3. Duplica el nuevo argumento (`ft_strdup()`)
  4. Incrementa contador `ac`

#### `handle_pipe_token(t_token **current_token, t_cmd **current_cmd)`
**Ubicación:** `parser/message_error_main.c`
- **Función:** Crea un nuevo comando conectado por pipe
- **Proceso:**
  1. Valida que el comando actual tenga argumentos
  2. Marca comando como builtin si corresponde
  3. Crea nuevo comando (`create_command()`)
  4. Configura el pipe (`setup_pipe_fds()`)
  5. Conecta comandos: `current->next = new`

#### `setup_pipe_fds(t_cmd *current_cmd, t_cmd *new_cmd)`
**Ubicación:** `parser/message_error_main.c`
- **Función:** Crea un pipe y conecta file descriptors
- **Configuración:**
  ```c
  pipe(current_cmd->pipe)           // Crea pipe[0] y pipe[1]
  current_cmd->out_fd = pipe[1]     // Salida del comando actual
  new_cmd->in_fd = pipe[0]          // Entrada del siguiente comando
  ```

#### `process_redir_token(t_token **current_token, t_cmd **current_cmd)`
**Ubicación:** `parser_parser_main4.c` (static)
- **Función:** Procesa redirections (`<`, `>`, `>>`, `<<`)
- **Validaciones:**
  - Verifica que haya un filename después del operador
  - Si falla, imprime error (`print_redir_error()`)
- **Acción:** `handle_redirection()`

#### `handle_redirection(t_token **tokens, t_cmd *cmd)`
**Ubicación:** `parser/parser_redirections_Chicken.c`
- **Función:** Dispatcher de redirections
- **Llama a:**
  - `handle_input_redirection()` → Para `<`
  - `handle_output_redirection()` → Para `>`
  - `handle_append_redirection()` → Para `>>`
  - `handle_heredoc()` → Para `<<`

#### `handle_input_redirection(char *filename)`
**Ubicación:** `parser/parser_redirections_little_chickens3.c`
- **Función:** Abre archivo para lectura
- **Retorna:** File descriptor o -1 si error

#### `handle_output_redirection(char *filename)`
**Ubicación:** `parser/parser_redirections_little_chickens3.c`
- **Función:** Abre/crea archivo para escritura (truncate)
- **Flags:** `O_WRONLY | O_CREAT | O_TRUNC`

#### `handle_append_redirection(char *filename)`
**Ubicación:** `parser/parser_redirections_little_chickens3.c`
- **Función:** Abre/crea archivo para append
- **Flags:** `O_WRONLY | O_CREAT | O_APPEND`

#### `handle_heredoc(char *delimiter)`
**Ubicación:** `parser/parser_redirections_little_chickens3.c`
- **Función:** Implementa heredoc (`<<`)
- **Proceso:**
  1. Lee líneas con `readline("> ")`
  2. Escribe en `/tmp/heredoc` hasta encontrar delimitador
  3. Abre el archivo en modo lectura
  4. Retorna file descriptor

#### `set_builtin_flag(t_cmd *cmd)`
**Ubicación:** `parser/parser_builtins2.c`
- **Función:** Marca si un comando es builtin
- **Builtins:** `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`
- **También:** Detecta asignaciones (`VAR=value`)

### 📊 Estructura de datos
```c
typedef struct s_cmd {
    char          **av;         // Array de argumentos
    int             ac;         // Contador de argumentos
    int             in_fd;      // FD de entrada (default: STDIN)
    int             out_fd;     // FD de salida (default: STDOUT)
    int             pipe[2];    // FDs del pipe
    int             is_builtin; // 1 si es builtin
    struct s_cmd   *next;       // Siguiente comando
} t_cmd;
```

### 🔍 Ejemplo paso a paso
**Tokens:** `[echo] [hola] [|] [cat] [>] [file.txt]`

1. `parse_tokens()` inicia
2. `init_first_command()` → Crea `cmd1`
3. `process_all_tokens()` empieza loop:

   **Iteración 1:** Token `[echo]`
   - `process_token_in_parser()` → Es `T_WORD`
   - `process_word_token()` → Agrega "echo" a `cmd1->av[0]`
   
   **Iteración 2:** Token `[hola]`
   - Es `T_WORD`
   - Agrega "hola" a `cmd1->av[1]`
   
   **Iteración 3:** Token `[|]`
   - Es `T_PIPE`
   - `handle_pipe_token()`:
     - Marca `cmd1` como builtin
     - Crea `cmd2`
     - `setup_pipe_fds()`: crea pipe, conecta `cmd1->out_fd = pipe[1]`, `cmd2->in_fd = pipe[0]`
     - `cmd1->next = cmd2`
   
   **Iteración 4:** Token `[cat]`
   - Es `T_WORD`
   - Agrega "cat" a `cmd2->av[0]`
   
   **Iteración 5:** Token `[>]`
   - Es `T_REDIR_OUT`
   - `process_redir_token()`:
     - Valida siguiente token es `T_WORD`
     - `handle_redirection()`:
       - `handle_output_redirection("file.txt")`
       - Abre "file.txt" → FD = 3
       - `cmd2->out_fd = 3`
   
   **Iteración 6:** Token `[file.txt]`
   - Ya procesado, se salta

4. `validate_final_command()` → Verifica que `cmd2` tenga argumentos
5. `set_builtin_flag(cmd2)` → No es builtin

**Resultado:**
```
cmd1: {
    av: ["echo", "hola", NULL]
    ac: 2
    in_fd: 0 (STDIN)
    out_fd: 4 (pipe[1])
    pipe: [3, 4]
    is_builtin: 1
    next: cmd2
}

cmd2: {
    av: ["cat", NULL]
    ac: 1
    in_fd: 3 (pipe[0])
    out_fd: 5 (file.txt)
    pipe: [-1, -1]
    is_builtin: 0
    next: NULL
}
```

---

## DIAGRAMA DE FLUJO

```
┌─────────────────────────────────────────────────────────┐
│                    ENTRADA DEL USUARIO                   │
│           "echo \"hola $USER\" | cat > out.txt"          │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌────────────────────────────────────────────────────────┐
│              FASE 1: TOKENIZACIÓN                       │
│                                                         │
│  tokenize() ──► process_token() ──► extract_word()      │
│                       │                   │              │
│                       └──► extract_metachar()            │
│                       │                                  │
│                       └──► get_token_type()              │
│                                                          │
│  Resultado:                                              │
│  [echo] [\"hola $USER\"] [|] [cat] [>] [out.txt]        │
│   WORD      WORD         PIPE WORD REDIR  WORD          │
└────────────────────┬───────────────────────────────────┘
                     │
                     ▼
┌────────────────────────────────────────────────────────┐
│              FASE 2: EXPANSIÓN                          │
│                                                         │
│  expand_variables() ──► requires_expansion()            │
│         │                                                │
│         └──► expand_string() ──► handle_double_quotes() │
│                    │                     │               │
│                    │           ┌─────────┘               │
│                    │           │                         │
│                    │           └──► expand_dollar()      │
│                    │           │        │                │
│                    │           │        └─► get_env_value()
│                    │           │                         │
│                    └──► handle_single_quotes()           │
│                                                          │
│  Resultado:                                              │
│  [echo] [hola axgimene] [|] [cat] [>] [out.txt]         │
│   WORD      WORD         PIPE WORD REDIR  WORD          │
└────────────────────┬───────────────────────────────────┘
                     │
                     ▼
┌────────────────────────────────────────────────────────┐
│              FASE 3: PARSING                            │
│                                                         │
│  parse_tokens() ──► init_first_command()                │
│        │                                                 │
│        └──► process_all_tokens() ──► process_token_in_parser()
│                    │                          │          │
│                    │              ┌───────────┘          │
│                    │              │                      │
│                    │              ├─► process_word_token()
│                    │              │        │             │
│                    │              │        └─► add_arg_to_command()
│                    │              │                      │
│                    │              ├─► handle_pipe_token()│
│                    │              │        │             │
│                    │              │        └─► setup_pipe_fds()
│                    │              │                      │
│                    │              └─► process_redir_token()
│                    │                       │             │
│                    │                       └─► handle_redirection()
│                    │                                │    │
│                    │                    ┌───────────┘    │
│                    │                    │                │
│                    │                    ├─► handle_input_redirection()
│                    │                    ├─► handle_output_redirection()
│                    │                    ├─► handle_append_redirection()
│                    │                    └─► handle_heredoc()
│                    │                                     │
│        ┌───────────┘                                     │
│        │                                                 │
│        └──► validate_final_command()                     │
│        │                                                 │
│        └──► set_builtin_flag()                           │
│                                                          │
│  Resultado: Lista enlazada de comandos                   │
│                                                          │
│  cmd1 (echo hola axgimene)                               │
│    ├─ av: ["echo", "hola", "axgimene"]                  │
│    ├─ out_fd: pipe[1]                                   │
│    └─ next ──────┐                                      │
│                   │                                      │
│  cmd2 (cat)      │                                      │
│    ├─ av: ["cat"] │                                     │
│    ├─ in_fd: pipe[0]                                    │
│    ├─ out_fd: FD de out.txt                             │
│    └─ next: NULL                                         │
└─────────────────────────────────────────────────────────┘
                     │
                     ▼
          ┌──────────────────┐
          │    EJECUCIÓN     │
          └──────────────────┘
```

---

## 🎓 RESUMEN POR ARCHIVO

### Tokenización
- **parser_tokenizer0.c**: Detecta tipos de token, extrae metacaracteres
- **parser_tokenizer_utils1.c**: Loop principal, creación de tokens
- **chicken_token.c**: Extracción de palabras respetando comillas

### Expansión
- **parser_expander6.c**: Expansión de variables (`$VAR`), comillas simples
- **parser_expander_utils7.c**: Coordinador de expansión, loop principal
- **expander_chicken.c**: Comillas dobles, contenido dentro de `"..."`
- **expander_double_quotes.c**: Helpers para procesamiento dentro de `"..."`
- **expander_helpers.c**: Utilidades (concatenación, errores)

### Parsing
- **parser_parser_main4.c**: Coordinador principal, dispatcher de tokens
- **parser_parser_args2.c**: Creación de comandos, gestión de argumentos
- **parser_parser_init5.c**: Inicialización de estructuras
- **parser_builtins2.c**: Detección de builtins
- **parser_redirections_Chicken.c**: Dispatcher de redirections
- **parser_redirections_little_chickens3.c**: Implementación de cada tipo de redirection
- **message_error_main.c**: Validaciones, manejo de pipes, errores de sintaxis

### Limpieza
- **free_clean8.c**: Liberación de memoria de tokens y comandos

---

## ✅ GARANTÍAS

1. **Sin memory leaks**: Todas las funciones liberan memoria correctamente
2. **Norma 42**: Máx 25 líneas/función, máx 5 funciones/archivo
3. **Solo funciones autorizadas**: libft + funciones permitidas del proyecto
4. **Validaciones**: Cada fase valida errores de sintaxis
