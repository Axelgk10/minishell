NAME = libft.a
CC = cc
CFLAGS = -Wall -Wextra -Werror

# Fuentes y objetos principales
SRC =   ft_isalpha.c ft_isdigit.c ft_isalnum.c \
        ft_isascii.c ft_isprint.c ft_toupper.c \
        ft_tolower.c ft_strlen.c ft_memset.c \
        ft_bzero.c ft_memcpy.c ft_memmove.c \
        ft_strlcpy.c ft_strlcat.c ft_strchr.c \
        ft_strrchr.c ft_strncmp.c ft_memchr.c \
        ft_memcmp.c ft_strnstr.c ft_atoi.c \
        ft_calloc.c ft_strdup.c ft_substr.c \
        ft_strjoin.c ft_strtrim.c ft_split.c \
        ft_itoa.c ft_strmapi.c ft_striteri.c \
        ft_putchar_fd.c ft_putstr_fd.c ft_putendl_fd.c \
        ft_putnbr_fd.c ft_strcmp.c ft_strjoin_char.c \
		printf/ft_print_char.c printf/ft_print_hex_lower.c \
		printf/ft_print_hex_upper.c printf/ft_print_nbr.c \
		printf/ft_print_percent.c printf/ft_print_ptr.c \
		printf/ft_print_str.c printf/ft_print_unsigned.c printf/ft_printf.c \

BONUS_SRC = ft_lstnew.c ft_lstadd_front.c ft_lstsize.c \
            ft_lstlast.c ft_lstadd_back.c ft_lstdelone.c \
            ft_lstclear.c ft_lstiter.c ft_lstmap.c 

# Archivos objeto
OBJ = $(SRC:.c=.o)
BONUS_OBJ = $(BONUS_SRC:.c=.o)

# Regla principal para crear la librería estática
all: $(NAME)

$(NAME): $(OBJ)
	@ar rcs $(NAME) $(OBJ)
	@echo "libft.a created!"

# Regla bonus para agregar funcionalidades adicionales
bonus: $(OBJ) $(BONUS_OBJ)printf/
	@ar rcs $(NAME) $(OBJ) $(BONUS_OBJ)
	@echo "libft.a with bonus created!"

# Regla para compilar los archivos .c a .o
%.o: %.c libft.h
	@$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar los archivos objeto
clean:
	@rm -f $(OBJ) $(BONUS_OBJ)
	@echo "Object files cleaned!"

# Regla para eliminar archivos .o y la librería
fclean: clean
	@rm -f $(NAME)
	@echo "libft.a removed!"

# Regla para limpiar y reconstruir
re: fclean all

# Especificamos que estas reglas no son archivos
.PHONY: all clean fclean re bonus

