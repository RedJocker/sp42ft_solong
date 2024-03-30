/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 19:00:30 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/18 20:28:53 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYSTEM_H
# define SYSTEM_H

# include "so_long.h"

int32_t				system_quit_invalid(char *err_msg);
int32_t				system_init(t_game *game, char *map_path);

#endif
