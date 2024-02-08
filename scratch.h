/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scratch.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/05 06:58:58 by maurodri          #+#    #+#             */
/*   Updated: 2024/02/07 22:16:07 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCRATCH_H
# define SCRATCH_H

# include "collection/ft_arraylist.h"
# include <MLX42/MLX42.h>

# define WIDTH 800
# define HEIGHT 600

typedef enum e_exit_status
{
	OK,
	ERROR
}	t_exit_status;

typedef struct s_color
{
	int32_t	r;
	int32_t	g;
	int32_t	b;
	int32_t	a;
}	t_color;

typedef enum e_entity_type
{
	HERO,
	ITEM,
	WALL,
	ENEMY,
	EXIT,
}	t_entity_type;

typedef struct s_drawable
{
	int32_t		i;
	mlx_image_t	*img;
}	t_drawable;

typedef struct s_context
{
	t_arraylist	drawables;
	t_arraylist	textures;
}	t_context;

typedef struct s_game
{
	mlx_t			*mlx;
	t_context		ctx;
	t_exit_status	exit_status;
}	t_game;

typedef enum e_component_type
{
	MOVEABLE,
}	t_component_type;

typedef struct s_component
{
	t_component_type	type;
	void				*component;
}	t_component;

typedef enum e_direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
}	t_direction;

typedef struct s_entity
{
	t_entity_type	type;
	int32_t			x;
	int32_t			y;
	t_drawable		*drawables;
	int32_t			d_len;
	t_component		**components;
	int32_t			c_len;
}	t_entity;

typedef t_direction	(*t_move_fun)(t_entity *entity, t_game *game);

typedef struct s_movable
{
	t_direction	direction;
	t_move_fun	*move;
}	t_movable;

#endif
