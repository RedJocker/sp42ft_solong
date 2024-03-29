/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_entity.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 23:43:28 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/12 23:53:15 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "map.h"
#include "map_internal.h"

t_entity	*map_entity_get(t_game *game, int x, int y)
{
	return (ft_arraylist_get(ft_arraylist_get(game->map.chart, y), x));
}

void	map_entity_update_pos(t_entity *entity, t_drawable *dwb, t_game *game)
{
	if (!dwb)
		return ;
	dwb->img->instances[dwb->i].x = game->ctx.overflow_x_offset
		+ (game->ctx.block_size * entity->x) + game->ctx.window_x_offset;
	dwb->img->instances[dwb->i].y = game->ctx.overflow_y_offset
		+ (game->ctx.block_size * entity->y) + game->ctx.window_y_offset;
}
