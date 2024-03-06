
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:28:12 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/06 01:29:21 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <MLX42/MLX42.h>
#include "collection/ft_arraylist.h"
#include "ft_memlib.h"
#include "get_next_line.h"
#include "scratch.h"
#include "ft_stdio.h"
#include "ft_util.h"
#include "ft_string.h"

int32_t system_invalid(char *err_msg)
{
	ft_printf("ERROR: %s\n", err_msg);
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
		ft_putstr_fd(msg, 2);
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

void	entity_hero_animate(t_entity *hero)
{
	t_drawable	*dwble;
	size_t		i;
	size_t		len;

	i = 0;
	len = ft_arraylist_len(hero->drawables);
	while (i < len)
	{
		dwble = ft_arraylist_get(hero->drawables, i);
		dwble->img->instances[dwble->i].enabled = (
				(size_t) mlx_get_time()) % 2 == i;
		i++;
	}
}

t_moveable	*entity_hero_get_moveable(t_entity *hero)
{
	t_component	*component;

	component = ft_arraylist_get(hero->components, 0);
	return (component->cmp);
}

t_entity	*map_get_entity(t_game *game, int x, int y)
{
	return (ft_arraylist_get(ft_arraylist_get(game->map.chart, y), x));
}

void	system_hero_wait_input(t_game *game, t_entity *hero)
{
	t_direction	direction;
	t_moveable	*moveable;

	if (game->state.acc_time < 0.1)
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

void	drawables_update_position(
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
			x * ctx.block_size + ctx.window_x_offset + ctx.overflow_x_offset);
		dwble->img->instances[dwble->i].y = (
			y * ctx.block_size + ctx.window_y_offset + ctx.overflow_y_offset);
		i++;
	}
}

void	system_hero_update_pos(t_game *game)
{
	ft_printf("x: %d y: %d\n", game->map.hero->x, game->map.hero->y);
	drawables_update_position(
		game->map.hero->drawables,
		game->map.hero->x,
		game->map.hero->y,
		game->ctx);
}


void	map_hero_collect_item(
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
	{
		game->state.gst = EXIT_ENABLE;
		system_hero_update_pos(game);
	}
}

void map_hero_move_to_floor(
	t_game *game, size_t new_pos[2], size_t old_pos[2])
{
	ft_arraylist_swap2d(game->map.chart, new_pos, old_pos);
}

void	map_hero_move_to_exit(
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
}

// pos[y, x]
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
	m->direction = IDLE;
	other = map_get_entity(game, new_pos[1], new_pos[0]);
	mlx_get_window_pos(game->mlx, test_pos + 1, test_pos);
	if (!other || other->type == ITEM || other->type == EXIT)
	{
		old_pos[1] = hero->x;
		old_pos[0] = hero->y;
		map_hero_switch_position(game, other, new_pos, old_pos);
		hero->x = (int) new_pos[1];
		hero->y = (int) new_pos[0];
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
	
	exit = game->map.exit;
	ft_arraylist_switch2d(game->map.chart, exit, exit->y, exit->x);
	drawable = ft_arraylist_get(exit->drawables, 0);
	drawable->img->instances[drawable->i].enabled = 1;
	game->state.gst = HERO_UPDATE_POS;
}

void	system_game_end(t_game *game)
{
	t_drawable		*exit_drawable;
	int				is_enabled;

	exit_drawable = ft_arraylist_get(game->map.exit->drawables, 0);
	is_enabled = ((int) mlx_get_time()) % 3 == 0;
	exit_drawable->img->instances[exit_drawable->i].enabled = is_enabled;
}

t_screen_overflow	system_hero_screen_overflown(t_game *game)
{
	t_drawable			*dwb;
	mlx_instance_t		ist;
	t_screen_overflow	over;

	over = 0;
	dwb = ft_arraylist_get(game->map.hero->drawables, 0);
	ist = dwb->img->instances[0];
	if (ist.x < 0)
		over |= SCREEN_OVERFLOW_LEFT;
	else if (ist.x + game->ctx.block_size > game->mlx->width)
		over |= SCREEN_OVERFLOW_RIGHT;
	if (ist.y < 0)
		over |= SCREEN_OVERFLOW_UP;
	else if (ist.y + game->ctx.block_size > game->mlx->height)
		over |= SCREEN_OVERFLOW_DOWN;
	return (over);
}

