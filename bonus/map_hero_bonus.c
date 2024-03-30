/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_hero_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:06:24 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/30 04:21:49 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "map_bonus.h"
#include "map_internal_bonus.h"

static void	map_hero_collect_item(
	t_game *game, t_entity *item, size_t hero_pos[2], size_t item_pos[2])
{
	t_entity	*hero;
	t_drawable	*itm_dwble;

	hero = game->map.hero;
	itm_dwble = ft_arraylist_get(item->drawables, 0);
	itm_dwble->img->instances[itm_dwble->i].enabled = false;
	ft_arraylist_replace2d(game->map.chart, hero, item_pos[0], item_pos[1]);
	ft_arraylist_switch2d(game->map.chart, NULL, hero_pos[0], hero_pos[1]);
	game->state.collectables_count--;
	if (game->state.collectables_count == 0)
		game->state.gst = EXIT_ENABLE;
}

static void	map_hero_move_to_floor(
	t_game *game, size_t new_pos[2], size_t old_pos[2])
{
	ft_arraylist_swap2d(game->map.chart, new_pos, old_pos);
}

static void	map_hero_move_to_exit(
	t_game *game)
{
	game->state.gst = GAME_END;
}

void	map_hero_switch_position(
	t_game *game, t_entity *other, size_t new_pos[2], size_t old_pos[2])
{	
	if (!other)
		map_hero_move_to_floor(game, new_pos, old_pos);
	else if (other->type == ITEM)
		map_hero_collect_item(game, other, old_pos, new_pos);
	else if (other->type == EXIT)
		map_hero_move_to_exit(game);
	game->map.hero->x = (int) new_pos[1];
	game->map.hero->y = (int) new_pos[0];
}
