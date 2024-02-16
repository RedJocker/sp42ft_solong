/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:28:12 by maurodri          #+#    #+#             */
/*   Updated: 2024/02/15 21:50:59 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <MLX42/MLX42.h>
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

void	loop(t_game *game)
{
	t_entity *e = game->map.hero;
	t_entity *o;
	t_moveable *moveable;
	t_component *component;
	t_direction direction;
	int32_t		new_x;
	int32_t		new_y;
	
	for (size_t i = 0; i < ft_arraylist_len(e->drawables); i++)
	{
		t_drawable *d = ft_arraylist_get(e->drawables, i);
		d->img->instances[d->i].enabled = ((size_t) mlx_get_time()) % 2 == i;
	}
	component = ft_arraylist_get(game->map.hero->components, 0);
	moveable = component->component;
	direction = moveable->move(e, game);
	if (direction == IDLE)
		return;
	new_x = e->x + (direction == RIGHT) - (direction == LEFT);
	new_y = e->y + (direction == DOWN) - (direction == UP);
	o = ft_arraylist_get(ft_arraylist_get(game->map.chart, new_y), new_x);
	if (!o)
	{
		for (size_t i = 0; i < ft_arraylist_len(e->drawables); i++)
		{
			t_drawable *d = ft_arraylist_get(e->drawables, i);
			d->img->instances[d->i].x = 32 * new_x;
			d->img->instances[d->i].y = 32 * new_y;
		}
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

int		entity_drawables_init(t_entity *entity, t_game *game)
{
	t_drawable	*drawable;
	size_t		i;

	i = 0;
	while (i < ft_arraylist_len(entity->drawables))
	{
		drawable = ft_arraylist_get(
				entity->drawables, i);
		drawable->i = mlx_image_to_window(
				game->mlx, drawable->img, entity->x * 32, entity->y * 32);
		if (drawable->i < 0)
			return (0);
		drawable->img->instances[drawable->i].enabled = i == 0;
		i++;
	}
	return (1);
}

t_direction entity_hero_move(t_entity *entity, t_game *game)
{
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
		free(component->component);
	free(component);
}

int	entity_hero_components_init(t_entity *entity, t_game *game)
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
	component->component = moveable;
	moveable->move = entity_hero_move;
	entity->components = ft_arraylist_add(entity->components, component);
	return (1);
}

int	entity_components_init(t_entity *entity, t_game *game)
{
	if (entity->type == HERO)
		return (entity_hero_components_init(entity, game));
	else
		return (1);
}

t_entity	*entity_new(size_t y, size_t x, t_entity_type *type, t_game *game)
{
	t_entity		*entity;

	entity = malloc(sizeof(t_entity));
	if (!entity)
		return (NULL);
	entity->type = *type;
	entity->drawables = ft_arraylist_get(game->ctx.drawables, entity->type);
	entity->x = x;
	entity->y = y;
	entity->components = NULL;
	if (!entity_drawables_init(entity, game))
		return (NULL);
	if (!entity_components_init(entity, game))
		return (NULL);
	if (entity->type == HERO)
		game->map.hero = entity;
	return (entity);
}

void	entity_destroy(t_entity *entity)
{
	if (!entity)
		return ;
	if (entity->components)
		ft_arraylist_destroy(entity->components);
	free(entity);
}

t_entity	*map_remove_floor(t_entity *entity)
{
	if (entity->type == FLOOR)
		return (ft_free_retnull(entity));
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
	mlx_loop_hook(game->mlx, (t_vfun1) loop, game);
	mlx_close_hook(game->mlx, system_exit_ok, game);
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
