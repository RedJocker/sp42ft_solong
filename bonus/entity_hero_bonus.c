/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entity_hero_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:06:20 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/30 04:20:23 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "system_bonus.h"
#include "entity_bonus.h"
#include "ft_stdio.h"

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
	free(component);
}

int	entity_hero_init_components(t_entity *entity)
{
	t_component	*component;
	t_moveable	*moveable;

	entity->components = ft_arraylist_new(
			(t_vfun1)entity_hero_components_destroy);
	if (!entity->components)
		return (system_quit_invalid("Failed to create "
				"components list for hero"));
	component = malloc(sizeof(t_component));
	if (!component)
		return (system_quit_invalid("Failed to alloc component for hero"));
	component->type = MOVEABLE;
	moveable = malloc(sizeof(t_moveable));
	if (!moveable)
		return (system_quit_invalid("Failed to alloc moveable for hero"));
	component->cmp = moveable;
	moveable->move = entity_hero_move;
	entity->components = ft_arraylist_add(entity->components, component);
	return (1);
}

void	entity_hero_animate(t_entity *hero)
{
	t_drawable	*dwble;
	size_t		i;
	size_t		len;
	t_moveable	*m;
	size_t		offset;

	m = entity_hero_get_moveable(hero);
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

t_moveable	*entity_hero_get_moveable(t_entity *hero)
{
	t_component	*component;

	component = ft_arraylist_get(hero->components, 0);
	return (component->cmp);
}
