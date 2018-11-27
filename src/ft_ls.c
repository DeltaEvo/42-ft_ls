/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/22 09:49:06 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/11/27 13:29:09 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <errno.h>
#include <string.h>

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

extern int g_ft_optind;

bool	is_special(char *name)
{
	return (ft_strequ(".", name) || ft_strequ("..", name));
}

void	pad(size_t size)
{
	while (size--)
		ft_putf(" ");
}

void	print_name(t_entry *e, bool color)
{
	if (color && g_colors[e->type] != 90)
		ft_putf("\x1b[%dm%s\x1b[0m", g_colors[e->type], e->name);
	else
		ft_putstr(e->name);
}

void	print_entry(t_flags *flags, t_entries *list, t_max *sizes)
{
	struct winsize	w;
	size_t			i;
	size_t			j;
	t_entry			*it;
	uint16_t		columns;
	uint16_t		lines;

	ioctl(0, TIOCGWINSZ, &w);
	columns = w.ws_col / (sizes->name + 1);
	lines = list->len / columns + !!(list->len % columns);
	i = 0;
	while (i < lines * columns)
	{
		j = i * lines % (lines * columns) + (i * lines) / (lines * columns);
		if (j < list->len)
		{
			it = list->entries + j;
			print_name(it, flags->color);
			if ((i + 1) % columns != 0)
				pad(sizes->name - ((it->path + it->path_len) - it->name) + 1);
		}
		if ((i + 1) % columns == 0)
			ft_putf("\n");
		i++;
	}
}

void	print_rwx(mode_t mode, bool set_id, bool color)
{
	if (color)
	{
		ft_putf("\x1b[%s\x1b[0m", mode & 0b100 ? "33mr" : "90m-");
		ft_putf("\x1b[%s\x1b[0m", mode & 0b10 ? "31mw" : "90m-");
		if (set_id)
			ft_putf("\x1b[%s\x1b[0m", mode & 0b1 ? "92ms" : "97mS");
		else
			ft_putf("\x1b[%s\x1b[0m", mode & 0b1 ? "32mx" : "90m-");
	}
	else
	{
		ft_putf("%c", mode & 0b100 ? 'r' : '-');
		ft_putf("%c", mode & 0b10 ? 'w' : '-');
		if (set_id)
			ft_putf("%c", mode & 0b1 ? 's' : 'S');
		else
			ft_putf("%c", mode & 0b1 ? 'x' : '-');
	}
}

void	print_date(time_t mtime)
{
	time_t	now;
	time_t	diff;
	char	*date;

	now = time(NULL);
	diff = now > mtime ? now - mtime : mtime - now;
	date = ctime(&mtime);
	date += 4;
	if (diff > 3600 * 24 * 30.5 * 6)
	{
		date[6] = 0;
		date[20] = 0;
		ft_putf(" %s %s", date, date + 15);
	}
	else
	{
		date[12] = 0;
		ft_putf(" %s", date);
	}
}



void	print_entry_long(t_flags *flags, t_entries *list, t_max *sizes)
{
	size_t			i;
	t_entry			*it;

	i = 0;
	if (list->len)
		ft_putf("total %d\n", sizes->blocks);
	while (i < list->len)
	{
		it = list->entries + i;
		if (flags->color)
			ft_putf("\x1b[%dm%c\x1b[0m", g_colors[it->type], g_summaries[it->type]);
		else
			ft_putf("%c", g_summaries[it->type]);
		print_rwx(it->mode >> 6, it->mode & S_ISUID, flags->color);
		print_rwx(it->mode >> 3, it->mode &  S_ISGID, flags->color);
		print_rwx(it->mode, 0, flags->color);
		pad(sizes->nlink - it->nlink_len);
		ft_putf("  %d", it->nlink);
		ft_putf(" %s", it->user);
		pad(sizes->user - it->user_len);
		ft_putf("  %s", it->group);
		pad(sizes->group - it->group_len);
		pad(sizes->size - it->size_len);
		ft_putf("  %d", it->size);
		print_date(it->mtime);
		ft_putf(" ");
		print_name(it, flags->color);
		if (it->type == DT_LNK)
			ft_putf(" -> %s", it->link);
		ft_putf("\n");
		i++;
	}
}

int		ls(char *name, t_flags *flags, char *path, size_t path_len)
{
	t_entries	*list;
	size_t		i;
	t_max		sizes;

	sizes = (t_max) {0, 0, 0, 0, 0, 0};
	if (!(list = collect_entries(path, path_len, flags, &sizes)))
	{
		if (errno)
		{
			ft_putf("%s: %s: %s\n", name, path, strerror(errno));
			errno = 0;
		}
		return (-1);
	}
	sort_entries(list, entry_name_cmp);
	if (flags->long_format)
		print_entry_long(flags, list, &sizes);
	else
		print_entry(flags, list, &sizes);
	i = 0;
	while (i < list->len)
	{
		if (flags->recursive && list->entries[i].type == DT_DIR && !is_special(list->entries[i].name))
		{
			ft_putf("\n%s:\n", list->entries[i].path);
			ls(name, flags, list->entries[i].path, list->entries[i].path_len);
		}
		i++;
	}
	destroy_list(list);
	return (0);
}

int		main(int argc, char **argv)
{
	char		c;
	char		*name;
	t_flags		flags;
	int			ret;
	size_t		len;

	ret = 0;
	name = argv[0];
	flags = (t_flags) {0, 0, 0, 0, 0, 0};
	while ((c = ft_getopt(argc, argv, "lRartG")) != -1)
	{
		if (c == 'l')
			flags.long_format = true;
		else if (c == 'R')
			flags.recursive = true;
		else if (c == 'a')
			flags.show_hidden = true;
		else if (c == 'r')
			flags.reverse = true;
		else if (c == 't')
			flags.sort_by_time = true;
		else if (c == 'G')
			flags.color = true;
	}
	argc -= g_ft_optind;
	argv += g_ft_optind;
	if (argc > 0)
	{
		while (argc--)
		{
			len = ft_strlen(*argv);
			ret |= ls(name, &flags, *argv, len - ((*argv)[len - 1] == '/'));
			argv++;
		}
		return (ret);
	}
	else
		return (ls(name, &flags, ".", 1));
}