void map_update_entity_pos(t_entity *entity, t_drawable *dwb, t_game *game)
{
	if (!dwb)
		return ;
	dwb->img->instances[dwb->i].x = game->ctx.overflow_x_offset +
		(game->ctx.block_size * entity->x) + game->ctx.window_x_offset;
	dwb->img->instances[dwb->i].y = game->ctx.overflow_y_offset +
		(game->ctx.block_size * entity->y) + game->ctx.window_y_offset;
}

void system_map_update_all_drawables_pos(t_game *game)
{
	size_t		chart_len;
	size_t		row;
	size_t		col;
	t_arraylist map_row;
	t_entity   *entity;
	size_t		col_len;
	size_t		drawables_len;
	size_t		i;
	
	chart_len = ft_arraylist_len(game->map.chart);
	row = 0;
	while (row < chart_len)
	{
		map_row = ft_arraylist_get(game->map.chart, row);
		col_len = ft_arraylist_len(map_row);
		col = -1;
		
		while (++col < col_len)
		{
			entity = ft_arraylist_get(map_row, col);
			
			if (!entity || !entity->drawables)
				continue ;
			drawables_len = ft_arraylist_len(entity->drawables);
			i = 0;
			while (i < drawables_len)
			{
				ft_printf("row: %d, col: %d, i: %d\n", row, col, i);
				map_update_entity_pos(
					entity, ft_arraylist_get(entity->drawables, i), game);
				i++;
			}
		}
		row++;
	}
}

void system_map_update_pos(t_screen_overflow over, t_game *game)
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
		system_hero_update_pos(game);
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
	{
		system_hero_update_pos(game);
		system_game_end(game);
	}
}

int32_t	context_load_asset(
	t_context *ctx, char *path, mlx_t *mlx, t_entity_type type)
{
	t_drawable		*drawable;
	mlx_texture_t	*texture;
	t_arraylist		lst;

	drawable = malloc(sizeof(t_drawable));
	if (!drawable)
		return (system_invalid("failed to allocate memory for drawable"));
	lst = ft_arraylist_get(ctx->drawables, type);
	if (!lst)
		return (system_invalid("list of drawables wall null for some type"));
	lst = ft_arraylist_add(lst, drawable);
	if (!lst)
		return (system_invalid("list was null after adding drawable"));
	texture = mlx_load_png(path);
	if (!texture)
		return (system_invalid("texture was null after mlx_load_png"));
	ctx->textures = ft_arraylist_add(ctx->textures, texture);
	if (!ctx->textures)
		return (system_invalid("list of textures was null after adding"));
	drawable->img = mlx_texture_to_image(mlx, texture);
	if (!drawable->img)
		return (system_invalid("failed to create image from texture"));
	return (1);
}

int32_t	context_init_drawables_list(t_context *ctx)
{
	int32_t		i;
	t_arraylist	lst;

	i = 0;
	while (i <= EXIT)
	{
		lst = ft_arraylist_new(free);
		if (!lst)
			return (system_invalid("failed to initialize list of drawables"));
		ctx->drawables = ft_arraylist_add(ctx->drawables, lst);
		if (!ctx->drawables)
			return (system_invalid("failed to add list to ctx->drawables"));
		i++;
	}
	return (1);
}

int32_t	context_init(t_context *ctx, mlx_t *mlx)
{
	int32_t	i;
	int32_t	is_ok;

	i = 0;
	ctx->overflow_x_offset = 0;
	ctx->overflow_y_offset = 0;
	if (!context_init_drawables_list(ctx))
		return (system_invalid("failed to init ctx drawables"));
	while (1)
	{
		if (i == HERO)
		{
			is_ok = context_load_asset(ctx, "./assets/hero.png", mlx, HERO);
			is_ok &= context_load_asset(ctx, "./assets/hero2.png", mlx, HERO);
		}
		else if (i == ITEM)
			is_ok = context_load_asset(ctx, "./assets/item.png", mlx, ITEM);
		else if (i == WALL)
			is_ok = context_load_asset(ctx, "./assets/wall.png", mlx, WALL);
		else if (i == EXIT)
			is_ok = context_load_asset(ctx, "./assets/exit.png", mlx, EXIT);
		else if (i > EXIT)
			break ;
		if (!is_ok)
			return (system_invalid("failed loading some asset"));
		i++;
	}
	return (1);
}

