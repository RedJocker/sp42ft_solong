/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_game_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 19:35:32 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/01 19:38:45 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "system_bonus.h"
#include "system_internal_bonus.h"
#include "entity_bonus.h"
#include "map_bonus.h"

void	system_game_over(t_game *game)
{
	int			pos[2];
	t_direction	dir;
	t_entity	*villain;
	int			i_j[2];
	t_drawable	*villain_drawable;

	game->state.gst = GAME_OVER;
	if (game->state.acc_time < 2.0)
		game->state.acc_time += game->mlx->delta_time;
	else
		mlx_close_window(game->mlx);
	dir = entity_hero_get_moveable(game->map.hero)->direction;
	pos[0] = game->map.hero->y + (dir == DOWN) - (dir == UP);
	pos[1] = game->map.hero->x + (dir == RIGHT) - (dir == LEFT);
	villain = map_entity_get(game, pos[1], pos[0]);
	i_j[1] = ((int)(game->state.acc_time * 5.0));
	if (i_j[1] > 5)
		i_j[1] = 5;
	i_j[0] = -1;
	while (++i_j[0] < 6)
	{
		villain_drawable = ft_arraylist_get(villain->drawables, i_j[0]);
		villain_drawable->img->instances[villain_drawable->i].enabled = (
				i_j[0] == i_j[1]);
	}
}

void	system_game_end(t_game *game)
{
	t_drawable		*exit_drawable;
	int				is_enabled;

	system_map_update_pos(system_hero_screen_overflown(game), game);
	game->state.gst = GAME_END;
	exit_drawable = ft_arraylist_get(game->map.exit->drawables, 0);
	is_enabled = ((int) mlx_get_time()) % 3 == 0;
	exit_drawable->img->instances[exit_drawable->i].enabled = is_enabled;
	if (game->state.acc_time < 3.0)
		game->state.acc_time += game->mlx->delta_time;
	else
		mlx_close_window(game->mlx);
}
