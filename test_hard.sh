#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

MINISHELL="./minishell"
VALGRIND="valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=readline_leaks.supp -q"
TOTAL=0
PASSED=0
FAILED=0

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}   MINISHELL HARD TESTS + VALGRIND${NC}"
echo -e "${BLUE}======================================${NC}\n"

# Función para ejecutar test con valgrind
run_hard_test() {
    local test_name="$1"
    local command="$2"
    local should_work="${3:-1}"
    
    TOTAL=$((TOTAL + 1))
    
    echo -e "${YELLOW}Test #$TOTAL: $test_name${NC}"
    echo "Command: $command"
    
    # Ejecutar con valgrind
    echo -e "$command\nexit" | $VALGRIND $MINISHELL > /tmp/minishell_out.txt 2>&1
    
    # Verificar leaks
    if grep -q "definitely lost: 0 bytes" /tmp/minishell_out.txt && \
       grep -q "indirectly lost: 0 bytes" /tmp/minishell_out.txt; then
        echo -e "${GREEN}✓ No memory leaks${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗ Memory leak detected!${NC}"
        grep "lost:" /tmp/minishell_out.txt
        FAILED=$((FAILED + 1))
    fi
    echo ""
}

echo -e "${BLUE}=== EDGE CASES - QUOTES ===${NC}\n"

run_hard_test "Comillas vacías múltiples" 'echo "" "" ""'
run_hard_test "Comillas anidadas estilo 1" 'echo "hola'\''mundo"'
run_hard_test "Comillas simples en dobles" "echo \"'test'\""
run_hard_test "Comillas dobles en simples" "echo '\"test\"'"
run_hard_test "Mix extremo" 'echo "a"b"c"'"'"'d'"'"'e'
run_hard_test "Espacios en comillas" 'echo "     "'
run_hard_test "Tabs en comillas" 'echo "		"'

echo -e "${BLUE}=== EDGE CASES - VARIABLES ===${NC}\n"

run_hard_test "Variable inexistente" 'echo $NONEXISTENT'
run_hard_test "Variable con números" 'echo $USER123ABC'
run_hard_test "Dollar solo" 'echo $'
run_hard_test "Dollar con espacio" 'echo $ USER'
run_hard_test "Múltiples dollars" 'echo $$$$'
run_hard_test "Exit status repetido" 'ls /fake; echo $? $? $?'
run_hard_test "Variable en medio" 'echo pre${USER}post'
run_hard_test "Variable vacía" 'A=; echo $A'

echo -e "${BLUE}=== EDGE CASES - REDIRECTIONS ===${NC}\n"

run_hard_test "Múltiples inputs" 'cat < /tmp/f1 < /tmp/f2'
run_hard_test "Redirección a /dev/null" 'echo test > /dev/null'
run_hard_test "Append a inexistente" 'echo test >> /tmp/newfile_$$'
run_hard_test "Input desde inexistente" 'cat < /tmp/noexiste_$$'
run_hard_test "Output sin permisos" 'echo test > /root/noperm 2>/dev/null'
run_hard_test "Heredoc vacío" 'cat << EOF
EOF'
run_hard_test "Heredoc con variables" 'cat << EOF
$USER
EOF'

echo -e "${BLUE}=== EDGE CASES - PIPES ===${NC}\n"

run_hard_test "Pipe con comando vacío" 'echo test | | cat'
run_hard_test "Muchos pipes" 'echo test | cat | cat | cat | cat | cat'
run_hard_test "Pipe con redirección" 'echo test | cat > /tmp/pipe_test_$$'
run_hard_test "Pipe al inicio" '| echo test'
run_hard_test "Pipe al final" 'echo test |'
run_hard_test "Pipes sin espacios" 'echo test|cat|wc'

echo -e "${BLUE}=== EDGE CASES - BUILTINS ===${NC}\n"

run_hard_test "cd a directorio largo" "cd /tmp/../tmp/./././../tmp"
run_hard_test "cd sin args" "cd"
run_hard_test "cd a archivo" "cd /tmp/test_file_$$"
run_hard_test "pwd con muchos args" "pwd a b c d e"
run_hard_test "env con args" "env test"
run_hard_test "export sin =" "export TESTVAR"
run_hard_test "export con múltiples =" "export A=B=C=D"
run_hard_test "export nombre inválido" "export 123=test"
run_hard_test "unset múltiples" "export A=1 B=2; unset A B C"
run_hard_test "echo -n múltiple" "echo -n -n -n -n test"

