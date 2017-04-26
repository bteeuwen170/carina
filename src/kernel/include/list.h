/*
 *
 * Elarix
 * src/kernel/include/list.h
 *
 * Copyright (C) 2016 - 2017 Bastiaan Teeuwen <bastiaan@mkcl.nl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 */

#ifndef _LIST_H
#define _LIST_H

struct list_head {
	struct list_head *prev, *next;
};

#define LIST_INIT(head)		{ &(head), &(head) }

#define LIST_HEAD(head)		struct list_head (head) = LIST_INIT((head))

static inline void list_init(struct list_head *head)
{
	if (!head)
		return;

	head->prev = head;
	head->next = head;
}

static inline void list_add(struct list_head *next, struct list_head *entry)
{
	if (!next || !entry)
		return;

	next->prev->next = entry;
	next->prev = entry;

	entry->prev = next->prev;
	entry->next = next;

	/* next->next->prev = new;

	new->prev = next;
	new->next = next->next; */
}

static inline void list_adda(struct list_head *prev, struct list_head *entry)
{
	if (!prev || !entry)
		return;

	entry->prev = prev;
	entry->next = prev->next;

	prev->next->prev = entry;
	prev->next = entry;
}

static inline void list_mv(struct list_head *next, struct list_head *entry)
{
	if (!next || !entry)
		return;

	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;

	list_add(next, entry);
}

static inline void list_mva(struct list_head *prev, struct list_head *entry)
{
	if (!prev || !entry)
		return;

	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;

	list_adda(prev, entry);
}

static inline void list_rm(struct list_head *entry)
{
	if (!entry)
		return;

	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;

	entry->prev = entry->next = NULL;
}

#define list_entry(entry, par, name) \
	((par *) ((char *) (entry) - (uintptr_t) (&((par *)0)->name)))

#define list_for_each(par, head, name) \
	for ((par) = list_entry((head)->next, typeof(*(par)), name); \
			&(par)->name != (head); (par) = \
			list_entry((par)->name.next, typeof(*(par)), name))


#endif