t_entity_type	entity_type_by_ch(char ch)
{
	t_entity_type	type;

	if (ch == 'P')
		type = HERO;
	else if (ch == 'C')
		type = ITEM;
	else if (ch == '1')
		type = WALL;
	else if (ch == 'E')
		type = EXIT;
	else
		type = FLOOR;
	return (type);
}

int	entity_drawables_init(
	t_entity *entity, t_arraylist drawables_ctx, t_game *game)
{
	t_drawable	*drawable_ctx;
	t_drawable	*drawable_copy;
	size_t		i;
	int			x;
	int			y;

	i = 0;
	while (i < ft_arraylist_len(drawables_ctx))
	{
		drawable_ctx = ft_arraylist_get(
				drawables_ctx, i);
		drawable_copy = malloc(sizeof(t_drawable));
		if (!drawable_copy)
			return (system_invalid("failed allocing memory for drawable_copy"));
		entity->drawables = ft_arraylist_add(entity->drawables, drawable_copy);
		if (!entity->drawables)
			return (system_invalid("entity->drawables list was null after add"));
		drawable_copy->img = drawable_ctx->img;
		x = entity->x * game->ctx.block_size + game->ctx.window_x_offset;
		y = entity->y * game->ctx.block_size + game->ctx.window_y_offset;
		drawable_copy->i = mlx_image_to_window(
				game->mlx, drawable_copy->img, x, y);
		if (drawable_copy->i < 0)
			return (system_invalid("mlx_image_to_window failed"));
		drawable_copy->img->instances[drawable_copy->i].enabled = i == 0
			&& entity->type != EXIT;
		i++;
	}
	return (1);
}

t_direction	entity_hero_move(t_entity *entity, t_game *game)
{
	(void) entity;
	if (mlx_is_key_down(game->mlx, MLX_KEY_W))
		return (UP);
	else if (mlx_is_key_down(game->mlx, MLX_KEY_D))
		return (RIGHT);
	else if (mlx_is_key_down(game->mlx, MLX_KEY_S))
		return (DOWN);
	else if (mlx_is_key_down(game->mlx, MLX_KEY_A))
		return (LEFT);
	else
		return (IDLE);
}

void	entity_hero_components_destroy(t_component *component)
{
	if (component->type == MOVEABLE)
		free(component->cmp);
	free(component);
}

int	entity_hero_components_init(t_entity *entity)
{
	t_component	*component;
	t_moveable	*moveable;

	entity->components = ft_arraylist_new(
			(t_vfun1)entity_hero_components_destroy);
	if (!entity->components)
		return (system_invalid("failed to create components list for hero"));
	component = malloc(sizeof(t_component));
	if (!component)
		return (system_invalid("failed to alloc component for hero"));
	component->type = MOVEABLE;
	moveable = malloc(sizeof(t_moveable));
	if (!moveable)
		return (system_invalid("failed to alloc moveable for hero"));
	component->cmp = moveable;
	moveable->move = entity_hero_move;
	entity->components = ft_arraylist_add(entity->components, component);
	return (1);
}

int	entity_components_init(t_entity *entity, t_game *game)
{
	(void) game;
	if (entity->type == HERO)
		return (entity_hero_components_init(entity));
	else
		return (1);
}

t_entity	*entity_new(size_t y, size_t x, t_entity_type *type, t_game *game)
{
	t_entity		*entity;
	t_arraylist		drawables;

	entity = malloc(sizeof(t_entity));
	if (!entity)
		return (NULL);
	entity->type = *type;
	drawables = ft_arraylist_get(game->ctx.drawables, entity->type);
	entity->drawables = ft_arraylist_new(free);
	entity->x = x;
	entity->y = y;
	entity->components = NULL;
	if (!entity_drawables_init(entity, drawables, game))
		return (NULL);
	if (!entity_components_init(entity, game))
		return (NULL);
	if (entity->type == HERO)
		game->map.hero = entity;
	else if (entity->type == EXIT)
		game->map.exit = entity;
	else if (entity->type == ITEM)
		game->state.collectables_count++;
	return (entity);
}

void	entity_destroy(t_entity *entity)
{
	if (!entity || entity->type == EXIT)
		return ;
	if (entity->components)
		ft_arraylist_destroy(entity->components);
	if (entity->drawables)
		ft_arraylist_destroy(entity->drawables);
	free(entity);
}

void	entity_exit_destroy(t_entity *entity)
{
	if (!entity || entity->type != EXIT)
		return ;
	if (entity->components)
		ft_arraylist_destroy(entity->components);
	if (entity->drawables)
		ft_arraylist_destroy(entity->drawables);
	free(entity);
}

