/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/22 09:49:06 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/11/23 16:24:58 by dde-jesu         ###   ########.fr       */
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
#include "entries.c"


extern int g_ft_optind;
bool is_special(char *name) {
	return (ft_strequ(".", name) || ft_strequ("..", name));
}

void pad(size_t size) {
	while (size--)
		ft_putf(" ");
}

void print_name(t_entry *entry, bool color)
{
	if (color)
		ft_putf("\x1b[%dm%s\x1b[0m", entry->type == DT_DIR ? 34 : 31,entry->name);
	else
		ft_putstr(entry->name);
}

void print_entry(t_flags *flags, t_entries *list, t_max_sizes *sizes)
{
	struct winsize	w;
	size_t			i;
	size_t 			j;
	t_entry			*it;
	uint16_t		columns;
	uint16_t		lines;
	uint16_t		last_cols;
	uint16_t		size;

	ioctl(0, TIOCGWINSZ, &w);
	columns = w.ws_col / (sizes->name + 1);
	lines = list->len / columns + !!(list->len % columns);
	i = 0;
	while (i < lines * columns)
	{
		j = i * lines % (lines * columns) + (i * lines) / (lines * columns);
		if (j < list->len) {
			it = list->entries + j;
			print_name(it, true);
			if ((i + 1) % columns != 0) {
				size = sizes->name - ((it->path + it->path_len) - it->name) + 1;
				pad(size);
			}
		}
		if ((i + 1) % columns == 0)
			ft_putf("\n");
		i++;
	}
}

void print_rwx(mode_t mode, bool color)
{
	if (color)
	{
		ft_putf("\x1b[%s\x1b[0m", mode & 0b100 ? "33mr" : "90m-");
		ft_putf("\x1b[%s\x1b[0m", mode & 0b10 ? "31mw" : "90m-");
		ft_putf("\x1b[%s\x1b[0m", mode & 0b1 ? "32mx" : "90m-");
	}
	else
	{
		ft_putf("%c", mode & 0b100 ? 'r' : '-');
		ft_putf("%c", mode & 0b10 ? 'w' : '-');
		ft_putf("%c", mode & 0b1 ? 'x' : '-');
	}
}

void print_date(char *date)
{
	date = date + 4;
	date[12] = 0;	
	ft_putf(" %s", date);
}


void print_entry_long(t_flags *flags, t_entries *list, t_max_sizes *sizes)
{
	size_t			i;
	t_entry			*it;

	i = 0;
	if (list->len)
		ft_putf("total %d\n", sizes->blocks);
	while (i < list->len)
	{
		it = list->entries + i;
		if (true)
			ft_putf("\x1b[%s\x1b[0m", it->type == DT_DIR ? "34md" : "90m-");
		else
			ft_putf("%c", it->type == DT_DIR ? 'd' : '-');
		print_rwx(it->mode >> 6, true);
		print_rwx(it->mode >> 3, true);
		print_rwx(it->mode, true);
		pad(sizes->nlink - it->nlink_len);
		ft_putf("  %d", it->nlink);
		pad(sizes->user - it->user_len);
		ft_putf(" %s", it->user);
		ft_putf("  %s", it->group);
		pad(sizes->group - it->group_len);
		pad(sizes->size - it->size_len);
		ft_putf("  %d", it->size);
		print_date(ctime(&it->mtime));
		ft_putf(" ");
		print_name(it, true);
		ft_putf("\n");
		i++;
	}
}

int	ls(char *name, t_flags *flags, char *path, size_t path_len)
{
	DIR *const		dir = opendir(path);
	t_entries	*list;
	size_t		i;
	t_max_sizes	sizes;

	if (!dir) {
		ft_putf("%s: %s: %s\n", name, path, strerror(errno));
		return (-1);
	}

	sizes = (t_max_sizes) {0,0,0,0,0,0};
	if(!(list = collect_entries(path, path_len, dir, flags, &sizes)))
	{
		closedir(dir);
		return (-1);
	}
	closedir(dir);
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

int	main(int argc, char **argv)
{
	char		c;
	char 		*name;
	t_flags		flags;
	int			ret;
	size_t		len;

	ret = 0;
	name = argv[0];
	flags = (t_flags) {0,0,0,0,0};
	while ((c = ft_getopt(argc, argv, "lRart")) != -1)
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
