/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_chomp.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/07 19:48:45 by maurodri          #+#    #+#             */
/*   Updated: 2024/03/12 21:33:11 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

char	*ft_chomp(char *str)
{
	char	*ptr;

	if (!str)
		return (str);
	ptr = str;
	while (*ptr)
		ptr++;
	if (ptr == str)
		return (str);
	ptr--;
	if (*ptr == '\n')
		*ptr = '\0';
	return (str);
}
