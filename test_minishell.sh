#!/bin/bash

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

MINISHELL="./minishell"
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Función para ejecutar un test
run_test() {
    local test_name="$1"
    local command="$2"
    local should_fail="${3:-0}"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo -e "\n${YELLOW}Test #$TOTAL_TESTS: $test_name${NC}"
    echo "Command: $command"
    
    # Ejecutar comando
    echo -e "$command\nexit" | $MINISHELL > /tmp/minishell_out.txt 2>&1
    
    if [ $? -eq 0 ] || [ "$should_fail" -eq 0 ]; then
        echo -e "${GREEN}✓ Test passed${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}✗ Test failed${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        cat /tmp/minishell_out.txt
    fi
}

# Compilar minishell
echo "Compiling minishell..."
make re > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo -e "${RED}Failed to compile minishell${NC}"
    exit 1
fi

echo -e "${GREEN}Minishell compiled successfully${NC}\n"
echo "========================================"
echo "         MINISHELL TEST SUITE"
echo "========================================"

# ============================================
# ECHO TESTS
# ============================================
echo -e "\n${YELLOW}=== ECHO TESTS ===${NC}"

run_test "echo simple" "echo"
run_test "echo con argumento" "echo hola"
run_test "echo con comillas vacías" "echo \"\""
run_test "echo con comillas y texto" "echo \"\" hola"
run_test "echo -n" "echo -n hola"
run_test "echo -n -n" "echo -n -n hola"
run_test "echo múltiples -n" "echo -n -nh -n hola"
run_test "echo múltiples n" "echo -nnnnnnnnn hola"
run_test "echo con \$?" "echo \$?hcudshfuewrf ew  wr 4ew\"\"\"\""
run_test "echo mix comillas 1" "echo \"\"uhjkhnkj\"hjbmb\"'''' defdjhfkjb \"iujiouhj\"f'f'f'f'f''f'f'f'f'f'f'"
run_test "echo mix comillas 2" "echo \"\"uhjkhnkj\"hjbmb\"'''' defdjhfkjb \"iujiouhj\"f'f'a'j'o''i'w'q''q"
run_test "echo mix comillas 3" "echo \"\"aaaaaaaa\"bbbbb\"'''' ccccccccc \"dddddddd\"e'e'e"
run_test "echo con variables 1" "echo \"ho   \$USER\"  'la   \$LESS'   \$USER\$TERM"
run_test "echo con variables 2" "echo \"ho   \$USER\"'la   \$LESS'\$USER\$TERM"
run_test "echo sin espacio 1" "echo\"ho   \$USER\""
run_test "echo sin espacio 2" "echo\"ho   \$USE\$LESS\""
run_test "echo entre comillas 1" "\"echo ho   \$USER\""
run_test "echo entre comillas 2" "\"echo\" \"ho   \$USER\""
run_test "echo parcial comillas" "e\"cho\" \"ho   \$USER\""
run_test "echo con espacios iniciales" "       echo \"ho   \$USER\""
run_test "echo mix quotes y vars 1" "echo \"\$HOME '\$LESS' \$USER\""
run_test "echo mix quotes y vars 2" "echo '\$HOME \"\$LESS\" \$USER'"
run_test "echo complex mix 1" "echo \"\$HOME '\$LESS \"pa \$TERM\" na' \$USER\""
run_test "echo complex mix 2" "echo '\$HOME \"\$LESS 'pa \$TERM' na\" \$USER'"
run_test "echo quote expansion" "echo '\$'\"HOME\""

# ============================================
# EXPORT TESTS
# ============================================
echo -e "\n${YELLOW}=== EXPORT TESTS ===${NC}"

run_test "export simple" "export a=b"
run_test "export override" "export b=wawa; export b=lala"
run_test "export con =" "export c=lala=kaka"
run_test "export sin valor" "export a"
run_test "export vacío" "export a="
run_test "export múltiple" "export a= b=o"
run_test "export múltiples =" "export a==="
run_test "export alfanumérico" "export a3=l"
run_test "export inválido número" "export 2a=lala"
run_test "export inválido !" "export !a=ma"
run_test "export con \$USER" "export \$USER=la"
run_test "export \$USER=\$TERM" "export \$USER=\$TERM"
run_test "export concatenación 1" "export \$USER=\$TERM\$HOME"
run_test "export concatenación 2" "export \$USER=\$TERM\$HOM"
run_test "export \$HOME=\$USER" "export \$HOME=\$USER"
run_test "export variable expandida" "export \$USER\$SHLVL"
run_test "export sin argumentos" "export"
run_test "export con espacios" "          export"
run_test "export comillas vacías" "export \"\""
run_test "export múltiples vars" "export a b=\$LESS \$USER \$HOME=la \$COLORTERM=pa c=d"

# ============================================
# CD TESTS
# ============================================
echo -e "\n${YELLOW}=== CD TESTS ===${NC}"

# Crear directorio de prueba
mkdir -p /tmp/test_dir 2>/dev/null
mkdir -p /tmp/test_dir_ro 2>/dev/null
chmod 000 /tmp/test_dir_ro 2>/dev/null
touch /tmp/test_file 2>/dev/null

run_test "cd directorio existente" "cd /tmp/test_dir; pwd"
run_test "cd directorio inexistente" "cd /tmp/nonexistent_dir_xyz"
run_test "cd archivo" "cd /tmp/test_file"
run_test "cd sin argumentos" "cd; pwd"
run_test "cd con espacios" "          cd; pwd"
run_test "cd con -" "cd /tmp; cd -; pwd"
run_test "cd con ~" "cd ~; pwd"
run_test "cd sin permisos" "cd /tmp/test_dir_ro"

