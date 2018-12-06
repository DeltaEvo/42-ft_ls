/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collect.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/28 10:13:19 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/12/06 13:19:09 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

uint8_t		mode_to_type(mode_t mode)
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

bool		collect_max(t_entry *entry, t_max *sizes)
{
	if (entry->user_len > sizes->user)
		sizes->user = entry->user_len;
	if (entry->group_len > sizes->group)
		sizes->group = entry->group_len;
	if (entry->size_len > sizes->size)
		sizes->size = entry->size_len;
	if (entry->nlink_len > sizes->nlink)
		sizes->nlink = entry->nlink_len;
	if (entry->major_len > sizes->major)
		sizes->major = entry->major_len;
	return (true);
}

bool		collect_long(t_entry *entry, t_max *sizes, struct stat *f_stat)
{
	const struct passwd	*pwd = getpwuid(f_stat->st_uid);
	const struct group	*grp = getgrgid(f_stat->st_gid);
	char				link[1024];
	ssize_t				ret;

	entry->size = f_stat->st_size;
	if (entry->type == DT_LNK)
	{
		if ((ret = readlink(entry->path, link, sizeof(link) - 1)) == -1)
			return (false);
		link[ret] = 0;
		entry->link = ft_strdup(link);
	}
	else if (entry->type == DT_BLK || entry->type == DT_CHR)
	{
		entry->size = f_stat->st_rdev & 0xFF;
		entry->major = f_stat->st_rdev >> 24;
		entry->major_len = nb_len(entry->major);
	}
	entry->size_len = nb_len(entry->size);
	entry->user = pwd ? ft_strdupl(pwd->pw_name, &entry->user_len)
					: ft_itoal(f_stat->st_uid, &entry->user_len);
	entry->group = grp ? ft_strdupl(grp->gr_name, &entry->group_len)
					: ft_itoal(f_stat->st_gid, &entry->group_len);
	return (collect_max(entry, sizes));
}

bool		collect_infos(uint16_t namlen, t_entry *entry, t_flags *flags,
		t_max *sizes)
{
	struct stat		f_stat;

	if (namlen > sizes->name)
		sizes->name = namlen;
	if (!flags->long_format && entry->type != DT_UNKNOWN && !flags->color)
		return (true);
	if (lstat(entry->path, &f_stat) == -1)
		return (false);
	if (entry->type == DT_UNKNOWN)
		entry->type = mode_to_type(f_stat.st_mode);
	entry->mode = f_stat.st_mode;
	entry->mtime = f_stat.st_mtime;
	if (flags->long_format)
	{
		sizes->blocks += f_stat.st_blocks;
		entry->major = 0;
		entry->major_len = 0;
		entry->nlink = f_stat.st_nlink;
		entry->nlink_len = nb_len(entry->nlink);
		return (collect_long(entry, sizes, &f_stat));
	}
	return (true);
}

t_entries	*collect_entries(char *r_path, size_t r_path_len, t_flags *flags,
		t_max *sizes)
{
	DIR *const		dir = opendir(r_path);
	struct dirent	*d_ent;
	t_entry			*entry;
	t_entries		*list;
	char			*path;

	if (!dir || !(list = create_list(10)))
		return (NULL);
	while ((d_ent = readdir(dir)))
	{
		if (!flags->show_hidden && d_ent->d_name[0] == '.')
			continue;
		entry = add_entry(&list);
		path = path_join(r_path, r_path_len, d_ent->d_name, d_ent->d_namlen);
		*entry = (t_entry) { .path = path, .name = path + r_path_len + 1,
		.path_len = r_path_len + d_ent->d_namlen + 1, .type = d_ent->d_type };
		if (!collect_infos(d_ent->d_namlen, entry, flags, sizes))
		{
			ft_putf_fd(2, "%s: %s: %s\n", flags->name, path, strerror(errno));
			closedir(dir);
			return (destroy_list(list));
		}
	}
	closedir(dir);
	return (list);
}
