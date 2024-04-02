/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:06:30 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/01 22:09:16 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "system_bonus.h"
#include "system_internal_bonus.h"
#include "ft_memlib.h"
#include "entity_bonus.h"
#include "context_bonus.h"
#include "ft_util.h"
#include "map_bonus.h"

int32_t	system_init_entities(t_game *game)
{
	if (ft_arraylist_transform2diarg(
			game->map.chart,
			(void *(*)(size_t, size_t, void*, void*)) entity_new,
		(void *) game,
		(t_vfun1) ft_nop) > 0)
		return (system_quit_invalid("Failed to init entities"));
	ft_arraylist_transform2d(
		game->map.chart,
		(void *(*)(void*)) entity_remove_floor,
		(t_vfun1) entity_destroy);
	return (1);
}

static void	system_init_window_size(t_game *game)
{
	int	map_w;
	int	map_h;

	game->ctx.block_size = 32;
	game->ctx.window_width = WIDTH;
	game->ctx.window_height = HEIGHT;
	map_w = game->ctx.block_size * map_width(&game->map);
	map_h = game->ctx.block_size * map_width(&game->map);
	system_map_center(game, map_w, map_h);
	if (map_w > WIDTH)
		game->ctx.window_width = map_w;
	if (map_h > HEIGHT)
		game->ctx.window_height = map_h;
}

void	system_resizefunc(int32_t width, int32_t height, t_game *game)
{
	t_screen_overflow	over;
	int					h_pos[2];
	int					map_h_w[2];

	game->ctx.window_width = width;
	game->ctx.window_height = height;
	map_h_w[0] = game->ctx.block_size * map_height(&game->map);
	map_h_w[1] = game->ctx.block_size * map_width(&game->map);
	system_map_center(game, map_h_w[1], map_h_w[0]);
	h_pos[0] = (game->map.hero->y * game->ctx.block_size)
		+ game->ctx.overflow_y_offset + game->ctx.window_y_offset;
	h_pos[1] = (game->map.hero->x * game->ctx.block_size)
		+ game->ctx.overflow_x_offset + game->ctx.window_x_offset;
	over = system_hero_screen_overflown(game);
	if ((over & SCREEN_OVERFLOW_DOWN) == SCREEN_OVERFLOW_DOWN)
		game->ctx.overflow_y_offset -= (
				h_pos[0] - height + 2 * (game->ctx.block_size));
	if ((over & SCREEN_OVERFLOW_RIGHT) == SCREEN_OVERFLOW_RIGHT)
		game->ctx.overflow_x_offset -= (
				h_pos[1] - width + 2 *(game->ctx.block_size));
	system_map_update_all_drawables_pos(game);
}

int32_t	system_init(t_game *game, char *map_path)
{
	ft_bzero(game, sizeof(t_game));
	game->ctx.drawables = ft_arraylist_new((t_vfun1) ft_arraylist_destroy);
	game->ctx.textures = ft_arraylist_new((t_vfun1) mlx_delete_texture);
	if (!game->ctx.drawables || !game->ctx.textures)
		return (
			system_quit_panic(game, MEMORY_ERROR, "No memory for drawables"));
	if (!map_init(&game->map, map_path))
		return (system_quit_panic(game, ERROR, "Failed to init map"));
	system_init_window_size(game);
	game->mlx = mlx_init(game->ctx.window_width,
			game->ctx.window_height,
			"So Long", true);
	if (!game->mlx)
		return (system_quit_panic(game, MLX_ERROR, NULL));
	if (!context_init(&game->ctx, game->mlx))
		return (system_quit_panic(game, ERROR, "Failed to init assets"));
	if (!system_init_entities(game))
		return (system_quit_panic(game, ERROR, "Failed to init entities"));
	mlx_set_window_limit(game->mlx, game->ctx.block_size * 10,
		game->ctx.block_size * 10, -1, -1);
	mlx_loop_hook(game->mlx, (t_vfun1) system_loop, game);
	mlx_close_hook(game->mlx, system_quit_ok, game);
	mlx_resize_hook(game->mlx, (mlx_resizefunc) system_resizefunc, game);
	game->state.gst = HERO_WAIT;
	return (OK);
}
