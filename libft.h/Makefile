# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: axgimene <axgimene@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/03 16:26:25 by gguardam          #+#    #+#              #
#    Updated: 2025/11/18 12:24:04 by axgimene         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a

CC = cc
CFLAGS = -Wall -Wextra -Werror

SOURCE = ft_atoi.c \
	 ft_atol.c \
         ft_isalpha.c \
         ft_memchr.c \
         ft_memset.c \
         ft_strlcpy.c \
         ft_strrchr.c \
         ft_bzero.c \
         ft_isdigit.c \
         ft_memcmp.c \
         ft_strchr.c \
         ft_strlen.c \
         ft_isspace.c \
         ft_tolower.c \
         ft_calloc.c \
         ft_isprint.c \
	 ft_isascii.c \
         ft_memcpy.c \
         ft_strdup.c \
		 ft_strcmp.c \
         ft_strncmp.c \
         ft_itoa.c \
         ft_putchar_fd.c \
         ft_putendl_fd.c \
         ft_putnbr_fd.c \
         ft_putstr_fd.c \
         ft_split.c \
         ft_striteri.c \
         ft_strjoin.c \
         ft_strmapi.c \
         ft_strtrim.c \
         ft_substr.c \
         ft_toupper.c \
         ft_isalnum.c \
         ft_memmove.c \
         ft_strlcat.c \
         ft_strnstr.c

BONUS_SOURCE = ft_lstsize_bonus.c \
               ft_lstlast_bonus.c \
               ft_lstdelone_bonus.c \
               ft_lstclear_bonus.c \
               ft_lstiter_bonus.c \
               ft_lstnew_bonus.c \
               ft_lstadd_back_bonus.c \
               ft_lstadd_front_bonus.c \
               ft_lstmap_bonus.c

OBJ = $(SOURCE:.c=.o)

BONUS_OBJ = $(BONUS_SOURCE:.c=.o)

INCLUDE = libft.h

AR = ar rcs
RM = rm -f

all: $(NAME)

$(NAME): $(OBJ) $(BONUS_OBJ)
	$(AR) $@ $^

%.o: %.c $(INCLUDE)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ) $(BONUS_OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

bonus: $(OBJ) $(BONUS_OBJ)
	$(AR) $(NAME) $(OBJ) $(BONUS_OBJ)

.PHONY: all clean fclean re bonus
