# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/06 12:00:00 by gguardam          #+#    #+#              #
#    Updated: 2025/11/18 18:28:16 by gguardam         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror

SOURCES = main.c \
			user_route/dir_manager.c \
			commands_built/ft_echo.c \
			commands_built/ft_env.c \
			commands_built/ft_cd.c \
			commands_built/ft_pwd.c \
			commands_built/ft_exit.c \
			commands_built/ft_export.c \
			commands_built/ft_local_var.c \
			execute/execution_utils.c \
			execute/execution_single_command.c \
			execute/execution_piped.c \
			execute/execution_piped_utils.c \
			utils/main_utils.c \
			utils/envs.c \
			utils/envs_utils.c \
			utils/export_utils.c \
			utils/errors.c \
			utils/struct_utils.c \
			signals/signals.c \
			parser/parser_tokenizer0.c \
			parser/parser_tokenizer_utils1.c \
			parser/parser_builtins2.c \
			parser/parser_parser_args2.c \
			parser/parser_redirections_Chicken.c \
			parser/parser_redirections_little_chickens3.c \
			parser/parser_parser_main4.c \
			parser/parser_parser_init5.c \
			parser/parser_expander6.c \
			parser/parser_expander_utils7.c \
			parser/free_clean8.c \
			parser/message_error_main.c \
			parser/expander_chicken.c \
			parser/chicken_token.c \

OBJECTS = $(SOURCES:.c=.o)

INCLUDES = -I. -I./libft
HEADERS = minishell.h

LIBS = -lreadline
LIBFT_DIR = libft.h
LIBFT = $(LIBFT_DIR)/libft.a
LIBFT_FLAGS = -L$(LIBFT_DIR) -lft

all: $(LIBFT) $(NAME)

$(LIBFT):
	@echo "Compiling libft..."
	@make -C $(LIBFT_DIR)

$(NAME): $(OBJECTS) $(LIBFT)
	@echo "Linking $(NAME)..."
	$(CC) $(OBJECTS) $(LIBFT_FLAGS) $(LIBS) $(LDFLAGS) -o $(NAME)
	@echo "$(NAME) compiled successfully!"

%.o: %.c $(HEADERS)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning object files..."
	rm -f $(OBJECTS)
	@make -C $(LIBFT_DIR) clean

fclean: clean
	@echo "Cleaning executables..."
	rm -f $(NAME)
	@make -C $(LIBFT_DIR) fclean

re: fclean all

# AddressSanitizer (muy rápido y efectivo)
asan: CFLAGS += -g -fsanitize=address -fno-omit-frame-pointer
asan: LDFLAGS += -fsanitize=address
asan: re
	@echo "Compiled with AddressSanitizer"

# LeakSanitizer (solo detecta leaks, más rápido que Valgrind)
lsan: CFLAGS += -g -fsanitize=leak -fno-omit-frame-pointer
lsan: LDFLAGS += -fsanitize=leak
lsan: re
	@echo "Compiled with LeakSanitizer"

# UndefinedBehaviorSanitizer (detecta comportamiento indefinido)
ubsan: CFLAGS += -g -fsanitize=undefined -fno-omit-frame-pointer
ubsan: LDFLAGS += -fsanitize=undefined
ubsan: re
	@echo "Compiled with UndefinedBehaviorSanitizer"

# Todos los sanitizers juntos
sanitize: CFLAGS += -g -fsanitize=address,undefined,leak -fno-omit-frame-pointer
sanitize: LDFLAGS += -fsanitize=address,undefined,leak
sanitize: re
	@echo "Compiled with all sanitizers"

# Debug build con símbolos
debug: CFLAGS += -g3 -O0
debug: re
	@echo "Compiled in debug mode"

# Valgrind helper (con supresiones de readline)
valgrind: debug
	@if [ -f "readline_leaks.supp" ]; then \
		echo "Run with: valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=readline_leaks.supp ./minishell"; \
	else \
		echo "Run with: valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell"; \
		echo "⚠️  Considera crear readline_leaks.supp para suprimir leaks de readline"; \
	fi

# Ejecutar todas las herramientas de detección de memory leaks (completo)
check-memory:
	@./check_memory.sh

# Análisis rápido de memory leaks
check:
	@echo "🔍 Análisis rápido de memory leaks..."
	@echo ""
	@echo "1️⃣ AddressSanitizer:"
	@make asan > /dev/null 2>&1 && ASAN_OPTIONS=detect_leaks=1 timeout 3 ./minishell <<< "echo test" 2>&1 | grep -E "(leak|ERROR SUMMARY)" | head -5 || echo "  (sin errores detectados)"
	@echo ""
	@echo "2️⃣ Valgrind (resumen):"
	@make debug > /dev/null 2>&1
	@if [ -f "readline_leaks.supp" ]; then \
		valgrind --leak-check=full --show-leak-kinds=all --suppressions=readline_leaks.supp --quiet ./minishell <<< "echo test" 2>&1 | grep -E "(definitely|indirectly|possibly|still reachable|ERROR SUMMARY)" | head -5 || echo "  (sin leaks detectados)"; \
	else \
		valgrind --leak-check=full --show-leak-kinds=all --quiet ./minishell <<< "echo test" 2>&1 | grep -E "(definitely|indirectly|possibly|still reachable|ERROR SUMMARY)" | head -5 || echo "  (sin leaks detectados)"; \
	fi
	@echo ""
	@echo "✅ Análisis completado"
	@echo "💡 Para ver el reporte completo: make check-memory"

.PHONY: all clean fclean re asan lsan ubsan sanitize debug valgrind check-memory check