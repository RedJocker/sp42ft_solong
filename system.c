/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 18:59:37 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/28 23:34:41 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "system.h"
#include "MLX42/MLX42.h"
#include "system_internal.h"
#include "ft_memlib.h"
#include "entity.h"
#include "context.h"
#include "ft_util.h"
#include "map.h"

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

static void	system_init_window_size(t_game *game)
{
	int	width;
	int	height;

	game->ctx.block_size = 32;
	game->ctx.window_width = WIDTH;
	game->ctx.window_height = HEIGHT;
	width = game->ctx.block_size * map_width(&game->map);
	height = game->ctx.block_size * map_height(&game->map);
	system_map_center(game, width, height);
}

void	system_resizefunc(int32_t width, int32_t height, t_game *game)
{
	t_game_state		state;
	t_screen_overflow	over;
	int					hero_pos[2];

	game->ctx.window_width = game->ctx.block_size * map_width(&game->map);
	game->ctx.window_height = game->ctx.block_size * map_height(&game->map);
	system_map_center(game, width, height);
	over = system_hero_screen_overflown(game);
	hero_pos[0] = (game->map.hero->y * game->ctx.block_size)
			+ game->ctx.overflow_y_offset + game->ctx.window_y_offset;
	hero_pos[1] = (game->map.hero->x * game->ctx.block_size)
			+ game->ctx.overflow_x_offset + game->ctx.window_x_offset;
	if ((over & SCREEN_OVERFLOW_DOWN) == SCREEN_OVERFLOW_DOWN)
		game->ctx.overflow_y_offset -= hero_pos[0] - height + 2 * (game->ctx.block_size);
	if ((over & SCREEN_OVERFLOW_RIGHT) == SCREEN_OVERFLOW_RIGHT)
		game->ctx.overflow_x_offset -= hero_pos[1] - width + 2 *(game->ctx.block_size);
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
	mlx_set_window_limit(game->mlx, game->ctx.block_size * 4,
						 game->ctx.block_size * 4, -1, -1);
	mlx_loop_hook(game->mlx, (t_vfun1) system_loop, game);
	mlx_close_hook(game->mlx, system_quit_ok, game);
	mlx_resize_hook(game->mlx, (mlx_resizefunc) system_resizefunc, game);
	game->state.gst = HERO_WAIT;
	return (OK);
}
