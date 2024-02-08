/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:28:12 by maurodri          #+#    #+#             */
/*   Updated: 2024/02/08 02:04:43 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <MLX42/MLX42.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "collection/ft_arraylist.h"
#include "scratch.h"

int32_t	system_panic(t_game *game)
{
	game->exit_status = ERROR;
	if (game->mlx)
		mlx_close_window(game->mlx);
	puts(mlx_strerror(mlx_errno));
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

int32_t	init(t_game *game)
{
	t_drawable		*hero_drawable;
	mlx_texture_t	*hero_texture;

	game->ctx.drawables = ft_arraylist_new(free);
	game->ctx.textures = ft_arraylist_new(
			(void (*)(void *)) mlx_delete_texture);
	game->mlx = mlx_init(WIDTH, HEIGHT, "So Long", true);
	hero_drawable = malloc(sizeof(t_drawable));
	hero_texture = mlx_load_png("./assets/hero.png");
	if (!game->ctx.drawables || !game->ctx.textures || !game->mlx
		|| !hero_drawable || !hero_texture)
		return (system_panic(game));
	game->ctx.textures = ft_arraylist_add(game->ctx.textures, hero_texture);
	game->ctx.drawables = ft_arraylist_add(game->ctx.drawables, hero_drawable);
	hero_drawable->img = mlx_texture_to_image(game->mlx, hero_texture);
	if (!hero_drawable->img || !game->ctx.drawables
		|| !game->ctx.textures)
		(system_panic(game));
	hero_drawable->i = mlx_image_to_window(
			game->mlx, hero_drawable->img, 10 * 32, 10 * 32);
	if (hero_drawable->i < 0)
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
