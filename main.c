/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:28:12 by maurodri          #+#    #+#             */
/*   Updated: 2024/02/09 11:41:58 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <MLX42/MLX42.h>
#include "collection/ft_arraylist.h"
#include "ft_memlib.h"
#include "scratch.h"
#include "ft_stdio.h"
#include "ft_util.h"
#include "ft_string.h"

int32_t	system_panic(t_game *game)
{
	game->exit_status = ERROR;
	if (game->mlx)
		mlx_close_window(game->mlx);
	ft_printf(mlx_strerror(mlx_errno));
	return (ERROR);
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
	(void) game;
}

int32_t	context_load_asset(t_context *ctx, char *path, mlx_t *mlx)
{
	t_drawable		*drawable;
	mlx_texture_t	*texture;

	drawable = malloc(sizeof(t_drawable));
	if (!drawable)
		return (0);
	ctx->drawables = ft_arraylist_add(ctx->drawables, drawable);
	if (!ctx->drawables)
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

int32_t	context_init(t_context *ctx, mlx_t *mlx)
{
	int32_t	i;
	int32_t	is_ok;

	i = 0;
	while (1)
	{
		if (i == HERO)
			is_ok = context_load_asset(ctx, "./assets/hero.png", mlx);
		else if (i == ITEM)
			is_ok = context_load_asset(ctx, "./assets/item.png", mlx);
		else
			break ;
		if (!is_ok)
			return (0);
		i++;
	}
	return (1);
}

t_entity_type entity_type_by_ch(char ch)
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

t_entity	*entity_init(size_t y, size_t x, char *ch, t_game *game)
{
	t_entity		*entity;
	t_drawable		*drawable;
	entity = malloc(sizeof(t_entity));
	if (!entity)
		return (NULL);
	entity->drawables = malloc(sizeof(t_drawable));
	entity->type = entity_type_by_ch(*ch);
	entity->x = x;
	entity->y = y;
	entity->components = NULL;
	drawable = ft_arraylist_get(game->ctx.drawables, entity->type);
	ft_memcpy(entity->drawables, drawable, sizeof(t_drawable));
	entity->drawables->i = mlx_image_to_window(
		game->mlx, drawable->img, entity->x * 32, entity->y * 32);
	if (entity->drawables->i < 0)
		return (NULL);
	return (entity);
}

void	entity_destroy(t_entity *entity)
{
	if (!entity)
		return ;
	if (entity->components)
		ft_arraylist_destroy(entity->components);
	free(entity->drawables);
	free(entity);
}
	
int32_t entities_init(t_game *game)
{
	if (ft_arraylist_transform2diarg(
			game->map.chart,
			(void* (*) (size_t, size_t, void *, void *)) entity_init,
			(void *) game,
			(void (*) (void *))entity_destroy) > 0)
		return (0);
	
	return (1);
}

void	*map_transform_string_line(void *s)
{
	char		*str;
	t_arraylist	lst_ch;
	size_t		i;
	size_t		len;

	str = s;
	lst_ch = ft_arraylist_new((void (*)(void *)) ft_nop);
	if (!lst_ch)
		return (NULL);
	i = 0;
	len = ft_strlen(str);
	while (i < len)
	{
		lst_ch = ft_arraylist_add(lst_ch, str + i);
		if (!lst_ch)
			return (NULL);
		i++;
	}
	return (lst_ch);
}

int32_t	game_map_init(t_map *map)
{
	map->chart = ft_arraylist_new((void (*)(void *)) ft_nop);
	if (!map->chart)
		return (0);
	map->chart = ft_arraylist_add(map->chart, "PCC");
	if (!map->chart)
		return (0);
	if (ft_arraylist_transform(map->chart, map_transform_string_line,
							(void (*)(void *)) ft_arraylist_destroy) > 0)
		return (0);
	return (1);
}

int32_t	init(t_game *game)
{
	game->ctx.drawables = ft_arraylist_new(free);
	game->ctx.textures = ft_arraylist_new(
			(void (*)(void *)) mlx_delete_texture);
	game->mlx = mlx_init(WIDTH, HEIGHT, "So Long", true);
	if (!game->ctx.drawables || !game->ctx.textures || !game->mlx)
		return (system_panic(game));
	if (!game_map_init(&game->map))
		return (system_panic(game));
	if (!context_init(&game->ctx, game->mlx))
		return (system_panic(game));
	if (!entities_init(game))
		return (system_panic(game));
	mlx_loop_hook(game->mlx, (void (*)(void *)) loop, game);
	mlx_close_hook(game->mlx, system_exit_ok, game);
	return (OK);
}

int32_t	main(void)
{
	t_game	game;

	if (init(&game) == OK)
		mlx_loop(game.mlx);
	ft_arraylist_destroy(game.ctx.drawables);
	ft_arraylist_destroy(game.ctx.textures);
	ft_arraylist_destroy(game.map.chart);
	if (game.mlx)
		mlx_terminate(game.mlx);
	return (game.exit_status);
}
