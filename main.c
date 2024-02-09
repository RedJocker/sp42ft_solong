/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:28:12 by maurodri          #+#    #+#             */
/*   Updated: 2024/02/09 05:20:34 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <MLX42/MLX42.h>

#include "collection/ft_arraylist.h"
#include "scratch.h"
#include "ft_stdio.h"

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

int32_t context_load_asset(t_context *ctx, char *path, mlx_t *mlx)
{
	t_drawable		*hero_drawable;
	mlx_texture_t	*hero_texture;

	hero_drawable = malloc(sizeof(t_drawable));
	if (!hero_drawable)
		return (0);
	ctx->drawables = ft_arraylist_add(ctx->drawables, hero_drawable);
	if (!ctx->drawables)
		return (0);
	hero_texture = mlx_load_png(path);
	if (!hero_texture)
		return (0);
	ctx->textures = ft_arraylist_add(ctx->textures, hero_texture);
	if (!ctx->textures)
		return (0);
	hero_drawable->img = mlx_texture_to_image(mlx, hero_texture);
	if (!hero_drawable->img)
		return (0);
	return (1);
}

int32_t context_init(t_context *ctx, mlx_t *mlx)
{
	int32_t	i;
	char 	*asset;
	int32_t  is_ok;
	
	i = 0;
	while (1)
	{
		if (i == HERO)
			is_ok = context_load_asset(ctx, "./assets/hero.png", mlx);
		else if (i == ITEM)
			is_ok = context_load_asset(ctx, "./assets/item.png", mlx);
		else
			break;
		if (!is_ok)
			return (0);
		i++;
	}
	return (1);
}



int32_t game_map_init(t_map *map)
{
	map->chart = ft_arraylist_new()
}

int32_t	init(t_game *game)
{
	t_drawable	*hero_drawable;
	t_drawable	*item_drawable;

	game->ctx.drawables = ft_arraylist_new(free);
	game->ctx.textures = ft_arraylist_new(
			(void (*)(void *)) mlx_delete_texture);
	game->mlx = mlx_init(WIDTH, HEIGHT, "So Long", true);
	if (!game->ctx.drawables || !game->ctx.textures || !game->mlx)
		return (system_panic(game));
	game_map_init(&game->map);
	context_init(&game->ctx, game->mlx);
	hero_drawable = ft_arraylist_get(game->ctx.drawables, HERO);
	hero_drawable->i = mlx_image_to_window(
			game->mlx, hero_drawable->img, 10 * 32, 10 * 32);
	if (hero_drawable->i < 0)
		(system_panic(game));
	item_drawable = ft_arraylist_get(game->ctx.drawables, ITEM);
	item_drawable->i = mlx_image_to_window(
			game->mlx, item_drawable->img, 15 * 32, 10 * 32);
	if (item_drawable->i < 0)
		(system_panic(game));
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
	if (game.mlx)
		mlx_terminate(game.mlx);
	return (game.exit_status);
}
