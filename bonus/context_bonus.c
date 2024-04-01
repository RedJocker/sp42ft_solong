/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:05:58 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/01 01:00:20 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "context_bonus.h"
#include "MLX42/MLX42.h"
#include "so_long_bonus.h"
#include "system_bonus.h"

static int32_t	context_load_asset(
	t_context *ctx, char *path, mlx_t *mlx, t_entity_type type)
{
	t_drawable		*drawable;
	mlx_texture_t	*texture;
	t_arraylist		lst;

	drawable = malloc(sizeof(t_drawable));
	if (!drawable)
		return (system_quit_invalid("No memory for drawable"));
	lst = ft_arraylist_get(ctx->drawables, type);
	if (!lst)
		return (system_quit_invalid("List of drawables was null"));
	lst = ft_arraylist_add(lst, drawable);
	if (!lst)
		return (system_quit_invalid("List was null after adding drawable"));
	texture = mlx_load_png(path);
	if (!texture)
		return (system_quit_invalid("Texture was null after mlx_load_png"));
	ctx->textures = ft_arraylist_add(ctx->textures, texture);
	if (!ctx->textures)
		return (system_quit_invalid("List of textures was null after add"));
	drawable->img = mlx_texture_to_image(mlx, texture);
	if (!drawable->img)
		return (system_quit_invalid("Failed to create image from texture"));
	return (1);
}

static int32_t	context_init_drawables_list(t_context *ctx)
{
	int32_t		i;
	t_arraylist	lst;

	i = 0;
	while (i <= EXIT)
	{
		lst = ft_arraylist_new(free);
		if (!lst)
			return (system_quit_invalid("Failed to init list of drawables"));
		ctx->drawables = ft_arraylist_add(ctx->drawables, lst);
		if (!ctx->drawables)
			return (system_quit_invalid("Failed add list to ctx->drawables"));
		i++;
	}
	return (1);
}

static int32_t	context_init_load_villain_assets(
	t_context *ctx, mlx_t *mlx)
{
	int32_t	is_ok;

	is_ok = context_load_asset(ctx, "./assets/villain1.png", mlx, VILAIN);
	if (!is_ok)
		return (0);
	is_ok = context_load_asset(ctx, "./assets/villain2.png", mlx, VILAIN);
	if (!is_ok)
		return (0);
	return (1);
}

static int32_t	context_init_load_hero_assets(t_context *ctx, mlx_t *mlx)
{
	int32_t	is_ok;

	is_ok = context_load_asset(ctx, "./assets/hero1.png", mlx, HERO);
	if (!is_ok)
		return (0);
	is_ok = context_load_asset(ctx, "./assets/hero2.png", mlx, HERO);
	if (!is_ok)
		return (0);
	is_ok = context_load_asset(ctx, "./assets/hero3.png", mlx, HERO);
	if (!is_ok)
		return (0);
	is_ok = context_load_asset(ctx, "./assets/hero4.png", mlx, HERO);
	if (!is_ok)
		return (0);
	is_ok = context_load_asset(ctx, "./assets/hero5.png", mlx, HERO);
	if (!is_ok)
		return (0);
	is_ok = context_load_asset(ctx, "./assets/hero6.png", mlx, HERO);
	if (!is_ok)
		return (0);
	is_ok = context_load_asset(ctx, "./assets/hero7.png", mlx, HERO);
	if (!is_ok)
		return (0);
	is_ok = context_load_asset(ctx, "./assets/hero8.png", mlx, HERO);
	return (is_ok);
}

int32_t	context_init(t_context *ctx, mlx_t *mlx)
{
	int32_t	i;
	int32_t	is_ok;

	ctx->overflow_x_offset = 0;
	ctx->overflow_y_offset = 0;
	if (!context_init_drawables_list(ctx))
		return (system_quit_invalid("Failed to init ctx drawables"));
	ctx->count_img = mlx_put_string(mlx, "movement count: 0", 10, 10);
	i = -1;
	while (++i <= EXIT)
	{
		if (i == HERO)
			is_ok = context_init_load_hero_assets(ctx, mlx);
		else if (i == ITEM)
			is_ok = context_load_asset(ctx, "./assets/item.png", mlx, ITEM);
		else if (i == WALL)
			is_ok = context_load_asset(ctx, "./assets/wall.png", mlx, WALL);
		else if (i == VILAIN)
			is_ok = context_init_load_villain_assets(ctx, mlx);
		else if (i == EXIT)
			is_ok = context_load_asset(ctx, "./assets/exit.png", mlx, EXIT);
		if (!is_ok)
			return (system_quit_invalid("failed loading some asset"));
	}
	return (1);
}
