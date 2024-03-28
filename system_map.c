/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 22:10:38 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/27 23:43:45 by maurodri         ###   ########.fr       */
/*   Updated: 2024/03/18 22:25:24 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "system.h"
#include "system_internal.h"
#include "map.h"

void	system_map_update_all_drawables_pos(t_game *game)
{
	size_t		chart_len[2];
	size_t		i[3];
	t_arraylist	map_row;
	t_entity	*entity;
	size_t		drawables_len;

	chart_len[0] = ft_arraylist_len(game->map.chart);
	i[0] = -1;
	while (++i[0] < chart_len[0])
	{
		map_row = ft_arraylist_get(game->map.chart, i[0]);
		chart_len[1] = ft_arraylist_len(map_row);
		i[1] = -1;
		while (++i[1] < chart_len[1])
		{
			entity = ft_arraylist_get(map_row, i[1]);
			if (!entity || !entity->drawables)
				continue ;
			drawables_len = ft_arraylist_len(entity->drawables);
			i[2] = -1;
			while (++i[2] < drawables_len)
				map_entity_update_pos(
					entity, ft_arraylist_get(entity->drawables, i[2]), game);
		}
	}
}

void	system_map_update_pos(t_screen_overflow over, t_game *game)
{
	if ((over & SCREEN_OVERFLOW_UP) == SCREEN_OVERFLOW_UP)
		game->ctx.overflow_y_offset += game->ctx.window_height;
	else if ((over & SCREEN_OVERFLOW_DOWN) == SCREEN_OVERFLOW_DOWN)
		game->ctx.overflow_y_offset -= game->ctx.window_height;
	else if ((over & SCREEN_OVERFLOW_LEFT) == SCREEN_OVERFLOW_LEFT)
		game->ctx.overflow_x_offset += game->ctx.window_width;
	else if ((over & SCREEN_OVERFLOW_RIGHT) == SCREEN_OVERFLOW_RIGHT)
		game->ctx.overflow_x_offset -= game->ctx.window_width;
	system_map_update_all_drawables_pos(game);
	game->state.gst = HERO_WAIT;
}

void	system_exit_enable(t_game *game)
{
	t_entity	*exit;
	t_drawable	*drawable;

	system_hero_update_drawables_pos(game);
	exit = game->map.exit;
	ft_arraylist_switch2d(game->map.chart, exit, exit->y, exit->x);
	drawable = ft_arraylist_get(exit->drawables, 0);
	drawable->img->instances[drawable->i].enabled = 1;
	map_entity_update_pos(exit, drawable, game);
	game->state.gst = HERO_UPDATE_POS;
}

void	system_update_drawables_pos(
	t_arraylist drawables, int x, int y, t_context ctx)
{
	size_t		i;
	size_t		len;
	t_drawable	*dwble;

	i = 0;
	len = ft_arraylist_len(drawables);
	while (i < len)
	{
		dwble = ft_arraylist_get(drawables, i);
		dwble->img->instances[dwble->i].x = (
				x * ctx.block_size
				+ ctx.window_x_offset + ctx.overflow_x_offset);
		dwble->img->instances[dwble->i].y = (
				y * ctx.block_size
				+ ctx.window_y_offset + ctx.overflow_y_offset);
		i++;
	}
}
