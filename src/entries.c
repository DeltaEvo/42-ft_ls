/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entries.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/22 12:05:26 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/11/29 15:00:59 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include <stdlib.h>
#include <dirent.h>

t_entries	*create_list(size_t capacity)
{
	t_entries	*list;

	list = malloc(sizeof(t_entries) + capacity * sizeof(t_entry));
	*list = (t_entries) {
		.capacity = capacity,
			.len = 0
	};
	return (list);
}

t_entries	*destroy_list(t_entries *list)
{
	size_t	i;
	t_entry	*ent;

	i = 0;
	while (i < list->len)
	{
		ent = list->entries + i;
		free(ent->path);
		free(ent->user);
		free(ent->group);
		if (ent->type == DT_LNK)
			free(ent->link);
		i++;
	}
	free(list);
	return (NULL);
}

t_entry		*add_entry(t_entries **list)
{
	size_t	new_capacity;

	if ((*list)->len == (*list)->capacity)
	{
		new_capacity = (*list)->capacity * 2;
		*list = ft_realloc(*list,
				sizeof(t_entries) + (*list)->capacity * sizeof(t_entry),
				sizeof(t_entries) + new_capacity * sizeof(t_entry));
		(*list)->capacity = new_capacity;
	}
	return (*list)->entries + (*list)->len++;
}

int			entry_name_cmp(t_entry *e1, t_entry *e2)
{
	return (strcmp(e1->name, e2->name));
}

void		sort_entries(t_entries *tab, int (*cmp)(), bool rev)
{
	size_t	i;
	size_t	j;
	t_entry	tmp;
	int		ret;

	if (tab->len < 2)
		return ;
	i = tab->len;
	while (--i)
	{
		j = -1;
		while (++j < i)
		{
			ret = cmp(tab->entries + j + 1, tab->entries + j);
			if (rev ? ret > 0 : ret < 0)
			{
				tmp = tab->entries[j + 1];
				tab->entries[j + 1] = tab->entries[j];
				tab->entries[j] = tmp;
			}
		}
	}
}
