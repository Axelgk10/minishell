# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gguardam <gguardam@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/06 12:00:00 by gguardam          #+#    #+#              #
#    Updated: 2025/11/26 11:19:36 by gguardam         ###   ########.fr        #
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

.PHONY: all clean fclean re