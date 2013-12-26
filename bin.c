#include "msp430.h"
#include "std.h"
#include "fs.h"
#include "string.h"
#include "malloc.h"

#define LAST_COMMAND NULL

static usize_t yes(const String* const args) {
	put("y\r\n", 3);
	return 0;
}

static const Exec b_yes = {
	.base = {
		.type = t_EXEC,
		.name = "yes",
		.sibling = LAST_COMMAND
	},
	.content = yes,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_yes

static usize_t uname(const String* const args) {
	put("Unix v-.1 MSP430G2553\r\n", 23);
	return 0;
}

static const Exec b_uname = {
	.base = {
		.type = t_EXEC,
		.name = "uname",
		.sibling = LAST_COMMAND
	},
	.content = uname,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_uname

static usize_t red(const String* const args) {
    if (streq("on", args)) {
    	P1OUT |= BIT0;
    }
    else if (streq("off", args)) {
    	P1OUT &= ~BIT0;
    }
    else {
        put("type on or off\r\n", 16);
    }
    return 0;
}

static const Exec b_red = {
    .base = {
        .type = t_EXEC,
        .name = "red",
        .sibling = LAST_COMMAND
    },
    .content = red,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_red

static usize_t green(const String* const args) {
    if (streq("on", args)) {
    	P1OUT |= BIT6;
    }
    else if (streq("off", args)) {
    	P1OUT &= ~BIT6;
    }
    else {
        put("type on or off\r\n", 16);
    }
    return 0;
}

static const Exec b_green = {
    .base = {
        .type = t_EXEC,
        .name = "green",
        .sibling = LAST_COMMAND
    },
    .content = green,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_green

static usize_t pwd(const String* const args) {
	put(cwd->base.name, 20);
	put("\r\n", 2);
	return 0;
}

static const Exec b_pwd = {
	.base = {
		.type = t_EXEC,
		.name = "pwd",
		.sibling = LAST_COMMAND
	},
	.content = pwd,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_pwd

static usize_t cd(const String* const args) {
	File* dir;
    if(args->size == 0) {
		dir = (File*) &root;
	}
	else {
        dir = fopen(args);
	}
	if (dir == NULL) {
        put("File not found\r\n", 16);
	}
	else if (dir->type != t_DIR) {
		put("File not dir\r\n", 14);
	}
	else {
		cwd = (Dir*) dir;
	}
	return 0;
}

static const Exec b_cd = {
	.base = {
		.type = t_EXEC,
		.name = "cd",
		.sibling = LAST_COMMAND
	},
	.content = cd,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_cd

static void listFiles(const Dir* const dir) {
	File* cur = dir->content;
	for(cur = dir->content; cur != NULL; cur = cur->sibling) {
		put(cur->name, 20);
		if (cur->type == t_DIR) {
			put("/", 1);
		}
		put("\r\n", 2);
	}
}

static usize_t ls(const String* const args) {
	File* dir;
    if (args->size == 0) {
		dir = (File*) cwd;
		}
	else {
        dir = fopen(args);
	}

	if (dir == NULL) {
        put("File not found\r\n", 16);
	}
	else if (dir->type != t_DIR) {
		put("File not dir\r\n", 14);
	}
	else {
		listFiles(dir);
	}
	return 0;
}

static const Exec b_ls = {
	.base = {
		.type = t_EXEC,
		.name = "ls",
		.sibling = LAST_COMMAND
	},
	.content = ls,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_ls

static usize_t ln(const String* const args) {
//    char* src_str = args;
//    usize_t len = argc;
//    if (len == 0) {
//        put("Source?\r\n", 9);
//        src_str = get(&len);
//    }
//    File* src = fopen(src_str, len);
//    if (src == NULL) {
//        put("File not found", 16);
//        return;
//    }

//    put("Target?\r\n", 9);
//    char* dst_str = get(&len);
//    if (dst == NULL) {
//        put("", 16);
//        return;
//    }

}

static usize_t mkdir(const String* const args) {
    char* name = malloc(args->size + 1);
    strncpy(name, args->str, args->size);
    Dir* dir = malloc(sizeof(Dir));
    dir->base.name = name;
    dir->base.sibling = cwd->content;
    dir->base.type = t_DIR;
    dir->content = NULL;
    cwd->content = (File*) dir;
    return 0;
}

static const Exec b_mkdir = {
    .base = {
        .type = t_EXEC,
        .name = "mkdir",
        .sibling = LAST_COMMAND
    },
    .content = mkdir,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_mkdir

static usize_t memtest(const String* const args) {
	char* stg = malloc(20);
	int i = 0;
	for (i = 0; i < 19; i++) {
		stg[i] = '0';
	}
	stg[i] = '\0';
	put(stg, 19);
    free(stg);
	return 0;
}

static const Exec b_memtest = {
	.base = {
		.type = t_EXEC,
		.name = "memtest",
		.sibling = LAST_COMMAND
	},
	.content = memtest,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_memtest

extern const Dir bin;

static usize_t sh(const String* const args) {
	while (1) {
		put("> ", 2);

        String cmd, args;
        get(&cmd);

        if (cmd.size == 0) {
			continue;
		}
        usize_t end = nexttoklen(cmd.str, ' ', cmd.size);

        if (end == cmd.size) {
            args.str = cmd.str + cmd.size; // '\0'
            args.size = 0;
        }
        else {
            args.str = cmd.str + end + 1;
            args.size = cmd.size - end - 1;
        }
        cmd.size = end;

        Exec* prog = (Exec*) fopenLoc(&cmd, &bin);
		if (prog == NULL) {
			put("Command not found\r\n", 19);
		}
		else {
            prog->content(&args);
			continue;
		}
	}
}

static const Exec b_sh = {
	.base = {
		.type = t_EXEC,
		.name = "sh",
		.sibling = LAST_COMMAND
	},
	.content = sh,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_sh
