/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_loop_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:06:32 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/01 18:58:01 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "collection/ft_arraylist.h"
#include "system_bonus.h"
#include "system_internal_bonus.h"
#include "entity_bonus.h"

static void	system_animate_entity(t_entity *entity, t_game *game)
{
	if (!entity)
		return ;
	if (entity->type == HERO)
		entity_hero_animate(entity, game);
	else if (entity ->type == VILAIN)
		entity_villain_animate(entity, game);
}

void	system_animate(t_game *game)
{
	ft_arraylist_foreach2darg(
		game->map.chart,
		(void (*)(void *, void *)) system_animate_entity,
		game);
}

void	system_loop(t_game *game)
{
	if (mlx_is_key_down(game->mlx, MLX_KEY_ESCAPE))
	{
		game->exit_status = OK;
		mlx_close_window(game->mlx);
	}
	if (game->state.gst == GAME_OVER)
	{
		system_game_over(game);
		return ;
	}
	system_animate(game);
	if (game->state.gst == HERO_WAIT)
		system_hero_wait_input(game, game->map.hero);
	else if (game->state.gst == HERO_MOVE)
		system_hero_move(game, game->map.hero);
	else if (game->state.gst == HERO_UPDATE_POS)
	{
		system_hero_update_drawables_pos(game);
		if (system_hero_screen_overflown(game) > 0)
			game->state.gst = MAP_UPDATE_POS;
		else
			game->state.gst = HERO_WAIT;
	}
	else if (game->state.gst == MAP_UPDATE_POS)
		system_map_update_pos(system_hero_screen_overflown(game), game);
	else if (game->state.gst == EXIT_ENABLE)
		system_exit_enable(game);
	else if (game->state.gst == GAME_END)
		system_game_end(game);
}
