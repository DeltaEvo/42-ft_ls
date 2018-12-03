/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/22 09:49:06 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/11/30 16:52:47 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

extern int g_ft_optind;

void	print_entry(t_flags *flags, t_entries *list, t_max *sizes)
{
	size_t			i;
	size_t			j;
	t_entry			*it;
	uint16_t		columns;
	uint16_t		lines;

	columns = get_columns() / (sizes->name + 1);
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

int		recursive_ls(t_flags *f, t_entries *l, bool first_nl)
{
	size_t		i;
	int			ret;

	i = 0;
	ret = 0;
	while (i < l->len)
	{
		if (l->entries[i].type == DT_DIR && !is_special(l->entries[i].name))
		{
			if (first_nl)
				ft_putf("\n");
			ft_putf("%s:\n", l->entries[i].path);
			ret |= ls(f, l->entries[i].path, l->entries[i].path_len);
		}
		i++;
	}
	return (ret);
}

int		ls(t_flags *flags, char *path, size_t path_len)
{
	t_entries	*list;
	t_max		sizes;
	int			ret;

	sizes = (t_max) {0, 0, 0, 0, 0, 0, 0};
	ret = 0;
	if (!(list = collect_entries(path, path_len, flags, &sizes)))
		return (-1);
	sort_entries(list, entry_name_cmp, flags->reverse);
	(flags->long_format ? print_entry_long : print_entry)(flags, list, &sizes);
	if (flags->recursive)
		ret = recursive_ls(flags, list, true);
	destroy_list(list);
	return (ret);
}

int		handle_args(t_flags *f, char **args, int size, int i)
{
	t_entries *const	files = create_list(10);
	t_entries *const	dirs = create_list(10);
	t_entry				*e;
	struct stat			f_stat;
	t_max				f_sizes;

	f_sizes = (t_max) {0, 0, 0, 0, 0, 0, 0};
	while (i < size)
	{
		stat(args[i], &f_stat);
		e = add_entry((t_entries **)(S_ISDIR(f_stat.st_mode) ? &dirs : &files));
		*e = (t_entry) { .path = args[i], .name = args[i],
		.path_len = ft_strlen(args[i]), .type = mode_to_type(f_stat.st_mode)};
		collect_infos(e->path_len, e, f, &f_sizes);
		i++;
	}
	sort_entries(files, entry_name_cmp, f->reverse);
	sort_entries(dirs, entry_name_cmp, f->reverse);
	if (files->len)
		(f->long_format ? print_entry_long : print_entry)(f, files, &f_sizes);
	if (dirs->len > 1)
		return (recursive_ls(f, dirs, files->len));
	return (ls(f, dirs->entries->path, dirs->entries->path_len));
}

int		main(int argc, char **argv)
{
	char		c;
	t_flags		flags;

	flags = (t_flags) {argv[0], 0, 0, 0, 0, 0, isatty(1)};
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
	if (argc)
		return (handle_args(&flags, argv, argc, 0));
	return (ls(&flags, ".", 1));
}
