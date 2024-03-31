/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_validate_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:06:26 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/31 00:22:39 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "map_bonus.h"
#include "map_internal_bonus.h"
#include "system_bonus.h"
#include "ft_string.h"
#include "ft_memlib.h"

int32_t	map_validate_retangular_shape(t_map *map)
{
	size_t		width;
	size_t		height;
	size_t		i;
	char		*row;

	row = ft_arraylist_get(map->chart, 0);
	width = ft_strlen(row);
	height = map_height(map);
	i = 1;
	while (i < height)
	{
		row = ft_arraylist_get(map->chart, i);
		if (ft_strlen(row) != width)
			return (0);
		i++;
	}
	return (1);
}

int32_t	map_validate_wall_row(t_map *map)
{
	size_t		height;
	char		*row;
	int			i_col;

	row = ft_arraylist_get(map->chart, 0);
	i_col = -1;
	while (row[++i_col])
		if (row[i_col] != '1')
			return (system_quit_invalid("Invalid first row for map"));
	height = map_height(map);
	row = ft_arraylist_get(map->chart, height - 1);
	i_col = -1;
	while (row[++i_col])
		if (row[i_col] != '1')
			return ((system_quit_invalid("Invalid last row for map")));
	return (1);
}

int32_t	map_validate_wall_col(t_map *map)
{
	size_t		width;
	size_t		height;
	size_t		i;
	char		*row;

	row = ft_arraylist_get(map->chart, 0);
	height = map_height(map);
	width = ft_strlen(row);
	i = 0;
	while (i < height)
	{
		row = ft_arraylist_get(map->chart, i);
		if (*row != '1')
			return (system_quit_invalid("Invalid col for map"));
		if (*(row + width - 1) != '1')
			return (system_quit_invalid("Invalid col for map"));
		i++;
	}
	return (1);
}

int32_t	map_validate_entities_count(char *char_map)
{
	if (char_map['P'] != 1)
		return (system_quit_invalid("Invalid count of P on map"));
	else if (char_map['E'] != 1)
		return (system_quit_invalid("Invalid count of E on map"));
	else if (char_map['C'] < 1)
		return (system_quit_invalid("Invalid count of C on map"));
	return (1);
}

int32_t	map_validate_entities(t_map *map, char char_histogram[256], int pos[2])
{
	size_t		height;
	size_t		i[2];
	char		*row;

	row = ft_arraylist_get(map->chart, 0);
	height = map_height(map);
	i[0] = 0;
	ft_bzero(char_histogram, 256);
	while (++i[0] < height)
	{
		row = ft_arraylist_get(map->chart, (size_t) i[0]);
		i[1] = -1;
		while (row[++i[1]])
		{
			if (!ft_strchr("10EPCV", row[i[1]]))
				return (system_quit_invalid("Invalid character on map"));
			char_histogram[(int) row[i[1]]]++;
			if (row[i[1]] == 'P')
			{
				pos[1] = i[1];
				pos[0] = i[0];
			}
		}
	}
	return (map_validate_entities_count(char_histogram));
}
