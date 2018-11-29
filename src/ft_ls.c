/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/22 09:49:06 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/11/29 13:10:14 by dde-jesu         ###   ########.fr       */
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

int		recursive_ls(t_flags *f, t_entries *l)
{
	size_t		i;
	int			ret;

	i = 0;
	ret = 0;
	while (i < l->len)
	{
		if (l->entries[i].type == DT_DIR && !is_special(l->entries[i].name))
		{
			ft_putf("\n%s:\n", l->entries[i].path);
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
		ret = recursive_ls(flags, list);
	destroy_list(list);
	return (ret);
}

int		multiple_files(t_flags *flags, char **files, int size)
{
	int			ret;
	size_t		len;

	ret = 0;
	while (size--)
	{
		len = ft_strlen(*files);
		ret |= ls(flags, *files, len - ((*files)[len - 1] == '/'));
		files++;
	}
	return (ret);
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
	if (argc > 0)
		return (multiple_files(&flags, argv, argc));
	return (ls(&flags, ".", 1));
}
