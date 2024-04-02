/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_animate.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 19:31:52 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/02 01:35:58 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "entity_bonus.h"

static void	system_animate_entity(t_entity *entity, t_game *game)
{
	if (!entity)
		return ;
	if (entity->type == HERO)
		entity_hero_animate(entity, game);
	else if (entity->type == VILAIN)
		entity_villain_animate(entity, game);
}

void	system_animate(t_game *game)
{
	ft_arraylist_foreacharg(
		game->ctx.animated,
		(void (*)(void *, void *)) system_animate_entity,
		game);
}
