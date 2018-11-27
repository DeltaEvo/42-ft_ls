# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/11/06 16:01:17 by dde-jesu          #+#    #+#              #
#    Updated: 2018/11/26 10:03:07 by dde-jesu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME=ft_ls
CFLAGS=-Wall -Werror -Wextra -Iincludes -Ilibft
CC=gcc

include src.mk

OBJS=$(SRCS:.c=.o)

all: $(NAME)

$(OBJS): Makefile src.mk

libft/libft.a:
	make -C libft libft.a

$(NAME): $(OBJS) libft/libft.a
	$(CC) -o $(NAME) $(OBJS) libft/libft.a
clean:
	rm -f $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean $(NAME)


.PHONY: clean fclean re proto
