/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_internal.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 22:46:40 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/18 22:14:35 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYSTEM_INTERNAL_H
# define SYSTEM_INTERNAL_H

# include "so_long.h"

int32_t				system_quit_panic(
						t_game *game, t_exit_status exit_code, char *msg);
void				system_quit_ok(void *param);
void				system_hero_move(t_game *game, t_entity *hero);
void				system_exit_enable(t_game *game);
void				system_game_end(t_game *game);
t_screen_overflow	system_hero_screen_overflown(t_game *game);
void				system_map_update_all_drawables_pos(t_game *game);
void				system_map_update_pos(t_screen_overflow over, t_game *game);
void				system_loop(t_game *game);
void				system_hero_wait_input(t_game *game, t_entity *hero);
void				system_hero_update_drawables_pos(t_game *game);
void				system_update_drawables_pos(
						t_arraylist drawables, int x, int y, t_context ctx);

#endif
