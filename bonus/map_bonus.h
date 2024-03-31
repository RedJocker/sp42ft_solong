/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_bonus.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:08:49 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/30 22:34:10 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAP_BONUS_H
# define MAP_BONUS_H

# include "so_long_bonus.h"

t_entity	*map_entity_get(t_game *game, int x, int y);
void		map_entity_update_pos(
				t_entity *entity, t_drawable *dwb, t_game *game);
size_t		map_height(t_map *map);
size_t		map_width(t_map *map);
int32_t		map_init(t_map *map, char *path);
void		map_hero_switch_position(
				t_game *game,
				t_entity *other,
				size_t new_pos[2],
				size_t old_pos[2]);

#endif
