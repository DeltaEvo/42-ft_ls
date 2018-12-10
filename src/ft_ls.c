/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/22 09:49:06 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/12/10 14:59:51 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include "ft/args.h"
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void	print_entry(t_flags *flags, t_entries *list, t_max *sizes, bool d)
{
	size_t			i;
	size_t			j;
	t_entry			*it;
	uint16_t		columns;
	uint16_t		lines;

	(void)d;
	columns = get_columns() / (sizes->name + 1);
	columns = columns ? columns : 1;
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
			else
				first_nl = true;
			ft_putf("%s:\n", l->entries[i].path);
			ret |= ls(f, l->entries[i].path, l->entries[i].path_len);
		}
		i++;
	}
	return (ret);
}

int		ls(t_flags *f, char *path, size_t path_len)
{
	t_entries	*list;
	t_max		sizes;
	int			ret;

	sizes = (t_max) {0, 0, 0, 0, 0, 0, 0};
	ret = 0;
	if (!(list = collect_entries(path, path_len, f, &sizes)))
		return (1);
	sort_entries(list, f->sort, f->reverse);
	(f->long_format ? print_entry_long : print_entry)(f, list, &sizes, 1);
	if (f->recursive)
		ret = recursive_ls(f, list, true);
	destroy_list(list);
	return (ret);
}

int		handle_args(t_flags *f, char **args, int size, int i)
{
	t_entries *const	file = create_list(10);
	t_entries *const	dir = create_list(10);
	t_entry				*e;
	struct stat			f_s;
	t_max				f_size;

	f_size = (t_max) {0, 0, 0, 0, 0, 0, 0};
	while (++i < size)
		if ((f->follow ? stat : lstat)(args[i], &f_s) == -1)
			ft_putf_fd(2, "%s: %s: %s\n", f->name, args[i], strerror(errno));
		else
		{
			e = add_entry((t_entries **)(S_ISDIR(f_s.st_mode) ? &dir : &file));
			*e = (t_entry) { .path = args[i], .name = args[i],
			.path_len = ft_strlen(args[i]), .type = mode_to_type(f_s.st_mode)};
			collect_infos(e->path_len, e, f, &f_size);
			e->path_len -= e->path[e->path_len - 1] == '/';
		}
	sort_entries(file, f->sort, f->reverse);
	sort_entries(dir, f->sort, f->reverse);
	if (file->len)
		(f->long_format ? print_entry_long : print_entry)(f, file, &f_size, 0);
	if (dir->len > 1 || (dir->len == 1 && file->len))
		return (recursive_ls(f, dir, file->len));
	return (ls(f, dir->entries->path, dir->entries->path_len));
}

int		main(int argc, char *argv[])
{
	t_flags		flags;
	bool		sort_by_time;
	int			ret;
	const t_arg	args[] = {
		{ ARG_BOOLEAN, 'l', "long", &flags.long_format, NULL },
		{ ARG_BOOLEAN, 'R', "recursive", &flags.recursive, NULL },
		{ ARG_BOOLEAN, 'a', "all", &flags.show_hidden, NULL },
		{ ARG_BOOLEAN, 'r', "reverse", &flags.reverse, NULL },
		{ ARG_BOOLEAN, 't', "time", &sort_by_time, NULL },
		{ ARG_BOOLEAN, 'G', "color", &flags.color, NULL },
		{ ARG_BOOLEAN, 'n', "numeric-uid-gid", &flags.uid, NULL },
		{ ARG_BOOLEAN, 'H', "follow-links", &flags.follow, NULL },
		{ ARG_BOOLEAN, 'u', "last-access", &flags.last_access, NULL },
		{ ARG_BOOLEAN, 'U', "file-creation", &flags.file_creation, NULL },
	};

	sort_by_time = 0;
	flags = (t_flags) {argv[0], 0, 0, 0, 0, 0, isatty(1), 0, 0, 0, 0};
	if ((ret = parse_args(args, argc, argv)) < 0)
		return (1);
	flags.sort = sort_by_time ? entry_time_cmp : entry_name_cmp;
	flags.follow |= !flags.long_format;
	if (ret != argc)
		return (handle_args(&flags, argv + ret, argc - ret, -1) ? 1 : !!errno);
	return (ls(&flags, ".", 1));
}
