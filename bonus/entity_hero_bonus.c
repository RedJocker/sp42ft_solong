/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entity_hero_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:06:20 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/03 19:59:50 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_memlib.h"
#include "system_bonus.h"
#include "entity_bonus.h"

t_direction	entity_hero_move(t_entity *entity, t_game *game)
{
	(void) entity;
	if (mlx_is_key_down(game->mlx, MLX_KEY_W))
		return (UP);
	else if (mlx_is_key_down(game->mlx, MLX_KEY_D))
		return (RIGHT);
	else if (mlx_is_key_down(game->mlx, MLX_KEY_S))
		return (DOWN);
	else if (mlx_is_key_down(game->mlx, MLX_KEY_A))
		return (LEFT);
	else
		return (IDLE);
}

void	entity_hero_components_destroy(t_component *component)
{
	if (component->type == MOVEABLE)
		free(component->cmp);
	else if (component->type == ANIMATED)
		free(component->cmp);
	free(component);
}

int	entity_hero_init_component(
	t_entity *entity, t_component_type c_type, size_t type_size)
{
	t_component			*component;
	t_components_union	*cmps;

	component = malloc(sizeof(t_component));
	if (!component)
		return (system_quit_invalid("Failed to alloc component for hero"));
	ft_bzero(component, sizeof(t_component));
	component->type = c_type;
	cmps = malloc(type_size);
	ft_bzero(cmps, type_size);
	if (!cmps)
		return (system_quit_invalid("Failed alloc component for hero"));
	component->cmp = cmps;
	if (c_type == MOVEABLE)
		cmps->moveable.move = entity_hero_move;
	else if (c_type == ANIMATED)
		cmps->animated.animate = entity_hero_animate;
	entity->components = ft_arraylist_add(entity->components, component);
	return (1);
}

int	entity_hero_init_components(t_entity *entity)
{
	t_component_type	c_type[2];	
	int					i;
	size_t				type_size[2];

	entity->components = ft_arraylist_new(
			(t_vfun1)entity_hero_components_destroy);
	if (!entity->components)
		return (system_quit_invalid("Failed to create "
				"components list for hero"));
	i = -1;
	c_type[0] = MOVEABLE;
	type_size[0] = sizeof(t_moveable);
	c_type[1] = ANIMATED;
	type_size[1] = sizeof(t_animated);
	while (++i < 2)
		entity_hero_init_component(entity, c_type[i], type_size[i]);
	return (1);
}

void	entity_hero_animate(t_entity *hero, t_game *game)
{
	t_drawable	*dwble;
	size_t		i;
	size_t		len;
	t_moveable	*m;
	size_t		offset;

	m = entity_hero_get_moveable(game->map.hero);
	if (m->direction == UP)
		offset = 6;
	else if (m->direction == DOWN)
		offset = 2;
	else if (m->direction == LEFT)
		offset = 4;
	else
		offset = 0;
	i = 0;
	len = ft_arraylist_len(hero->drawables);
	while (i < len)
	{
		dwble = ft_arraylist_get(hero->drawables, i);
		dwble->img->instances[dwble->i].enabled = (
				(size_t) mlx_get_time() % 2) + offset == i;
		i++;
	}
}
