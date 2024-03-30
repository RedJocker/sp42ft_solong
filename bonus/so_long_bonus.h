/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long_bonus.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:08:53 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/30 04:08:53 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SO_LONG_H
# define SO_LONG_H

# include "collection/ft_arraylist.h"
# include <MLX42/MLX42.h>

# define WIDTH 800
# define HEIGHT 600

typedef void		(*t_vfun1) (void *);

typedef enum e_exit_status
{
	OK,
	ERROR,
	MLX_ERROR,
	MEMORY_ERROR,
	INVALID_MAP
}	t_exit_status;

typedef enum e_entity_type
{
	HERO = 0,
	ITEM,
	WALL,
	//ENEMY,
	FLOOR,
	EXIT,
}	t_entity_type;

typedef struct s_drawable
{
	int32_t		i;
	mlx_image_t	*img;
}	t_drawable;

typedef enum s_game_state
{
	HERO_WAIT,
	HERO_MOVE,
	HERO_UPDATE_POS,
	MAP_UPDATE_POS,
	EXIT_ENABLE,
	GAME_END,
}	t_game_state;

typedef enum s_screen_overflow
{
	SCREEN_OVERFLOW_UP = 1,
	SCREEN_OVERFLOW_RIGHT = 2,
	SCREEN_OVERFLOW_DOWN = 4,
	SCREEN_OVERFLOW_LEFT = 8
}	t_screen_overflow;

typedef struct s_state
{
	t_game_state	gst;
	double			acc_time;
	int				move_count;
	int				collectables_count;
}	t_state;

typedef struct s_context
{
	t_arraylist	drawables;
	t_arraylist	textures;
	int			block_size;
	int			window_height;
	int			window_width;
	int			window_y_offset;
	int			window_x_offset;
	int			overflow_y_offset;
	int			overflow_x_offset;
}	t_context;

typedef struct s_entity
{
	t_entity_type	type;
	int32_t			x;
	int32_t			y;
	t_arraylist		drawables;
	t_arraylist		components;
}	t_entity;

typedef struct s_map
{
	t_arraylist	chart;
	t_entity	*hero;
	t_entity	*exit;
}	t_map;

typedef struct s_game
{
	mlx_t			*mlx;
	t_map			map;
	t_context		ctx;
	t_state			state;
	t_exit_status	exit_status;
}	t_game;

typedef enum e_component_type
{
	MOVEABLE,
}	t_component_type;

typedef struct s_component
{
	t_component_type	type;
	void				*cmp;
}	t_component;

typedef enum e_direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT,
	IDLE
}	t_direction;

typedef t_direction	(*t_move_fun)(t_entity *entity, t_game *game);

typedef struct s_moveable
{
	t_direction	direction;
	t_move_fun	move;
}	t_moveable;

#endif
