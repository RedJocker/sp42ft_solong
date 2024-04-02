/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_internal.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 22:54:22 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/02 12:52:18 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAP_INTERNAL_H
# define MAP_INTERNAL_H

# include "so_long.h"

# define MAX_MAP_SIZE 250

int32_t		map_validate_retangular_shape(t_map *map);
int32_t		map_validate_wall_row(t_map *map);
int32_t		map_validate_wall_col(t_map *map);
int32_t		map_validate_entities_count(char *char_map);
int32_t		map_validate_entities(
				t_map *map, char char_histogram[256], int pos[2]);
int32_t		map_validate_path(t_map *map, char char_histogram[256], int pos[2]);
int32_t		map_is_valid(t_map *map);

#endif