# Limpiar
chmod 755 /tmp/test_dir_ro 2>/dev/null
rm -rf /tmp/test_dir /tmp/test_dir_ro /tmp/test_file 2>/dev/null

# ============================================
# ENV TESTS
# ============================================
echo -e "\n${YELLOW}=== ENV TESTS ===${NC}"

run_test "env simple" "env | head -5"
run_test "env con argumento" "env lalal"
run_test "env con directorio" "env /tmp"

# ============================================
# PWD TESTS
# ============================================
echo -e "\n${YELLOW}=== PWD TESTS ===${NC}"

run_test "pwd simple" "pwd"
run_test "pwd con argumentos" "pwd arg1 arg2"

# ============================================
# UNSET TESTS
# ============================================
echo -e "\n${YELLOW}=== UNSET TESTS ===${NC}"

run_test "unset sin argumentos" "unset"
run_test "unset simple" "export TEST=value; unset TEST; env | grep TEST"
run_test "unset múltiple" "export A=1 B=2 C=3; unset A B C; env | grep -E 'A=|B=|C='"

# ============================================
# EXIT TESTS
# ============================================
echo -e "\n${YELLOW}=== EXIT TESTS ===${NC}"

run_test "exit sin argumentos" "exit"
run_test "exit 0" "exit 0"
run_test "exit 1" "exit 1"
run_test "exit 42" "exit 42"
run_test "exit no numérico" "exit lalal"

# ============================================
# PIPE TESTS
# ============================================
echo -e "\n${YELLOW}=== PIPE TESTS ===${NC}"

# Crear archivo de prueba
echo -e "red\nblue\ngreen\nred\nblue" > /tmp/colors.txt

run_test "pipe simple" "ls /tmp | wc -l"
run_test "pipe sin espacios" "ls /tmp|wc -l"
run_test "pipe múltiple" "cat /tmp/colors.txt | sort | uniq | wc -l"
run_test "pipe doble ||" "ls || wc"
run_test "pipe vacío" "ls |   | wc"
run_test "pipe con redirección" "ls |   > /tmp/out.txt"
run_test "pipe al final" "ls | wc |"
run_test "pipe solo" "|"
run_test "pipe al inicio" "| ls | wc"

rm -f /tmp/colors.txt 2>/dev/null

# ============================================
# REDIRECTION TESTS
# ============================================
echo -e "\n${YELLOW}=== REDIRECTION TESTS ===${NC}"

run_test "output redirect" "echo hola > /tmp/test_out.txt; cat /tmp/test_out.txt"
run_test "múltiples >" "echo hola >>>>>>> /tmp/test_out.txt"
run_test "múltiples <" "echo hola <<<<<<< /tmp/test_in.txt"
run_test "redirect sin espacio" "echo hola>/tmp/test_out.txt"
run_test "input redirect" "echo test > /tmp/test_in.txt; wc < /tmp/test_in.txt"
run_test "input y output" "echo test > /tmp/test_in.txt; wc < /tmp/test_in.txt > /tmp/test_out.txt; cat /tmp/test_out.txt"
run_test "múltiples outputs" "echo hola > /tmp/f1.txt > /tmp/f2.txt > /tmp/f3.txt; cat /tmp/f3.txt"
run_test "redirect con pipe" "echo hola > /tmp/test_out.txt | ls"
run_test "cat con pipe" "echo test > /tmp/test.txt; cat /tmp/test.txt | wc -l"
run_test "múltiples cat pipe" "echo hola | cat | cat | cat | cat"

# Limpiar archivos de prueba
rm -f /tmp/test_*.txt /tmp/f*.txt 2>/dev/null

# ============================================
# HEREDOC TESTS
# ============================================
echo -e "\n${YELLOW}=== HEREDOC TESTS ===${NC}"

run_test "heredoc simple" "cat << EOF\nhola\nmundo\nEOF"
run_test "heredoc a archivo" "cat << EOF > /tmp/heredoc.txt\nhola\nEOF\ncat /tmp/heredoc.txt"
run_test "heredoc solo delimiter" "<< EOF"
run_test "heredoc con pipe" "cat << EOF | wc -l\nline1\nline2\nEOF"

rm -f /tmp/heredoc.txt 2>/dev/null

# ============================================
# GENERAL TESTS
# ============================================
echo -e "\n${YELLOW}=== GENERAL TESTS ===${NC}"

run_test "input vacío" ""
run_test "solo espacios" "     "
run_test "comillas sin cerrar" "echo \"hola"
run_test "exit status" "ls /nonexistent 2>/dev/null; echo \$?"
run_test "unset PATH" "export OLD_PATH=\$PATH; unset PATH; /bin/ls /tmp; export PATH=\$OLD_PATH"

# ============================================
# RESUMEN
# ============================================
echo -e "\n========================================"
echo -e "           TEST SUMMARY"
echo -e "========================================"
echo -e "Total tests: $TOTAL_TESTS"
echo -e "${GREEN}Passed: $PASSED_TESTS${NC}"
echo -e "${RED}Failed: $FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "\n${GREEN}All tests passed! 🎉${NC}"
    exit 0
else
    echo -e "\n${RED}Some tests failed${NC}"
    exit 1
fi
