#include "fs.h"

// Include built-in executables and import `LAST_COMMMAND`
#include "bin.c"

// Base ROOTFS definition

/// Executables

Dir bin = {
	.base = {
		.type = t_DIR,
		.name = {"bin", 3},
		.sibling = NULL
	},
	.content = LAST_COMMAND,
};

Text hello = {
	.base = {
		.type = t_TEXT,
		.name = {"hello", 5},
		.sibling = NULL
	},
	.content = &newstr("Hello World!", 12),
};

/// User personal files
Dir home = {
	.base = {
		.type = t_DIR,
		.name = {"home", 4},
		.sibling = (File*) &bin
	},
	.content = (File*) &hello
};

/// Moar Executables
Link usr_bin = {
	.base = {
		.type = t_LINK,
		.name = {"bin", 3},
		.sibling = NULL
	},
    .content = (File*) &bin
};

/// Moar user files
Link usr_share = {
	.base = {
		.type = t_LINK,
		.name = {"share", 5},
        .sibling = (File*) &usr_bin
	},
    .content = (File*) &home,
};

/// Moar user files
Dir usr = {
	.base = {
		.type = t_DIR,
		.name = {"usr", 3},
        .sibling = (File*) &home
	},
    .content = (File*) &usr_share
};

/// Filesystem root
Dir root = {
	.base = {
		.type = t_DIR,
		.name = {"/", 1},
		.sibling = NULL
	},
    .content = (File*) &usr
};

/**
 * The current working directory
 */
volatile Dir* cwd = &root;

/**
 * Fetch a file by the path given
 *
 * @param name  a non-null string of a file name
 *              returns `NULL` if empty
 *
 * @return the file in question,
 *         or `NULL` if not found
 *
 */

File* fopen(String name) {
	usize_t i;
	File* current;
    String pathname;

    if (name.size == 0) {
		return NULL;
	}

    if (name.str[0] == '/') {
        current = (File*) &root;
		i = 1;
	}
	else {
        current = (File*) cwd;
        if (name.str[0] == '.' && name.str[1] == '/') { // "./"
			i = 2;
		}
		else {
			i = 0;
		}
	}
    if (name.str[name.size-1] == '/') { // valid but not helpful
    	name.size -= 1;
    }

    name.str += i;
    name.size -= i;

    while(name.size != 0 && current != NULL) {
        tokenize(&name, '/', &pathname);
        current = fopenlocal(&pathname, (Dir*) current);
	}

	return current;
}

/**
 * Fetch a file in the specified directory
 *
 * @param name  a non-null string of a file name
 *              returns `NULL` if empty
 * @param dir   where the file can be found
 *              returns `NULL` if NULL or non-dir
 *
 * @return the file in question,
 *         or `NULL` if not found
 *
 */

File* fopenlocal(const String* name, const Dir* dir) {
	File* current;

	if (dir == NULL || dir->base.type != t_DIR) {
		return NULL;
	}

	for (current = dir->content; current != NULL; current = current->sibling) {
        if(streq(&(current->name), name)) {
            if(current->type == t_LINK) {
                current = ((Link*) current)->content;
            }
			return current;
		}
	}
	return NULL;
}
