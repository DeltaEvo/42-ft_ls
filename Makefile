# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/11/06 16:01:17 by dde-jesu          #+#    #+#              #
#    Updated: 2018/12/06 12:41:30 by dde-jesu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME=ft_ls
CFLAGS=-Wall -Werror -Wextra -Iincludes -Ilibft/include
CC=gcc

include src.mk

OBJS=$(SRCS:.c=.o)

all: $(NAME)

$(OBJS): Makefile src.mk includes/ft_ls.h

$(NAME): $(OBJS)
	$(MAKE) -C libft libft.a
	$(CC) -o $(NAME) $(OBJS) libft/libft.a

clean:
	$(MAKE) -C libft clean
	rm -f $(OBJS)

fclean: clean
	$(MAKE) -C libft fclean
	rm -rf $(NAME)

re: fclean $(NAME)


.PHONY: clean fclean re proto
