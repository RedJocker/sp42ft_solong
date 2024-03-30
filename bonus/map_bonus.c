/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_bonus.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:06:22 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/30 04:21:24 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "map_bonus.h"
#include "map_internal_bonus.h"
#include "system_bonus.h"

size_t	map_height(t_map *map)
{
	return (ft_arraylist_len(map->chart));
}

size_t	map_width(t_map *map)
{
	return (ft_arraylist_len(
			ft_arraylist_get(map->chart, 0)));
}

int32_t	map_is_valid(t_map *map)
{
	char	char_histogram[256];
	int		hero_pos[2];

	hero_pos[0] = -1;
	hero_pos[1] = -1;
	if (!map->chart)
		return (system_quit_invalid("Failed to init map->chart"));
	else if (!map_validate_retangular_shape(map))
		return (system_quit_invalid("Invalid map, should be retangular"));
	else if (!map_validate_wall_row(map))
		return (0);
	else if (!map_validate_wall_col(map))
		return (0);
	else if (!map_validate_entities(map, char_histogram, hero_pos))
		return (0);
	else if (!map_validate_path(map, char_histogram, hero_pos))
		return (system_quit_invalid("Invalid map, no path to solve"));
	return (1);
}
