#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/05 06:51:38 by maurodri          #+#    #+#              #
#    Updated: 2024/03/13 23:43:18 by maurodri         ###   ########.fr        #
#                                                                              #
#******************************************************************************#

NAME := so_long
LIBMLX_DIR := ./lib/MLX42
LIBMLX := $(LIBMLX_DIR)/build/libmlx42.a
LIBFT_DIR := ./lib/libftx
LIBFT := $(LIBFT_DIR)/libft.a
SRCS := main.c \
		context.c \
		entity.c \
		entity_hero.c \
		entity_init.c \
		map.c \
		map_entity.c \
		map_hero.c \
		map_validate.c  \
		map_validate_path.c  \
		system.c
OBJ_DIR := ./obj/
OBJS := $(addprefix $(OBJ_DIR), $(patsubst %.c, %.o, $(SRCS)))
DEP_FLAGS := -MP -MD
DEP_FILES := $(patsubst %.o, %.d, $(OBJS))
INCLUDES := -I./ -I$(LIBMLX_DIR)/include -I$(LIBFT_DIR)/includes
VPATH := ./
CFLAGS := -g3 -fsanitize=address -fsanitize=undefined -Wall -Wextra #-Werror 
LFLAGS := -ldl -lglfw -pthread -lm
CC := cc

all: $(NAME)

$(NAME): $(OBJS) $(LIBMLX) $(LIBFT)
	$(CC) $(CFLAGS) $^ $(INCLUDES) $(LFLAGS) -o $@
	etags $$(find . -name '*.[ch]') --include '~/glibc/TAGS'
	echo $(DEP_FILES)

$(OBJS): $(OBJ_DIR)%.o : %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEP_FLAGS) -o $@ -c $< $(INCLUDES) 

$(OBJ_DIR):
	@mkdir -p $@

$(LIBMLX):
	$(MAKE) -C $(LIBMLX_DIR)/build  #--no-print-directory

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)  #--no-print-directory

.PHONY: all clean fclean re bonus

clean:
	rm -fr $(OBJ_DIR) **/*~ *~ **/.#*
	$(MAKE) -C $(LIBMLX_DIR)/build clean
	$(MAKE) -C $(LIBFT_DIR) fclean

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEP_FILES)
