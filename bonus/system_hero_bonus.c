/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_hero_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:06:31 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/02 12:44:36 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_memlib.h"
#include "ft_string.h"
#include "system_internal_bonus.h"
#include "entity_bonus.h"
#include "map_bonus.h"

void	system_hero_move_count(t_game *game)
{
	char		countmsg[100];
	char		*cntstr;

	mlx_delete_image(game->mlx, game->ctx.count_img);
	cntstr = ft_itoa(++game->state.move_count);
	ft_bzero(countmsg, 100);
	ft_strlcpy(countmsg, "movement count:     ", 20);
	ft_strlcpy(countmsg + 16, cntstr, 84);
	game->ctx.count_img = mlx_put_string(game->mlx, countmsg, 10, 10);
	free(cntstr);
}

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
		system_hero_move_count(game);
	}
	else if (other->type == VILAIN)
		system_game_over(game);
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
