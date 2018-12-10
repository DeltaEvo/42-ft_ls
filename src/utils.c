/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/28 10:11:35 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/12/07 16:27:57 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include <stdlib.h>
#include <sys/ioctl.h>

size_t		nb_len(uint64_t n)
{
	size_t	len;

	len = 1;
	while (n /= 10)
		len++;
	return (len);
}

char		*ft_strdupl(const char *src, size_t *len)
{
	char	*str;

	*len = ft_strlen(src);
	if (!(str = malloc(*len + 1)))
		return (NULL);
	while (*src)
		*str++ = *src++;
	*str = '\0';
	return (str - *len);
}

char		*ft_itoal(int64_t nb, size_t *len)
{
	size_t	pow;
	char	*str;
	uint8_t	mod;

	mod = nb < 0;
	pow = nb_len(nb);
	*len = pow + mod;
	if (!(str = (char *)malloc(*len + 1)))
		return (NULL);
	str[pow + mod] = 0;
	if (mod)
		*str++ = '-';
	else
		nb = -nb;
	while (pow--)
	{
		str[pow] = -(nb % 10) + '0';
		nb /= 10;
	}
	return (str - mod);
}

void		pad(size_t size)
{
	while (size--)
		ft_putf(" ");
}

uint16_t	get_columns(void)
{
	struct winsize	w;

	if (ioctl(0, TIOCGWINSZ, &w) == -1)
		return (0);
	return (w.ws_col);
}
