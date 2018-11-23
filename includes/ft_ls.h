#ifndef FT_LS_H
# define FT_LS_H

#include <dirent.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/stat.h>

# define CACHE 64

typedef struct	s_entry {
	char 	*path;
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
}				t_flags;

typedef	struct	s_max_sizes {
	size_t	name;
	size_t	group;
	size_t	user;
	size_t	nlink;
	size_t	size;
	size_t	blocks;
}				t_max_sizes;

#endif
