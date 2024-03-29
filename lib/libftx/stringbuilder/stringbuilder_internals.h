/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stringbuilder_internals.h                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 20:37:07 by maurodri          #+#    #+#             */
/*   Updated: 2024/02/06 20:52:47 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRINGBUILDER_INTERNALS_H
# define STRINGBUILDER_INTERNALS_H

# define DEFAULT_LINE_SIZE 90

# include <stdlib.h>

struct s_stringbuilder
{
	char			*str;
	unsigned int	size;
	unsigned int	capacity;
};

#endif
