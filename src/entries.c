/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entries.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/22 12:05:26 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/11/23 17:10:55 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

char *path_join(char *path, size_t path_len, char *name, size_t name_len)
{
    char    *str;

    str = malloc(path_len + name_len + 2);
    ft_memcpy(str, path, path_len);
    str[path_len] = '/';
    ft_memcpy(str + path_len + 1, name, name_len);
    str[path_len + name_len + 1] = 0;
    return (str);
}

t_entries	*create_list(size_t capacity) {
	t_entries	*list;

	list = malloc(sizeof(t_entries) + capacity * sizeof(t_entry));
	*list = (t_entries) {
		.capacity = capacity,
		.len = 0
	};

	return (list);
}


void	destroy_list(t_entries *list)
{
	size_t i;
	t_entry	*ent;

	i = 0;
	while (i < list->len)
	{
		ent = list->entries + i;
		free(ent->path);
		free(ent->user);
		free(ent->group);
		i++;
	}
	free(list);
}

void	*ft_realloc(void *o_ptr, size_t oldsize, size_t newsize)
{
	void	*ptr;

	ptr = malloc(newsize);
	ft_memcpy(ptr, o_ptr, oldsize);
	free(o_ptr);
	return (ptr);
}

t_entry	*add_entry(t_entries **list)
{
	size_t	new_capacity;
	if ((*list)->len == (*list)->capacity)
	{
		new_capacity = (size_t)((float)(*list)->capacity * 2);
		*list = ft_realloc(*list, sizeof(t_entries) + (*list)->capacity * sizeof(t_entry), sizeof(t_entries) + new_capacity * sizeof(t_entry));
		(*list)->capacity = new_capacity;
	}
	return (*list)->entries + (*list)->len++;
}

char	*ft_strdupl(const char *src, size_t *len)
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

size_t	nb_len(int nb)
{
	size_t len;

	len = 1;
	while (nb /= 10)
		len++;
	return (len);
}

uint8_t	mode_to_type(mode_t mode)
{
	if (S_ISREG(mode))
		return (DT_REG);
	else if (S_ISDIR(mode))
		return (DT_DIR);
	else if (S_ISCHR(mode))
		return (DT_CHR);
	else if (S_ISBLK(mode))
		return (DT_BLK);
	else if (S_ISFIFO(mode))
		return (DT_FIFO);
	else if (S_ISLNK(mode))
		return (DT_LNK);
	else if (S_ISSOCK(mode))
		return (DT_SOCK);
	else
		return (DT_UNKNOWN);
}

bool collect_long(t_entry *entry, t_max_sizes *sizes)
{
	struct stat		f_stat;
	struct passwd	*pwd;
	struct group	*grp;

	if (lstat(entry->path, &f_stat) == -1)
	{
		ft_putf("ft_ls: %s: %s\n", entry->path, strerror(errno));
		return (false);
	}
	sizes->blocks += f_stat.st_blocks;
	if (entry->type == DT_UNKNOWN)
		entry->type = mode_to_type(f_stat.st_mode);
	entry->mode = f_stat.st_mode;
	entry->nlink = f_stat.st_nlink;
	entry->nlink_len = nb_len(entry->nlink);
	entry->mtime = f_stat.st_mtime;
	entry->size = f_stat.st_size;
	entry->size_len = nb_len(entry->size);
	pwd = getpwuid(f_stat.st_uid);
	entry->user = pwd ? ft_strdupl(pwd->pw_name, &entry->user_len) : (entry->user_len = nb_len(f_stat.st_uid), ft_itoa(f_stat.st_uid));
	grp = getgrgid(f_stat.st_gid);
	entry->group = grp ? ft_strdupl(grp->gr_name, &entry->group_len) : (entry->user_len = nb_len(f_stat.st_gid), ft_itoa(f_stat.st_gid));
	if (entry->user_len > sizes->user)
		sizes->user = entry->user_len;
	if (entry->group_len > sizes->group)
		sizes->group = entry->group_len;
	if (entry->size_len > sizes->size)
		sizes->size = entry->size_len;
	if (entry->nlink_len > sizes->nlink)
		sizes->nlink = entry->nlink_len;
	return (true);
}

t_entries	*collect_entries(char *r_path, size_t r_path_len, DIR* dir, t_flags *flags, t_max_sizes *sizes) {
	struct dirent	*d_entry;
	t_entry			*entry;
	t_entries		*list;
	char			*path;
	
	list = create_list(10);
	while ((d_entry = readdir(dir))) {
		if (flags->show_hidden || d_entry->d_name[0] != '.') {
			entry = add_entry(&list);
			path = path_join(r_path, r_path_len, d_entry->d_name, d_entry->d_namlen);
			*entry = (t_entry) {
				.path = path,
				.path_len = r_path_len + d_entry->d_namlen + 1,
				.name = path + r_path_len + 1,
				.type = d_entry->d_type
			};
			if (d_entry->d_namlen > sizes->name)
				sizes->name = d_entry->d_namlen;
			if (flags->long_format || entry->type == DT_UNKNOWN)
				if (!collect_long(entry, sizes))
				{
					destroy_list(list);
					return (NULL);
				}
		}
	}
	return (list);
}
