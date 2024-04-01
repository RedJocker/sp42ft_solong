#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/05 06:51:38 by maurodri          #+#    #+#              #
#    Updated: 2024/03/31 23:22:52 by maurodri         ###   ########.fr        #
#                                                                              #
#******************************************************************************#

NAME := so_long
LIBMLX_DIR := ./lib/MLX42
LIBMLX := $(LIBMLX_DIR)/build/libmlx42.a
LIBFT_DIR := ./lib/libftx
LIBFT := $(LIBFT_DIR)/libft.a
FILES := main.c \
		context.c \
		entity.c \
		entity_hero.c \
		entity_init.c \
		map.c \
		map_entity.c \
		map_hero.c \
		map_init.c \
		map_validate.c  \
		map_validate_path.c  \
		system.c \
		system_hero.c \
		system_quit.c \
		system_map.c \
		system_loop.c

BONUS_FILES := context_bonus.c \
		entity_bonus.c \
		entity_hero_bonus.c \
		entity_villain_bonus.c \
		entity_init_bonus.c \
		main_bonus.c \
		map_bonus.c \
		map_entity_bonus.c \
		map_hero_bonus.c \
		map_init_bonus.c \
		map_validate_bonus.c \
		map_validate_path_bonus.c \
		system_bonus.c \
		system_hero_bonus.c \
		system_loop_bonus.c \
		system_map_bonus.c \
		system_quit_bonus.c 

OBJ_DIR := ./obj/
MANDATORY_OBJS := $(addprefix $(OBJ_DIR), $(patsubst %.c, %.o, $(FILES)))
BONUS_OBJS := $(addprefix $(OBJ_DIR), $(patsubst %.c, %.o, $(BONUS_FILES)))
DEP_FLAGS := -MP -MD
INCLUDES := -I./ -I$(LIBMLX_DIR)/include -I$(LIBFT_DIR)/includes
VPATH := ./ ./mandatory ./bonus
CFLAGS := -g3 -fsanitize=address -fsanitize=undefined -Wall -Wextra #-Werror 
LFLAGS := -ldl -lglfw -pthread -lm
CC := cc

ifdef WITH_BONUS
	INCLUDES := -I./bonus -I$(LIBMLX_DIR)/include -I$(LIBFT_DIR)/includes
	CLEAR := $(MANDATORY_OBJS) $(patsubst %.o, %.d, $(MANDATORY_OBJS))
	OBJS := $(BONUS_OBJS)
	ETAGS_BASE := ./bonus
	DEP_FILES := $(patsubst %.o, %.d, $(OBJS))
else
	INCLUDES := -I./mandatory -I$(LIBMLX_DIR)/include -I$(LIBFT_DIR)/includes
	CLEAR := $(BONUS_OBJS) $(patsubst %.o, %.d, $(BONUS_OBJS))
	OBJS := $(MANDATORY_OBJS)
	ETAGS_BASE := ./mandatory
endif


all: $(NAME)

$(NAME): $(OBJS) $(LIBMLX) $(LIBFT)
	rm -f $(CLEAR)
	$(CC) $(CFLAGS) $^ $(INCLUDES) $(LFLAGS) -o $@
	etags $$(find $(ETAGS_BASE) -name '*.[ch]') --include '~/glibc/TAGS'
	echo $(DEP_FILES)

$(OBJS): $(OBJ_DIR)%.o : %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEP_FLAGS) -o $@ -c $< $(INCLUDES) 

$(OBJ_DIR):
	@mkdir -p $@

$(LIBMLX):
	cmake $(LIBMLX_DIR) -B $(LIBMLX_DIR)/build
	$(MAKE) -C $(LIBMLX_DIR)/build  #--no-print-directory

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)  #--no-print-directory

.PHONY: all clean fclean re bonus run run_invalid

bonus:
	$(MAKE) WITH_BONUS=1

clean:
	rm -fr $(OBJ_DIR) **/*~ *~ **/.#*
	$(MAKE) -C $(LIBMLX_DIR)/build clean
	$(MAKE) -C $(LIBFT_DIR) fclean

fclean: clean
	rm -f $(NAME)

re: fclean all

run_invalid: $(NAME)
	for map in $$(ls ./maps/invalid/*.ber); do \
		echo "file: $$map"; \
		./so_long $$map; \
	done

run: $(NAME)
	for map in $$(ls ./maps/*.ber); do \
		echo "file: $$map"; \
		./so_long $$map; \
	done


-include $(DEP_FILES)
