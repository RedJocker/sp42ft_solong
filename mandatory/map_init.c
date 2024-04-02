/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 20:45:09 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/02 12:56:36 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>
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

int32_t	map_init_read_lines(t_map *map, int fd)
{
	char	*str;
	int		rows;

	str = ft_chomp(get_next_line(fd));
	rows = -1;
	while (str)
	{
		map->chart = ft_arraylist_add(map->chart, str);
		if (!map->chart)
			return (system_quit_invalid("Failed to add to map->chart\n"));
		if (ft_strlen(str) > MAX_MAP_SIZE)
			return ((system_quit_invalid("Invalid width for map, max 250")));
		if (++rows > MAX_MAP_SIZE)
			return ((system_quit_invalid("Invalid height for map, max 250")));
		str = ft_chomp(get_next_line(fd));
	}
	return (1);
}

int32_t	map_init(t_map *map, char *path)
{
	int		fd;
	int		read_ok;

	map->chart = ft_arraylist_new((free));
	if (!map->chart)
		return (system_quit_invalid("Malloc fail\n"));
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (system_quit_invalid("File open fail\n"));
	read_ok = map_init_read_lines(map, fd);
	close(fd);
	if (!read_ok)
		return (0);
	if (!map_is_valid(map))
		return (0);
	if (ft_arraylist_transform(map->chart, map_transform_string_line,
			(t_vfun1) ft_arraylist_destroy) > 0)
		return (system_quit_invalid("Failed to transform map"));
	return (1);
}
