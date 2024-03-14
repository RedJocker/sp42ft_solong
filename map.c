/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 20:20:57 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/14 01:05:45 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include "map.h"
#include "map_internal.h"
#include "system.h"
#include "entity.h"
#include "ft_string.h"
#include "get_next_line.h"

static void	*map_transform_string_line(void *s)
{
	char			ch;
	t_arraylist		lst_type;
	size_t			i_to_len[2];
	t_entity_type	*type;

	lst_type = ft_arraylist_new(free);
	if (!lst_type)
		return (NULL);
	i_to_len[0] = 0;
	i_to_len[1] = ft_strlen(((char *)s));
	while (i_to_len[0] < i_to_len[1])
	{
		ch = ((char *)s)[i_to_len[0]];
		if (ch == '\n')
			break ;
		type = malloc(sizeof(t_entity_type));
		*type = entity_type_by_ch(ch);
		if (!type)
			return (NULL);
		lst_type = ft_arraylist_add(lst_type, type);
		if (!lst_type)
			return (NULL);
		i_to_len[0]++;
	}
	return (lst_type);
}

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
	if (!map_validate_retangular_shape(map))
		return (system_invalid("Invalid map, should be retangular"));
	else if (!map_validate_wall_row(map))
		return (system_invalid("Invalid row for map"));
	else if (!map_validate_wall_col(map))
		return (system_invalid("Invalid col for map"));
	else if (!map_validate_entities(map, char_histogram, hero_pos))
		return (0);
	else if (!map_validate_path(map, char_histogram, hero_pos))
		return (system_invalid("Invalid map, no path to solve"));
	return (1);
}

int32_t	map_init(t_map *map, char *path)
{
	int		fd;
	char	*str;

	map->chart = ft_arraylist_new((free));
	if (!map->chart)
		return (system_invalid("Malloc fail\n"));
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (system_invalid("File open fail\n"));
	str = ft_chomp(get_next_line(fd));
	while (str)
	{
		map->chart = ft_arraylist_add(map->chart, str);
		if (!map->chart)
			return (system_invalid("Failed to add str to map->chart on init\n"));
		str = ft_chomp(get_next_line(fd));
	}
	if (!map->chart)
		return (system_invalid("Failed to init map->chart"));
	if (!map_is_valid(map))
		return (0);
	if (ft_arraylist_transform(map->chart, map_transform_string_line,
			(t_vfun1) ft_arraylist_destroy) > 0)
		return (system_invalid("Failed to transform map"));
	return (1);
}
