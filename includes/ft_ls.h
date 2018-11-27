/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/26 10:03:38 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/11/27 12:40:21 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_LS_H
# define FT_LS_H

# include <dirent.h>
# include <stdint.h>
# include <stddef.h>
# include <stdbool.h>
# include <sys/stat.h>

# define CACHE 64

typedef struct	s_entry {
	char	*path;
	size_t	path_len;
	char	*name;
	uint8_t	type;
	char	*user;
	size_t	user_len;
	char	*group;
	size_t	group_len;
	mode_t	mode;
	nlink_t	nlink;
	size_t	nlink_len;
	off_t	size;
	size_t	size_len;
	time_t	mtime;
	char	*link;
}				t_entry;

typedef struct	s_entries {
	size_t	len;
	size_t	capacity;
	t_entry	entries[];
}				t_entries;

typedef	struct	s_flags {
	bool	long_format;
	bool	recursive;
	bool	show_hidden;
	bool	reverse;
	bool	sort_by_time;
	bool	color;
}				t_flags;

typedef	struct	s_max {
	size_t	name;
	size_t	group;
	size_t	user;
	size_t	nlink;
	size_t	size;
	size_t	blocks;
}				t_max;

void			destroy_list(t_entries *list);
t_entries		*collect_entries(char *p, size_t r_p, t_flags *f, t_max *s);

int     entry_name_cmp(t_entry *e1, t_entry *e2);
void    sort_entries(t_entries *tab, int (*cmp)(t_entry *e1, t_entry *e2));

#endif
