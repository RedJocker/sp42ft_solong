/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_bonus.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 04:08:54 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/30 04:23:59 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYSTEM_H
# define SYSTEM_H

# include "so_long_bonus.h"

int32_t				system_quit_invalid(char *err_msg);
int32_t				system_init(t_game *game, char *map_path);

#endif
