# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/04 09:22:25 by axgimene          #+#    #+#              #
#    Updated: 2025/11/06 13:05:15 by axgimene         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -Iincludes -Ilibft
READLINE_FLAGS = -lreadline

SRC_DIR = src
OBJ_DIR = obj

SRC = src/main.c \
	src/pollo_token.c \
	src/parser_tokenizer0.c \
	src/parser_tokenizer_utils1.c \
	src/parser_builtins2.c \
	src/parser_parser_args2.c \
	src/parser_redirectionsPollitos3.c \
	src/parser_redirectionsPollo.c \
	src/parser_parser_main4.c \
	src/parser_parser_init5.c \
	src/parser_expander6.c \
	src/parser_expander_utils7.c \
	src/free_clean8.c \
	src/executor.c

OBJ = $(SRC:.c=.o)
OBJ := $(OBJ:src/%=obj/%)

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

# ============================================================================
# REGLAS
# ============================================================================

all: $(LIBFT) $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) $(LIBFT) $(READLINE_FLAGS) -o $(NAME)
	@echo "✓ Compilacion completada: $(NAME)"

obj/%.o: src/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@echo "✓ Compilado: $<"

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean
	@echo "✓ Limpieza completada"

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean
	@echo "✓ Limpieza total completada"

re: fclean all

.PHONY: all clean fclean re