/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   long_format.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/28 10:10:19 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/11/29 15:06:48 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include <time.h>

static const char g_summaries[] = {
	[DT_UNKNOWN] = '-',
	[DT_FIFO] = 'p',
	[DT_CHR] = 'c',
	[DT_DIR] = 'd',
	[DT_BLK] = 'b',
	[DT_REG] = '-',
	[DT_LNK] = 'l',
	[DT_SOCK] = 's'
};

static const char g_colors[] = {
	[DT_UNKNOWN] = 90,
	[DT_FIFO] = 93,
	[DT_CHR] = 33,
	[DT_DIR] = 34,
	[DT_BLK] = 33,
	[DT_REG] = 90,
	[DT_LNK] = 35,
	[DT_SOCK] = 36
};

void	print_name(t_entry *e, bool color)
{
	if (color && g_colors[e->type] != 90)
		ft_putf("\x1b[%dm%s\x1b[0m", g_colors[e->type], e->name);
	else
		ft_putstr(e->name);
}

void	print_rwx(mode_t m, char s, bool color)
{
	if (!color)
	{
		ft_putf("%c", m & 0b100 ? 'r' : '-');
		ft_putf("%c", m & 0b10 ? 'w' : '-');
		if (s)
			ft_putf("%c", m & 0b1 ? s : s - 32);
		else
			ft_putf("%c", m & 0b1 ? 'x' : '-');
		return ;
	}
	ft_putf("\x1b[%s\x1b[0m", m & 0b100 ? "33mr" : "90m-");
	ft_putf("\x1b[%s\x1b[0m", m & 0b10 ? "31mw" : "90m-");
	if (s)
		ft_putf("\x1b[%dm%c\x1b[0m", m & 0b1 ? 92 : 97, m & 0b1 ? s : s - 32);
	else
		ft_putf("\x1b[%s\x1b[0m", m & 0b1 ? "32mx" : "90m-");
}

void	print_size_and_date(t_entry *e, t_max *sizes)
{
	time_t	now;
	time_t	diff;
	char	*date;

	pad(sizes->size - e->size_len);
	ft_putf(" %d", e->size);
	now = time(NULL);
	diff = now > e->mtime ? now - e->mtime : e->mtime - now;
	date = ctime(&e->mtime);
	date += 4;
	if (diff > 3600 * 24 * 30.5 * 6)
	{
		date[6] = 0;
		date[20] = 0;
		ft_putf(" %s %s ", date, date + 15);
	}
	else
	{
		date[12] = 0;
		ft_putf(" %s ", date);
	}
}

void	print_infos(t_entry *e, t_flags *flags, t_max *sizes)
{
	if (flags->color)
		ft_putf("\x1b[%dm%c\x1b[0m", g_colors[e->type], g_summaries[e->type]);
	else
		ft_putf("%c", g_summaries[e->type]);
	print_rwx(e->mode >> 6, e->mode & S_ISUID ? 's' : 0, flags->color);
	print_rwx(e->mode >> 3, e->mode & S_ISGID ? 's' : 0, flags->color);
	print_rwx(e->mode, e->mode & S_ISVTX ? 't' : 0, flags->color);
	pad(sizes->nlink - e->nlink_len);
	ft_putf("  %d %s", e->nlink, e->user);
	pad(sizes->user - e->user_len);
	ft_putf("  %s", e->group);
	pad(sizes->group - e->group_len + 1);
	if (e->type == DT_BLK || e->type == DT_CHR)
	{
		pad(sizes->major - e->major_len + 2);
		ft_putf("%d,", e->major);
	}
	else if (sizes->major)
		pad(sizes->major + 3);
	print_size_and_date(e, sizes);
	print_name(e, flags->color);
	if (e->type == DT_LNK)
		ft_putf(" -> %s", e->link);
	ft_putf("\n");
}

void	print_entry_long(t_flags *flags, t_entries *list, t_max *sizes)
{
	size_t	i;

	i = 0;
	if (list->len)
		ft_putf("total %d\n", sizes->blocks);
	while (i < list->len)
		print_infos(list->entries + i++, flags, sizes);
}
