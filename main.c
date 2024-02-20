/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:28:12 by maurodri          #+#    #+#             */
/*   Updated: 2024/02/19 23:09:27 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <MLX42/MLX42.h>
#include <stddef.h>
#include <stdint.h>
#include "collection/ft_arraylist.h"
#include "ft_memlib.h"
#include "get_next_line.h"
#include "scratch.h"
#include "ft_stdio.h"
#include "ft_util.h"
#include "ft_string.h"

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

void	system_exit_ok(void *param)
{
	t_game	*game;

	game = param;
	game->exit_status = OK;
	if (game->mlx)
		mlx_close_window(game->mlx);
}

int	colision_check(int32_t x, int32_t y, t_entity *e)
{
	return (x == e->x && y == e->y);
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
		dwble->img->instances[dwble->i].enabled = ((size_t) mlx_get_time()) % 2 == i;
		i++;
	}
}

t_moveable *entity_hero_get_moveable(t_entity *hero)
{
	t_component	*component;

	component = ft_arraylist_get(hero->components, 0);
	return (component->cmp);
}

t_entity	*map_get_entity(t_game *game, int x, int y)
{
	return ft_arraylist_get(ft_arraylist_get(game->map.chart, y),x);
}

void	system_hero_wait_input(t_game *game, t_entity *hero)
{
	t_direction direction;
	t_moveable *moveable;

	if (game->state.acc_time < 0.18)
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

void drawables_update_position(t_arraylist drawables, int x, int y)
{
	size_t		i;
	size_t		len;
	t_drawable	*dwble;

	i = 0;
	len = ft_arraylist_len(drawables);
	while (i < len)
	{
		dwble = ft_arraylist_get(drawables, i);
		dwble->img->instances[dwble->i].x = 32 * x;
		dwble->img->instances[dwble->i].y = 32 * y;
		i++;
	}
}

void	entity_hero_collect_item(
	t_game *game, t_entity *item, size_t hero_pos[2], size_t item_pos[2])
{
	t_entity	*hero;
	t_drawable	*itm_dwble;
	hero = game->map.hero;
	
	itm_dwble = ft_arraylist_get(item->drawables, 0);
	ft_printf("itm_dwble->i %d\n", itm_dwble->i);
	itm_dwble->img->instances[itm_dwble->i].enabled = false;
	ft_arraylist_replace2d(game->map.chart, hero, item_pos[0], item_pos[1]);
	ft_arraylist_switch2d(game->map.chart, NULL, hero_pos[0], hero_pos[1]);
	game->state.collectables_count--;
	ft_printf("collectables_count: %10d\n", game->state.collectables_count);
}

// pos[y, x]
void	system_hero_move(t_game *game, t_entity *hero)
{
	size_t		new_pos[2];
	size_t		old_pos[2];
	t_moveable	*m;
	t_entity	*other;

	m = entity_hero_get_moveable(hero);
	new_pos[1] = hero->x + (m->direction == RIGHT) - (m->direction == LEFT);
	new_pos[0] = hero->y + (m->direction == DOWN) - (m->direction == UP);
	m->direction = IDLE;
	other = map_get_entity(game, new_pos[1], new_pos[0]);
	if (!other || other->type == ITEM)
	{
		old_pos[1] = hero->x;
		old_pos[0] = hero->y;
		if (!other)
			ft_arraylist_swap2d(game->map.chart, new_pos, old_pos);
		else
			entity_hero_collect_item(game, other, old_pos, new_pos);
		hero->x = (int) new_pos[1];
		hero->y = (int) new_pos[0];
		drawables_update_position(hero->drawables, hero->x, hero->y);
		game->state.move_count++;
		ft_printf("movement count: %5d\n", game->state.move_count);
	}
	game->state.gst = HERO_WAIT;
}

void	system_loop(t_game *game)
{
	t_entity *e;

	if (mlx_is_key_down(game->mlx, MLX_KEY_ESCAPE))
	{
		game->exit_status = OK;
		mlx_close_window(game->mlx);
	}
	e = game->map.hero;
	entity_hero_animate(e);
	if (game->state.gst == HERO_WAIT)
		system_hero_wait_input(game, e);
	else if (game->state.gst == HERO_MOVE)
 		system_hero_move(game, e);
}

int32_t	context_load_asset(
	t_context *ctx, char *path, mlx_t *mlx, t_entity_type type)
{
	t_drawable		*drawable;
	mlx_texture_t	*texture;
	t_arraylist		lst;

	drawable = malloc(sizeof(t_drawable));
	if (!drawable)
		return (0);
	lst = ft_arraylist_get(ctx->drawables, type);
	if (!lst)
		return (0);
	lst = ft_arraylist_add(lst, drawable);
	if (!lst)
		return (0);
	texture = mlx_load_png(path);
	if (!texture)
		return (0);
	ctx->textures = ft_arraylist_add(ctx->textures, texture);
	if (!ctx->textures)
		return (0);
	drawable->img = mlx_texture_to_image(mlx, texture);
	if (!drawable->img)
		return (0);
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
			return (0);
		ctx->drawables = ft_arraylist_add(ctx->drawables, lst);
		if (!ctx->drawables)
			return (0);
		i++;
	}
	return (1);
}

int32_t	context_init(t_context *ctx, mlx_t *mlx)
{
	int32_t	i;
	int32_t	is_ok;

	i = 0;
	if (!context_init_drawables_list(ctx))
		return (0);
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
		else
			break ;
		if (!is_ok)
			return (0);
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

int		entity_drawables_init(t_entity *entity, t_arraylist drawables_ctx, t_game *game)
{
	t_drawable	*drawable_ctx;
	t_drawable	*drawable_copy;
	size_t		i;

	i = 0;
	while (i < ft_arraylist_len(drawables_ctx))
	{
		drawable_ctx = ft_arraylist_get(
				drawables_ctx, i);
		drawable_copy = malloc(sizeof(t_drawable));
		if (!drawable_copy)
			return (0);
		entity->drawables = ft_arraylist_add(entity->drawables, drawable_copy);
		drawable_copy->img = drawable_ctx->img;
		drawable_copy->i = mlx_image_to_window(
				game->mlx, drawable_copy->img, entity->x * 32, entity->y * 32);
		if (entity->type == ITEM)
			ft_printf("======%d\n", drawable_copy->i);
		if (drawable_copy->i < 0)
			return (0);
		drawable_copy->img->instances[drawable_copy->i].enabled = i == 0;
		i++;
	}
	return (1);
}

t_direction entity_hero_move(t_entity *entity, t_game *game)
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
	t_component *component;
	t_moveable	*moveable;

	entity->components = ft_arraylist_new((t_vfun1)entity_hero_components_destroy);
	if (!entity->components)
		return (0);
	component = malloc(sizeof(t_component));
	if (!component)
		return (0);
	component->type = MOVEABLE;
	moveable = malloc(sizeof(t_moveable));
	if (!moveable)
		return (0);
	component->cmp = moveable;
	moveable->move = entity_hero_move;
	entity->components = ft_arraylist_add(entity->components, component);
	return (1);
}

int	entity_components_init(t_entity *entity, t_game *game)
{
	if (entity->type == HERO)
		return (entity_hero_components_init(entity));
	else
		return (1);
}

t_entity	*entity_new(size_t y, size_t x, t_entity_type *type, t_game *game)
{
	t_entity		*entity;
	t_arraylist 	drawables;
	
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
	else if (entity->type == ITEM)
		game->state.collectables_count++;
	return (entity);
}

void	entity_destroy(t_entity *entity)
{
	if (!entity)
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
	else
		return entity;
}

int32_t	entities_init(t_game *game)
{
	if (ft_arraylist_transform2diarg(
			game->map.chart,
			(void *(*)(size_t, size_t, void*, void*)) entity_new,
		(void *) game,
		(t_vfun1) ft_nop) > 0)
		return (0);
	ft_arraylist_transform2d(
		game->map.chart,
		(void *(*) (void*)) map_remove_floor,
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

int32_t	map_init(t_map *map)
{
	int 	fd;
	char	*str;

	map->chart = ft_arraylist_new((free));
	if (!map->chart)
		return (0);
	fd = open("./maps/m.ber", O_RDONLY);
	if (fd < 0)
		return (0);
	str = get_next_line(fd);
	while (str)
	{
		map->chart = ft_arraylist_add(map->chart, str);
		if (!map->chart)
			return (0);
		str = get_next_line(fd);
	}
	if (!map->chart)
		return (0);
	if (ft_arraylist_transform(map->chart, map_transform_string_line,
			(t_vfun1) ft_arraylist_destroy) > 0)
		return (0);
	return (1);
}

int32_t	system_init(t_game *game)
{
	game->state.collectables_count = 0;
	game->ctx.drawables = ft_arraylist_new((t_vfun1) ft_arraylist_destroy);
	game->ctx.textures = ft_arraylist_new((t_vfun1) mlx_delete_texture);
	if (!game->ctx.drawables || !game->ctx.textures)
		return (system_panic(game, MEMORY_ERROR, "No memory to init drawables"));
	game->mlx = mlx_init(WIDTH, HEIGHT, "So Long", true);
	if (!game->mlx)
		return (system_panic(game, MLX_ERROR, NULL));
	if (!map_init(&game->map))
		return (system_panic(game, ERROR, "Failed to init map"));
	if (!context_init(&game->ctx, game->mlx))
		return (system_panic(game, ERROR, "Failed to init assets"));
	if (!entities_init(game))
		return (system_panic(game, ERROR, "Failed to init entities"));
	mlx_loop_hook(game->mlx, (t_vfun1) system_loop, game);
	mlx_close_hook(game->mlx, system_exit_ok, game);
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
	return (game.exit_status);
}
