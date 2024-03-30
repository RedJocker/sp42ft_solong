/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entity_bonus.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:08:47 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/30 04:20:05 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENTITY_H
# define ENTITY_H

# include "so_long_bonus.h"

t_entity_type	entity_type_by_ch(char ch);
t_entity		*entity_new(
					size_t y, size_t x, t_entity_type *type, t_game *game);
void			entity_destroy(t_entity *entity);
void			entity_exit_destroy(t_entity *entity);
t_entity		*entity_remove_floor(t_entity *entity);
// entity_init.c
int				entity_init_drawables(
					t_entity *entity, t_arraylist drawables_ctx, t_game *game);
int				entity_init_components(t_entity *entity, t_game *game);
// entity_hero.c
void			entity_hero_animate(t_entity *hero);
t_moveable		*entity_hero_get_moveable(t_entity *hero);
t_direction		entity_hero_move(t_entity *entity, t_game *game);
void			entity_hero_components_destroy(t_component *component);
int				entity_hero_init_components(t_entity *entity);

#endif
