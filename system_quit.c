/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_quit.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 20:42:09 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/18 20:43:53 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "system.h"
#include "system_internal.h"
#include "ft_stdio.h"

int32_t	system_quit_invalid(char *err_msg)
{
	ft_putstr_fd("ERROR:\n\t", 2);
	ft_putstr_fd(err_msg, 2);
	ft_putstr_fd("\n", 2);
	return (0);
}

int32_t	system_quit_panic(t_game *game, t_exit_status exit_code, char *msg)
{
	game->exit_status = exit_code;
	if (game->mlx)
		mlx_close_window(game->mlx);
	if (exit_code == MLX_ERROR)
		ft_putstr_fd((char *) mlx_strerror(mlx_errno), 2);
	else if (msg)
	{
		ft_putstr_fd("ERROR:\n\t", 2);
		ft_putstr_fd(msg, 2);
		ft_putstr_fd("\n", 2);
	}
	return (exit_code);
}

void	system_quit_ok(void *param)
{
	t_game	*game;

	game = param;
	game->exit_status = OK;
	if (game->mlx)
		mlx_close_window(game->mlx);
}
