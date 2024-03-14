/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 18:59:37 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/14 04:45:29 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "system.h"
#include "MLX42/MLX42.h"
#include "so_long.h"
#include "system_internal.h"
#include "ft_stdio.h"
#include "ft_memlib.h"
#include "entity.h"
#include "map.h"
#include "context.h"
#include "ft_util.h"

int32_t	system_entities_init(t_game *game)
{
	if (ft_arraylist_transform2diarg(
			game->map.chart,
			(void *(*)(size_t, size_t, void*, void*)) entity_new,
		(void *) game,
		(t_vfun1) ft_nop) > 0)
		return (system_invalid("Failed to init entities"));
	ft_arraylist_transform2d(
		game->map.chart,
		(void *(*)(void*)) entity_remove_floor,
		(t_vfun1) entity_destroy);
	return (1);
}

int32_t	system_invalid(char *err_msg)
{
	ft_putstr_fd("ERROR:\n\t", 2);
	ft_putstr_fd(err_msg, 2);
	ft_putstr_fd("\n", 2);
	return (0);
}

int32_t	system_panic(t_game *game, t_exit_status exit_code, char *msg)
{
	game->exit_status = exit_code;
	if (game->mlx)
		mlx_close_window(game->mlx);
	if (exit_code == MLX_ERROR)
		ft_putstr_fd((char *) mlx_strerror(mlx_errno), 2);
	else if (msg)
	{
		ft_putstr_fd("ERROR:\n\t", 2);
		ft_putstr_fd(msg, 2);
		ft_putstr_fd("\n", 2);
	}
	return (exit_code);
}

void	system_quit_ok(void *param)
{
	t_game	*game;

	game = param;
	game->exit_status = OK;
	if (game->mlx)
		mlx_close_window(game->mlx);
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
		game->state.move_count++;
		ft_printf("movement count: %5d\n", game->state.move_count);
	}
	if (game->state.gst == HERO_MOVE)
		game->state.gst = HERO_UPDATE_POS;
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

void	system_game_end(t_game *game)
{
	t_drawable		*exit_drawable;
	int				is_enabled;

	system_hero_update_drawables_pos(game);
	exit_drawable = ft_arraylist_get(game->map.exit->drawables, 0);
	is_enabled = ((int) mlx_get_time()) % 3 == 0;
	exit_drawable->img->instances[exit_drawable->i].enabled = is_enabled;
}

t_screen_overflow	system_hero_screen_overflown(t_game *game)
{
	t_drawable			*dwb;
	mlx_instance_t		ist;
	t_screen_overflow	over;
	int					half_block;

	over = 0;
	dwb = ft_arraylist_get(game->map.hero->drawables, 0);
	ist = dwb->img->instances[0];
	half_block = game->ctx.block_size / 2;
	if (ist.x + half_block < 0)
		over |= SCREEN_OVERFLOW_LEFT;
	else if (ist.x + half_block > game->mlx->width)
		over |= SCREEN_OVERFLOW_RIGHT;
	if (ist.y + half_block < 0)
		over |= SCREEN_OVERFLOW_UP;
	else if (ist.y + half_block > game->mlx->height)
		over |= SCREEN_OVERFLOW_DOWN;
	return (over);
}

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
		game->ctx.overflow_y_offset += game->mlx->height;
	else if ((over & SCREEN_OVERFLOW_DOWN) == SCREEN_OVERFLOW_DOWN)
		game->ctx.overflow_y_offset -= game->mlx->height;
	else if ((over & SCREEN_OVERFLOW_LEFT) == SCREEN_OVERFLOW_LEFT)
		game->ctx.overflow_x_offset += game->mlx->width;
	else if ((over & SCREEN_OVERFLOW_RIGHT) == SCREEN_OVERFLOW_RIGHT)
		game->ctx.overflow_x_offset -= game->mlx->width;
	system_map_update_all_drawables_pos(game);
	game->state.gst = HERO_WAIT;
}

void	system_loop(t_game *game)
{
	if (mlx_is_key_down(game->mlx, MLX_KEY_ESCAPE))
	{
		game->exit_status = OK;
		mlx_close_window(game->mlx);
	}
	entity_hero_animate(game->map.hero);
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

void	system_hero_wait_input(t_game *game, t_entity *hero)
{
	t_direction	direction;
	t_moveable	*moveable;

	if (game->state.acc_time < 0.02)
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

void	system_hero_update_drawables_pos(t_game *game)
{
	system_update_drawables_pos(
		game->map.hero->drawables,
		game->map.hero->x,
		game->map.hero->y,
		game->ctx);
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

void system_resizefunc(int32_t width, int32_t height, t_game *game)
{
	(void) width;
	(void) height;
	system_map_update_pos(system_hero_screen_overflown(game), game);
}

int32_t	system_init(t_game *game, char *map_path)
{
	ft_bzero(game, sizeof(t_game));
	game->ctx.drawables = ft_arraylist_new((t_vfun1) ft_arraylist_destroy);
	game->ctx.textures = ft_arraylist_new((t_vfun1) mlx_delete_texture);
	if (!game->ctx.drawables || !game->ctx.textures)
		return (
			system_panic(game, MEMORY_ERROR, "No memory to init drawables"));
	if (!map_init(&game->map, map_path))
		return (system_panic(game, ERROR, "Failed to init map"));
	system_init_window_size(game);
	game->mlx = mlx_init(game->ctx.window_width,
			game->ctx.window_height,
			"So Long", true);
	if (!game->mlx)
		return (system_panic(game, MLX_ERROR, NULL));
	if (!context_init(&game->ctx, game->mlx))
		return (system_panic(game, ERROR, "Failed to init assets"));
	if (!system_entities_init(game))
		return (system_panic(game, ERROR, "Failed to init entities"));
	mlx_loop_hook(game->mlx, (t_vfun1) system_loop, game);
	mlx_close_hook(game->mlx, system_quit_ok, game);
	mlx_resize_hook(game->mlx, (mlx_resizefunc) system_resizefunc, game);
	game->state.gst = HERO_WAIT;
	return (OK);
}
