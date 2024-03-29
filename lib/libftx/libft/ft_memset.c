/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/14 17:47:48 by maurodri          #+#    #+#             */
/*   Updated: 2024/02/06 09:03:09 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_memlib.h"

void	*ft_memset(void *buffer, int ch, size_t len)
{
	size_t			i;
	unsigned char	c;
	char			*buf;

	c = (unsigned char) ch;
	buf = buffer;
	i = 0;
	while (i < len)
	{
		buf[i++] = c;
	}
	return (buffer);
}
