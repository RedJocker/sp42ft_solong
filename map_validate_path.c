/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_validate_path.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 23:23:36 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/14 04:22:31 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "collection/ft_arraylist.h"
#include "map.h"
#include "map_internal.h"

static int32_t	check_all_found(char char_histogram[256])
{
	return (char_histogram[(int) 'P'] == 0
		&& char_histogram[(int) 'C'] == 0
		&& char_histogram[(int) 'E'] == 0);
}

static char	get_char_and_mark(t_map *map, int x, int y)
{
	char	*row;
	char	current;

	row = ft_arraylist_get(map->chart, y);
	current = (row[x]);
	if (current == 'P' || current == 'E' || current == 'C')
	{
		row[x] += 32;
	}
	else if (current == '0')
	{
		row[x] = 'o';
	}
	return (current);
}

static int32_t	check_path(t_map *map, char char_histogram[256], int x, int y)
{
	char	c;

	c = get_char_and_mark(map, x, y);
	if (c == 'P' || c == 'E' || c == 'C')
		char_histogram[(int) c]--;
	if (check_all_found(char_histogram))
		return (1);
	if (c == 'P' || c == 'E' || c == 'C' || c == '0')
	{
		return (
			check_path(map, char_histogram, x - 1, y)
			|| check_path(map, char_histogram, x + 1, y)
			|| check_path(map, char_histogram, x, y - 1)
			|| check_path(map, char_histogram, x, y + 1));
	}
	return (0);
}

int32_t	map_validate_path(t_map *map, char char_histogram[256], int hero_pos[2])
{
	return (check_path(map, char_histogram, hero_pos[1], hero_pos[0]));
}
