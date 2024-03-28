/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 18:59:37 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/27 23:19:01 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "system.h"
#include "MLX42/MLX42.h"
#include "so_long.h"
#include "system_internal.h"
#include "ft_memlib.h"
#include "entity.h"
#include "context.h"
#include "ft_util.h"
#include "map.h"
#include "ft_stdio.h"

int32_t	system_entities_init(t_game *game)
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
}

static void	system_init_window_size(t_game *game)
{
	game->ctx.block_size = 32;
	game->ctx.window_width = game->ctx.block_size * map_width(&game->map);
	game->ctx.window_height = game->ctx.block_size * map_height(&game->map);
	if (game->ctx.window_width < WIDTH)
	{
		game->ctx.window_x_offset = (WIDTH - game->ctx.window_width) / 2;
		game->ctx.window_width = WIDTH;
	}
	else
		game->ctx.window_x_offset = 0;
	if (game->ctx.window_height < HEIGHT)
	{
		game->ctx.window_y_offset = (HEIGHT - game->ctx.window_height) / 2;
		game->ctx.window_height = HEIGHT;
	}
	else
		game->ctx.window_y_offset = 0;
}

void	system_resizefunc(int32_t width, int32_t height, t_game *game)
{
	t_game_state		state;
	t_screen_overflow	over;
	int					hero_pos[2];
	int					off[2];

	state = game->state.gst;
	int map_w = game->ctx.block_size * map_width(&game->map);
	int map_h = game->ctx.block_size * map_height(&game->map);
	if (map_w < width)
	{
		game->ctx.window_x_offset = (width - map_w) / 2;
		game->ctx.window_width = width;
	}
	else
		game->ctx.window_x_offset = 0;
	if (map_h < HEIGHT)
	{
		game->ctx.window_y_offset = (height - map_h) / 2;
		game->ctx.window_height = height;
	}
	else
		game->ctx.window_y_offset = 0;
	over = system_hero_screen_overflown(game);
	hero_pos[0] = (game->map.hero->y * game->ctx.block_size)
			+ game->ctx.overflow_y_offset + game->ctx.window_y_offset;
	hero_pos[1] = (game->map.hero->x * game->ctx.block_size)
			+ game->ctx.overflow_x_offset + game->ctx.window_x_offset;
	if ((over & SCREEN_OVERFLOW_DOWN) == SCREEN_OVERFLOW_DOWN)
	{
		off[0] = hero_pos[0] - height + 2 * (game->ctx.block_size);
		game->ctx.overflow_y_offset -= off[0];
	}
	if ((over & SCREEN_OVERFLOW_RIGHT) == SCREEN_OVERFLOW_RIGHT)
	{
		
		off[1] =  hero_pos[1] - width + 2 *(game->ctx.block_size);
		game->ctx.overflow_x_offset -= off[1];
	}
	game->ctx.window_height = height;
	game->ctx.window_width = width;
	system_map_update_all_drawables_pos(game);
	system_map_update_pos(system_hero_screen_overflown(game), game);
	game->state.gst = state;
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
	if (!system_entities_init(game))
		return (system_quit_panic(game, ERROR, "Failed to init entities"));
	mlx_set_window_limit(game->mlx, game->ctx.block_size * 4, game->ctx.block_size * 4, -1, -1);
	mlx_loop_hook(game->mlx, (t_vfun1) system_loop, game);
	mlx_close_hook(game->mlx, system_quit_ok, game);
	mlx_resize_hook(game->mlx, (mlx_resizefunc) system_resizefunc, game);
	game->state.gst = HERO_WAIT;
	return (OK);
}