t_entity	*map_remove_floor(t_entity *entity)
{
	if (entity->type == FLOOR)
	{
		entity_destroy(entity);
		return (NULL);
	}
	else if (entity->type == EXIT)
		return (NULL);
	else
		return (entity);
}

int32_t	entities_init(t_game *game)
{
	if (ft_arraylist_transform2diarg(
			game->map.chart,
			(void *(*)(size_t, size_t, void*, void*)) entity_new,
		(void *) game,
		(t_vfun1) ft_nop) > 0)
		return (system_invalid("failed to init entities"));
	ft_arraylist_transform2d(
		game->map.chart,
		(void *(*)(void*)) map_remove_floor,
		(t_vfun1) entity_destroy);
	return (1);
}

void	*map_transform_string_line(void *s)
{
	char			ch;
	t_arraylist		lst_type;
	size_t			i;
	size_t			len;
	t_entity_type	*type;

	lst_type = ft_arraylist_new(free);
	if (!lst_type)
		return (NULL);
	i = 0;
	len = ft_strlen(((char *)s));
	while (i < len)
	{
		ch = ((char *)s)[i];
		if (ch == '\n')
			break ;
		type = malloc(sizeof(t_entity_type));
		*type = entity_type_by_ch(ch);
		if (!type)
			return (NULL);
		lst_type = ft_arraylist_add(lst_type, type);
		if (!lst_type)
			return (NULL);
		i++;
	}
	return (lst_type);
}

size_t	map_height(t_map *map)
{
	return (ft_arraylist_len(map->chart));
}

size_t	map_width(t_map *map)
{
	return (ft_arraylist_len(
				ft_arraylist_get(map->chart, 0)));
}

int32_t	map_init(t_map *map)
{
	int		fd;
	char	*str;

	map->chart = ft_arraylist_new((free));
	if (!map->chart)
		return (system_invalid("malloc fail\n"));
	fd = open("./maps/mauricio.ber", O_RDONLY);
	if (fd < 0)
		return (system_invalid("file open fail\n"));
	str = get_next_line(fd);
	while (str)
	{
		map->chart = ft_arraylist_add(map->chart, str);
		if (!map->chart)
			return (system_invalid("failed to add string to map->chart on init\n"));
		str = get_next_line(fd);
	}
	if (!map->chart)
		return (system_invalid("failed to init map->chart"));
	if (ft_arraylist_transform(map->chart, map_transform_string_line,
			(t_vfun1) ft_arraylist_destroy) > 0)
		return (system_invalid("failed to transform map"));
	return (1);
}

void	system_init_window_size(t_game *game)
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

int32_t	system_init(t_game *game)
{
	game->mlx = NULL;
	game->state.collectables_count = 0;
	game->ctx.drawables = ft_arraylist_new((t_vfun1) ft_arraylist_destroy);
	game->ctx.textures = ft_arraylist_new((t_vfun1) mlx_delete_texture);
	if (!game->ctx.drawables || !game->ctx.textures)
		return (
			system_panic(game, MEMORY_ERROR, "No memory to init drawables"));
	if (!map_init(&game->map))
		return (system_panic(game, ERROR, "Failed to init map"));
	system_init_window_size(game);
	game->mlx = mlx_init(game->ctx.window_width,
						 game->ctx.window_height,
						 "So Long", true);
	if (!game->mlx)
		return (system_panic(game, MLX_ERROR, NULL));
	if (!context_init(&game->ctx, game->mlx))
		return (system_panic(game, ERROR, "Failed to init assets"));
	if (!entities_init(game))
		return (system_panic(game, ERROR, "Failed to init entities"));
	mlx_loop_hook(game->mlx, (t_vfun1) system_loop, game);
	mlx_close_hook(game->mlx, system_quit_ok, game);
	game->state.gst = HERO_WAIT;
	game->state.move_count = 0;
	return (OK);
}

int32_t	main(void)
{
	t_game	game;

	if (system_init(&game) == OK)
		mlx_loop(game.mlx);
	ft_arraylist_destroy(game.ctx.drawables);
	ft_arraylist_destroy(game.ctx.textures);
	ft_arraylist_destroy(game.map.chart);
	if (game.mlx)
		mlx_terminate(game.mlx);
	entity_exit_destroy(game.map.exit);
	return (game.exit_status);
}
