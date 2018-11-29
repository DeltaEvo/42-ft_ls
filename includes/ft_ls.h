/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/26 10:03:38 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/11/29 15:01:18 by dde-jesu         ###   ########.fr       */
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
	uint8_t	major;
	size_t	major_len;
	time_t	mtime;
	char	*link;
}				t_entry;

typedef struct	s_entries {
	size_t	len;
	size_t	capacity;
	t_entry	entries[];
}				t_entries;

typedef	struct	s_flags {
	char	*name;
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
	size_t	major;
	size_t	blocks;
}				t_max;

t_entries		*destroy_list(t_entries *list);
void			sort_entries(t_entries *tab, int (*cmp)(), bool rev);
t_entries		*collect_entries(char *p, size_t r_p, t_flags *f, t_max *s);
int				entry_name_cmp(t_entry *e1, t_entry *e2);
void			*ft_realloc(void *o_ptr, size_t oldsize, size_t newsize);
char			*path_join(char *path, size_t plen, char *n, size_t nlen);
void			pad(size_t size);
bool			is_special(char *name);
void			print_entry_long(t_flags *flags, t_entries *list, t_max *sizes);
void			print_name(t_entry *e, bool color);
t_entry			*add_entry(t_entries **list);
t_entries		*create_list(size_t capacity);
uint16_t		get_columns(void);
size_t			nb_len(uint64_t n);
char			*ft_itoal(int64_t nb, size_t *len);
char			*ft_strdupl(const char *src, size_t *len);
int				ls(t_flags *flags, char *path, size_t path_len);

#endif
