/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entity_villain_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 23:23:45 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/01 22:09:18 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "entity_bonus.h"
#include "system_bonus.h"

void	entity_villain_components_destroy(t_component *component)
{
	if (component->type == ANIMATED)
		free(component->cmp);
	free(component);
}

int	entity_villain_init_component(
	t_entity *entity, t_component_type c_type, size_t type_size)
{
	t_component			*component;
	t_components_union	*cmps;

	component = malloc(sizeof(t_component));
	if (!component)
		return (system_quit_invalid("Failed to alloc component for villain"));
	component->type = c_type;
	cmps = malloc(type_size);
	if (!cmps)
		return (system_quit_invalid("Failed alloc component for villain"));
	component->cmp = cmps;
	if (c_type == ANIMATED)
		cmps->animated.animate = entity_villain_animate;
	entity->components = ft_arraylist_add(entity->components, component);
	return (1);
}

int	entity_villain_init_components(t_entity *entity)
{
	t_component_type	c_type[1];	
	int					i;
	size_t				type_size[1];

	entity->components = ft_arraylist_new(
			(t_vfun1)entity_villain_components_destroy);
	if (!entity->components)
		return (system_quit_invalid("Failed to create "
				"components list for villain"));
	i = -1;
	c_type[0] = ANIMATED;
	type_size[0] = sizeof(t_animated);
	while (++i < 1)
		entity_villain_init_component(entity, c_type[i], type_size[i]);
	return (1);
}

void	entity_villain_animate(t_entity *villain, t_game *game)
{
	t_drawable	*dwble;
	size_t		i;
	size_t		len;

	(void) game;
	i = 0;
	len = ft_arraylist_len(villain->drawables);
	while (i < len)
	{
		dwble = ft_arraylist_get(villain->drawables, i);
		dwble->img->instances[dwble->i].enabled = (
				(size_t) mlx_get_time() % 2) == i;
		i++;
	}
}