echo -e "${BLUE}=== EDGE CASES - COMANDOS ===${NC}\n"

run_hard_test "Comando vacío" ""
run_hard_test "Solo espacios" "     "
run_hard_test "Solo tabs" "		"
run_hard_test "Comando inexistente" "comandoquenoexiste123"
run_hard_test "Comando con path" "/bin/echo test"
run_hard_test "Comando relativo ./" "./minishell" 0
run_hard_test "Comando .." ".."
run_hard_test "Comando ." "."

echo -e "${BLUE}=== EDGE CASES - ARGUMENTOS EXTREMOS ===${NC}\n"

run_hard_test "100 argumentos" "echo 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100"
run_hard_test "Argumento muy largo" "echo aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
run_hard_test "Muchas variables" 'echo $USER$HOME$PATH$TERM$SHELL'

echo -e "${BLUE}=== EDGE CASES - EXPORTS COMPLEJOS ===${NC}\n"

run_hard_test "Export con quotes" 'export TEST="hola mundo"'
run_hard_test "Export con comillas simples" "export TEST='hola mundo'"
run_hard_test "Export con espacios en valor" 'export "TEST=hola mundo"'
run_hard_test "Export variable con _" "export MY_VAR_123=test"

echo -e "${BLUE}=== EDGE CASES - MEMORIA ===${NC}\n"

run_hard_test "Muchas comillas" 'echo "a" "b" "c" "d" "e" "f" "g" "h" "i" "j" "k" "l" "m" "n" "o" "p"'
run_hard_test "Comando largo repetido" 'echo test; echo test; echo test; echo test; echo test'
run_hard_test "Variables repetidas" 'export A=1; export A=2; export A=3; unset A; export A=4'

echo -e "${BLUE}=== EDGE CASES - PATHS ===${NC}\n"

run_hard_test "cd ~" "cd ~; pwd"
run_hard_test "cd ~/test/../" "cd ~/; pwd"
run_hard_test "cd -" "cd /tmp; cd -; pwd"
run_hard_test "cd con path HOME" "cd $HOME; pwd"

echo -e "${BLUE}=== STRESS TEST - LEAKS PESADOS ===${NC}\n"

# Test de leak masivo
echo -e "${YELLOW}Test #$((TOTAL + 1)): Stress test - 50 comandos consecutivos${NC}"
TOTAL=$((TOTAL + 1))
COMMANDS="pwd
ls /tmp
echo test
cd /tmp
pwd
cd -
export TEST=value
echo \$TEST
unset TEST
cat /etc/passwd | head -5
echo \$USER
pwd
ls /
echo hola
cat /etc/hosts | grep localhost
echo \$HOME
export A=1
export B=2
unset A B
cd /
pwd
cd ~
echo test | cat
ls /tmp | wc -l
echo \$PATH
pwd
env | head -3
export C=test
echo \$C
unset C
cd /tmp
ls
pwd
cd -
echo \"test\"
echo 'test'
export D=\"value\"
echo \$D
cat /etc/hostname
pwd
ls /home
echo done
pwd
cd /
pwd
cd ~
pwd
ls | head -1
echo final
pwd
exit"

echo "$COMMANDS" | $VALGRIND $MINISHELL > /tmp/minishell_stress.txt 2>&1

if grep -q "definitely lost: 0 bytes" /tmp/minishell_stress.txt && \
   grep -q "indirectly lost: 0 bytes" /tmp/minishell_stress.txt; then
    echo -e "${GREEN}✓ No memory leaks after 50 commands${NC}"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ Memory leak detected in stress test!${NC}"
    grep "lost:" /tmp/minishell_stress.txt
    FAILED=$((FAILED + 1))
fi
echo ""

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}         TEST SUMMARY${NC}"
echo -e "${BLUE}======================================${NC}"
echo -e "Total tests: $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed! No memory leaks detected! 🎉${NC}"
else
    echo -e "${RED}Some tests failed. Check the output above.${NC}"
fi
