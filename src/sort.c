/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/06 17:25:40 by dde-jesu          #+#    #+#             */
/*   Updated: 2018/12/10 14:56:06 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "ft/str.h"

int	entry_name_cmp(t_entry *e1, t_entry *e2)
{
	return (ft_strcmp(e1->name, e2->name));
}

int	entry_time_cmp(t_entry *e1, t_entry *e2)
{
	return (e2->time - e1->time);
}
