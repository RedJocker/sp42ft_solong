/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entity_hero2_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 19:46:08 by maurodri          #+#    #+#             */
/*   Updated: 2024/04/01 19:46:57 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_bonus.h"

t_moveable	*entity_hero_get_moveable(t_entity *hero)
{
	t_component	*component;

	component = ft_arraylist_get(hero->components, 0);
	return (component->cmp);
}
