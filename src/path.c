/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/29 10:21:33 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/11/29 10:30:31 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include <stdlib.h>

bool		is_special(char *name)
{
	return (ft_strequ(".", name) || ft_strequ("..", name));
}

char		*path_join(char *path, size_t path_len, char *name, size_t name_len)
{
	char	*str;

	str = malloc(path_len + name_len + 2);
	ft_memcpy(str, path, path_len);
	str[path_len] = '/';
	ft_memcpy(str + path_len + 1, name, name_len);
	str[path_len + name_len + 1] = 0;
	return (str);
}
