/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:06:21 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/02 00:51:46 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_bonus.h"
#include "system_bonus.h"
#include "entity_bonus.h"
#include "ft_string.h"

static int32_t	is_valid_map_filename(char *filename)
{
	char	*p;

	p = filename;
	while (*p)
		p++;
	if (p == filename)
		return (system_quit_invalid("Empty filename"));
	p -= 4;
	if (p < filename || ft_strncmp(p, ".ber", 4) != 0)
		return (system_quit_invalid("Invalid extension for map file"));
	return (1);
}

int32_t	main(int args_len, char *args[])
{
	t_game	game;

	if (args_len != 2)
	{
		system_quit_invalid("Invalid command."
			" Usage example: so_long some_map.ber");
		return (1);
	}
	else if (!is_valid_map_filename(args[1]))
		return (1);
	if (system_init(&game, args[1]) == OK)
		mlx_loop(game.mlx);
	ft_arraylist_destroy(game.ctx.drawables);
	ft_arraylist_destroy(game.ctx.textures);
	ft_arraylist_destroy(game.map.chart);
	ft_arraylist_destroy(game.ctx.animated);
	if (game.mlx)
		mlx_terminate(game.mlx);
	entity_exit_destroy(game.map.exit);
	return (game.exit_status);
}
