/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entity_init_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:06:20 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/30 04:20:57 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "entity_bonus.h"
#include "system_bonus.h"

int	entity_init_drawables(
	t_entity *entity, t_arraylist drawables_ctx, t_game *game)
{
	t_drawable	*drawable_ctx;
	t_drawable	*drawable_copy;
	int			xyi[3];

	xyi[2] = -1;
	while ((size_t)++xyi[2] < ft_arraylist_len(drawables_ctx))
	{
		drawable_ctx = ft_arraylist_get(drawables_ctx, xyi[2]);
		drawable_copy = malloc(sizeof(t_drawable));
		if (!drawable_copy)
			return (system_quit_invalid("No memory for drawable_copy"));
		entity->drawables = ft_arraylist_add(entity->drawables, drawable_copy);
		if (!entity->drawables)
			return (system_quit_invalid("E-drawables was null after add"));
		drawable_copy->img = drawable_ctx->img;
		xyi[0] = entity->x * game->ctx.block_size + game->ctx.window_x_offset;
		xyi[1] = entity->y * game->ctx.block_size + game->ctx.window_y_offset;
		drawable_copy->i = mlx_image_to_window(
				game->mlx, drawable_copy->img, xyi[0], xyi[1]);
		if (drawable_copy->i < 0)
			return (system_quit_invalid("Mlx_image_to_window failed"));
		drawable_copy->img->instances[drawable_copy->i].enabled = (
				xyi[2] == 0 && entity->type != EXIT);
	}
	return (1);
}

int	entity_init_components(t_entity *entity, t_game *game)
{
	(void) game;
	if (entity->type == HERO)
		return (entity_hero_init_components(entity));
	else
		return (1);
}
