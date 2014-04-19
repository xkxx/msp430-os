#ifndef PC
	#include "msp430.h"
#endif
#include "std.h"
#include "fs.h"
#include "string.h"
#include "malloc.h"

#define LAST_COMMAND NULL

static usize_t yes(const String* args) {
	putraw("y\r\n", 3);
	return 0;
}

static const Exec b_yes = {
	.base = {
		.type = t_EXEC,
		.name = {"yes", 3},
		.sibling = LAST_COMMAND
	},
	.content = yes,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_yes

static usize_t uname(const String* args) {
	putraw("Unix v-.1 MSP430G2553\r\n", 23);
	return 0;
}

static const Exec b_uname = {
	.base = {
		.type = t_EXEC,
		.name = {"uname", 5},
		.sibling = LAST_COMMAND
	},
	.content = uname,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_uname

static usize_t red(const String* args) {
    if (streq(&(String) newstr("on", 2), args)) {
    	P1OUT |= BIT0;
    }
    else if (streq(&(String) newstr("off", 3), args)) {
    	P1OUT &= ~BIT0;
    }
    else {
        putraw("type on or off\r\n", 16);
    }
    return 0;
}

static const Exec b_red = {
    .base = {
        .type = t_EXEC,
        .name = {"red", 3},
        .sibling = LAST_COMMAND
    },
    .content = red,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_red

static usize_t green(const String* args) {
    if (streq(&(String) newstr("on", 2), args)) {
    	P1OUT |= BIT6;
    }
    else if (streq(&(String) newstr("off", 3), args)) {
    	P1OUT &= ~BIT6;
    }
    else {
        putraw("type on or off\r\n", 16);
    }
    return 0;
}

static const Exec b_green = {
    .base = {
        .type = t_EXEC,
        .name = {"green", 5},
        .sibling = LAST_COMMAND
    },
    .content = green,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_green

static usize_t pwd(const String* args) {
	put(cwd->base.name);
	putraw("\r\n", 2);
	return 0;
}

static const Exec b_pwd = {
	.base = {
		.type = t_EXEC,
		.name = {"pwd", 3},
		.sibling = LAST_COMMAND
	},
	.content = pwd,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_pwd

static Dir* open_dir(const String* name, const Dir* fallback) {
	File* dir;
	if(name->size == 0) {
		dir = (File*) fallback;
	}
	else {
		dir = fopen(*name);
	}
	if (dir == NULL) {
		putraw("File not found\r\n", 16);
	}
	else if (dir->type != t_DIR) {
		putraw("File not dir\r\n", 14);
	}

	return (Dir*) dir;
}

static usize_t cd(const String* args) {
	Dir* dir = open_dir(args, &root);
    if(dir == NULL) {
    	return 1;
	}
	else {
		cwd = (Dir*) dir;
	}
	return 0;
}

static const Exec b_cd = {
	.base = {
		.type = t_EXEC,
		.name = {"cd", 2},
		.sibling = LAST_COMMAND
	},
	.content = cd,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_cd

static void list_files(const Dir* dir) {
	File* cur = dir->content;
    char type[3] = " \r\n";
    // String type = newstr(" \r\n", 3); // type.str[0] causes crash

    for(cur = dir->content; cur != NULL; cur = cur->sibling) {
        put(cur->name);
        switch(cur->type) {
            case t_DIR:  type[0] = '/'; break;
            case t_LINK: type[0] = '@'; break;
            case t_EXEC: type[0] = '*'; break;
            default:     type[0] = ' ';
        }
        putraw(type, 3);
    }
}

static usize_t ls(const String* args) {
	Dir* dir = open_dir(args, cwd);
    if(dir == NULL) {
    	return 1;
	}
	else {
		list_files(dir);
	}
	return 0;
}

static const Exec b_ls = {
	.base = {
		.type = t_EXEC,
		.name = {"ls", 2},
		.sibling = LAST_COMMAND
	},
	.content = ls,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_ls

static usize_t ln(const String* args) {
//    String* src_str = args;
//    if (len == 0) {
//        put("Source?\r\n", 9);
//        get(src_str);
//    }
//    File* src = fopen(src_str, len);
//    if (src == NULL) {
//        put("File not found", 16);
//        return;
//    }
//
//    put("Target?\r\n", 9);
//    String* dst_str = get(&len);
//    if (dst == NULL) {
//        put("", 16);
//        return;
//    }

	return 0;
}

static usize_t mkdir(const String* args) {
    char* name = malloc(args->size);
    Dir* dir = malloc(sizeof(Dir));
    if (name == NULL || dir == NULL) {
    	return 1; // out of memory
    }
    // setting dir name
    strncpy(name, args->str, args->size);
    dir->base.name.str = name;
    dir->base.name.size = args->size;

    // tying existing files behind new dir
    dir->base.sibling = cwd->content;
    dir->base.type = t_DIR;
    dir->content = NULL;

    // linking dir to current directory
    cwd->content = (File*) dir;

    return 0;
}

static const Exec b_mkdir = {
    .base = {
        .type = t_EXEC,
        .name = {"mkdir", 5},
        .sibling = LAST_COMMAND
    },
    .content = mkdir,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_mkdir

static usize_t memtest(const String* args) {
	char* stg = malloc(20);
	usize_t i = 0;
	for (i = 0; i < 19; i++) {
		stg[i] = '0';
	}
	stg[i] = '\0';
	putraw(stg, 19);
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

static usize_t sh(const String* args_) {
    String cmd, args;

	while (1) {
		putraw("> ", 2);

        get(&args);

        if (args.size == 0) {
			continue;
		}

        // tokenize args into cmd . ' ' . args
        // notice that `tokenize` puts `args` after the first delimiter
        // which is where args are supposed to begin
        tokenize(&args, ' ', &cmd);

        Exec* prog = (Exec*) fopenlocal(&cmd, &bin);

		if (prog == NULL) {
			putraw("Command not found\r\n", 19);
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
		.name = {"sh", 2},
		.sibling = LAST_COMMAND
	},
	.content = sh,
};

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_sh
