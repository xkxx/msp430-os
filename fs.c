#include "fs.h"

#include "bin.c"

const Dir bin = {
	.base = {
		.type = t_DIR,
		.name = "bin",
		.sibling = NULL
	},
	.content = LAST_COMMAND,
};

const Text hello = {
	.base = {
		.type = t_TEXT,
		.name = "hello",
		.sibling = NULL
	},
	.content = "Hello World!",
};

const Dir home = {
	.base = {
		.type = t_DIR,
		.name = "home",
		.sibling = (File*) &bin
	},
	.content = (File*) &hello
};

const Link usr_bin = {
	.base = {
		.type = t_LINK,
		.name = "bin",
		.sibling = NULL
	},
    .content = (File*) &bin
};

const Link usr_share = {
	.base = {
		.type = t_LINK,
		.name = "share",
        .sibling = (File*) &usr_bin
	},
    .content = (File*) &home,
};

const Dir usr = {
	.base = {
		.type = t_DIR,
		.name = "usr",
        .sibling = (File*) &home
	},
    .content = (File*) &usr_share
};

Dir root = {
	.base = {
		.type = t_DIR,
		.name = "/",
		.sibling = NULL
	},
    .content = (File*) &usr
};

volatile Dir* cwd = &root;

File* fopen(const String* const name) {
    if (name->size == 0) {
		return NULL;
	}
	File* current;
	usize_t i;
    String pathname;
    if (name->str[0] == '/') {
        current = (File*) &root;
		i = 1;
	}
	else {
        if (name->str[0] == '.') { // "./"
			i = 2;
		}
		else {
			i = 0;
		}
        current = (File*) cwd;
	}
    for (; i < name->size; i += pathname.size + 1) {
        pathname.str = name->str + i;
        pathname.size = nexttoklen(pathname.str, '/', name->size);

        current = fopenLoc(&pathname, (Dir*) current);

		if (current == NULL) {
			break;
		}

	}
	return current;
}

File* fopenLoc(const String* const name, const Dir* const dir) {
	if (dir == NULL || dir->base.type != t_DIR) {
		return NULL;
	}

	File* current;
	for (current = dir->content; current != NULL; current = current->sibling) {
        if(streq(current->name, name)) {
            if(current->type == t_LINK) {
                current = ((Link*) current)->content;
            }
			return current;
		}
	}
	return NULL;
}
