#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/05 06:51:38 by maurodri          #+#    #+#              #
#    Updated: 2024/02/08 01:56:22 by maurodri         ###   ########.fr        #
#                                                                              #
#******************************************************************************#


# cc main.c ... libmlx42.a -Iinclude -ldl -lglfw -pthread -lm

NAME := ft_so_long
LIBMLX_DIR := ./lib/MLX42
LIBMLX := $(LIBMLX_DIR)/build/libmlx42.a
LIBFT_DIR := ./lib/libftx
LIBFT := $(LIBFT_DIR)/libft.a
SRCS := main.c
OBJ_DIR := ./obj/
OBJS := $(addprefix $(OBJ_DIR), $(patsubst %.c, %.o, $(SRCS)))
DEP_FLAGS := -MP -MD
DEP_FILES := $(addsuffix .d, $(OBJS))
INCLUDES := -I./ -I$(LIBMLX_DIR)/include -I$(LIBFT_DIR)/includes
CFLAGS := -g3 -fsanitize=address -fsanitize=undefined -Wall -Wextra #-Werror 
LFLAGS := -ldl -lglfw -pthread -lm
CC := cc

all: $(NAME)

$(NAME): $(OBJS) $(LIBMLX) $(LIBFT)
	$(CC) $(CFLAGS) $^ $(INCLUDES) $(LFLAGS) -o $@
	etags $$(find . -name '*.[ch]') --include '~/glibc/TAGS'

$(OBJS): $(OBJ_DIR)%.o : %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS)  -o $@ -c $< $(INCLUDES) 

$(OBJ_DIR):
	@mkdir -p $@

$(LIBMLX):
	$(MAKE) -C $(LIBMLX_DIR)/build  #--no-print-directory

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)  #--no-print-directory

.PHONY: all clean fclean re bonus

clean:
	rm -fr $(OBJS_DIR) **/*~ *~ **/.#*
	$(MAKE) -C $(LIBMLX_DIR)/build clean
	$(MAKE) -C $(LIBFT_DIR) fclean

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEP_FILES)
