/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_hero.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 20:14:03 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/02 12:49:01 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "system.h"
#include "system_internal.h"
#include "entity.h"
#include "map.h"
#include "ft_stdio.h"

void	system_hero_move(t_game *game, t_entity *hero)
{
	size_t		new_pos[2];
	size_t		old_pos[2];
	int32_t		test_pos[2];
	t_moveable	*m;
	t_entity	*other;

	m = entity_hero_get_moveable(hero);
	new_pos[1] = hero->x + (m->direction == RIGHT) - (m->direction == LEFT);
	new_pos[0] = hero->y + (m->direction == DOWN) - (m->direction == UP);
	other = map_entity_get(game, new_pos[1], new_pos[0]);
	mlx_get_window_pos(game->mlx, test_pos + 1, test_pos);
	if (!other || other->type == ITEM || other->type == EXIT)
	{
		old_pos[1] = hero->x;
		old_pos[0] = hero->y;
		map_hero_switch_position(game, other, new_pos, old_pos);
		game->state.move_count++;
		ft_printf("movement count: %5d\n", game->state.move_count);
	}
	if (game->state.gst == HERO_MOVE)
		game->state.gst = HERO_UPDATE_POS;
}

t_screen_overflow	system_hero_screen_overflown(t_game *game)
{
	int					h_pos[2];
	t_screen_overflow	over;
	int					half_bk;

	h_pos[0] = (game->map.hero->y * game->ctx.block_size)
		+ game->ctx.overflow_y_offset + game->ctx.window_y_offset;
	h_pos[1] = (game->map.hero->x * game->ctx.block_size)
		+ game->ctx.overflow_x_offset + game->ctx.window_x_offset;
	over = 0;
	half_bk = game->ctx.block_size / 2;
	if (h_pos[1] + half_bk < 0 + game->ctx.block_size)
		over |= SCREEN_OVERFLOW_LEFT;
	else if (h_pos[1] + half_bk > game->ctx.window_width - game->ctx.block_size)
		over |= SCREEN_OVERFLOW_RIGHT;
	if (h_pos[0] + half_bk < 0 + game->ctx.block_size)
		over |= SCREEN_OVERFLOW_UP;
	else if (
		h_pos[0] + half_bk > game->ctx.window_height - game->ctx.block_size)
		over |= SCREEN_OVERFLOW_DOWN;
	return (over);
}

void	system_hero_wait_input(t_game *game, t_entity *hero)
{
	t_direction	direction;
	t_moveable	*moveable;

	if (game->state.acc_time < 0.075)
	{
		game->state.acc_time += game->mlx->delta_time;
	}
	else
	{
		moveable = entity_hero_get_moveable(hero);
		direction = moveable->move(hero, game);
		if (direction != IDLE)
		{
			game->state.gst = HERO_MOVE;
			game->state.acc_time = 0;
			moveable->direction = direction;
		}
	}
}

void	system_hero_update_drawables_pos(t_game *game)
{
	system_update_drawables_pos(
		game->map.hero->drawables,
		game->map.hero->x,
		game->map.hero->y,
		game->ctx);
}
