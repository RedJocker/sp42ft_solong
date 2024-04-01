/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entity_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:06:18 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/01 01:49:00 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "entity_bonus.h"
#include "system_bonus.h"

t_entity_type	entity_type_by_ch(char ch)
{
	t_entity_type	type;

	if (ch == 'p')
		type = HERO;
	else if (ch == 'c')
		type = ITEM;
	else if (ch == '1')
		type = WALL;
	else if (ch == 'e')
		type = EXIT;
	else if (ch == 'V')
		type = VILAIN;
	else
		type = FLOOR;
	return (type);
}

t_entity	*entity_remove_floor(t_entity *entity)
{
	if (entity->type == FLOOR)
	{
		entity_destroy(entity);
		return (NULL);
	}
	else if (entity->type == EXIT)
		return (NULL);
	else
		return (entity);
}

t_entity	*entity_new(size_t y, size_t x, t_entity_type *type, t_game *game)
{
	t_entity		*entity;
	t_arraylist		drawables;

	entity = malloc(sizeof(t_entity));
	if (!entity)
		return (NULL);
	entity->type = *type;
	drawables = ft_arraylist_get(game->ctx.drawables, entity->type);
	entity->drawables = ft_arraylist_new(free);
	entity->x = x;
	entity->y = y;
	entity->components = NULL;
	if (!entity_init_drawables(entity, drawables, game))
		return (NULL);
	if (!entity_init_components(entity, game))
		return (NULL);
	if (entity->type == HERO)
		game->map.hero = entity;
	else if (entity->type == EXIT)
		game->map.exit = entity;
	else if (entity->type == ITEM)
		game->state.collectables_count++;
	return (entity);
}

void	entity_destroy(t_entity *entity)
{
	if (!entity || entity->type == EXIT)
		return ;
	if (entity->components)
		ft_arraylist_destroy(entity->components);
	if (entity->drawables)
		ft_arraylist_destroy(entity->drawables);
	free(entity);
}

void	entity_exit_destroy(t_entity *entity)
{
	if (!entity || entity->type != EXIT)
		return ;
	if (entity->components)
		ft_arraylist_destroy(entity->components);
	if (entity->drawables)
		ft_arraylist_destroy(entity->drawables);
	free(entity);
}
