#ifndef PC
	#include "msp430.h"
#endif
#include "std.h"
#include "fs.h"
#include "string.h"
#include "malloc.h"

extern Dir bin;

#define LAST_COMMAND NULL

#define DEFINE_EXEC(func, len) \
	static Exec b_##func = { \
	.base = { \
		.type = t_EXEC, \
		.name = {#func, len}, \
		.sibling = LAST_COMMAND \
	}, \
	.content = func, \
};

static usize_t yes(String* args) {
	println(newstr("y", 1));
	return 0;
}

DEFINE_EXEC(yes, 3)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_yes

static usize_t uname(String* args) {
	println(newstr("Unix v.1 MSP430G2553", 20));
	return 0;
}

DEFINE_EXEC(uname, 5)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_uname

static usize_t echo(String* args) {
	printline(args);
	return 0;
}

DEFINE_EXEC(echo, 4)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_echo

static usize_t red(String* args) {
    if (streq(&newstr("on", 2), args)) {
    	P1OUT |= BIT0;
    }
    else if (streq(&newstr("off", 3), args)) {
    	P1OUT &= ~BIT0;
    }
    else {
        println(newstr("type on or off", 14));
    }
    return 0;
}

DEFINE_EXEC(red, 3)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_red

static usize_t green(String* args) {
    if (streq(&(String) newstr("on", 2), args)) {
    	P1OUT |= BIT6;
    }
    else if (streq(&(String) newstr("off", 3), args)) {
    	P1OUT &= ~BIT6;
    }
    else {
        println(newstr("type on or off", 14));
    }
    return 0;
}

DEFINE_EXEC(green, 5)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_green

static usize_t pwd(String* args) {
	println(cwd->base.name);
	return 0;
}

DEFINE_EXEC(pwd, 3)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_pwd

static usize_t cat(String* args) {
	Text* text = (Text*) fopen(*args);
    if(text == NULL) {
    	println(newstr("File not found", 14));
	}
	else if (text->base.type != t_TEXT) {
		println(newstr("Not txt file", 12));
	}
	else {
		printline(text->content);
		return 0;
	}
	return 1;
}

DEFINE_EXEC(cat, 3)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_cat

static Dir* open_dir(const String* name, const Dir* fallback) {
	File* dir;
	if(name->size == 0) {
		dir = (File*) fallback;
	}
	else {
		dir = fopen(*name);
	}
	if (dir == NULL) {
    	println(newstr("File not found", 14));
	}
	else if (dir->type != t_DIR) {
		println(newstr("Not dir", 7));
	}

	return (Dir*) dir;
}

static usize_t cd(String* args) {
	Dir* dir = open_dir(args, &root);
    if(dir == NULL) {
    	return 1;
	}
	else {
		cwd = (Dir*) dir;
	}
	return 0;
}

DEFINE_EXEC(cd, 2)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_cd

static void list_files(const Dir* dir) {
	File* cur;
    char type = ' ';
    String type_str = newstr(&type, 1);

    for(cur = dir->content; cur != NULL; cur = cur->sibling) {
        put(cur->name);
        switch(cur->type) {
            case t_DIR:  type = '/'; break;
            case t_LINK: type = '@'; break;
            case t_EXEC: type = '*'; break;
            default:     type = ' ';
        }
        println(type_str);
    }
}

static usize_t ls(String* args) {
	Dir* dir = open_dir(args, cwd);
    if(dir == NULL) {
    	return 1;
	}
	else {
		list_files(dir);
	}
	return 0;
}

DEFINE_EXEC(ls, 2)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_ls

static usize_t new_file(String* name, void* content, Dir* parent, File_t ftype) {
	char* dst_name = malloc(name->size);
    GenericFile* file = malloc(sizeof(GenericFile));

	if (dst_name == NULL || file == NULL) {
		return 1; // out of memory
	}

    // setting file name
    strncpy(dst_name, name->str, name->size);
    file->base.name.str = dst_name;
    file->base.name.size = name->size;
    // tying existing files behind new dir
    file->base.sibling = parent->content;
    file->base.type = ftype;
    file->content = content;

    // linking file to current directory
    parent->content = (File*) file;

    return 0;
}

static usize_t ln(String* args) {
    String not_found = newstr("File not found", 16);
    // get src file
    String path = newstr(args->str, args->size);
    if (path.size == 0) {
        putraw("Source? ", 8);
        get(&path);
    }
    File* src = fopen(path);
    if (src == NULL) {
        println(not_found);
        return 1;
    }
    // get destination path & name
    putraw("Target? ", 8);
    String link_name;
    get(&path); // reusing previously consumed `path`
	Dir* parent;
	parsePath(&path, &link_name);
    parent = open_dir(&path, cwd);
    if (parent == NULL) {
    	println(not_found);
    	return 1;
    }

    return new_file(&link_name, src, parent, t_LINK);
}

DEFINE_EXEC(ln, 2)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_ln

static usize_t mkdir(String* args) {
    String path = {args->str, args->size};
    if (path.str[path.size - 1] == '/') {
    	path.size -= 1; // valid but not useful
    }
    String dir_name;
    Dir* parent;
    parsePath(&path, &dir_name);
    parent = open_dir(&path, cwd);
    if (parent == NULL) {
    	return 1;
    }

    return new_file(&dir_name, NULL, parent, t_DIR);
}

DEFINE_EXEC(mkdir, 5)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_mkdir

static usize_t memtest(String* args) {
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

DEFINE_EXEC(memtest, 7)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_memtest

static char parseNum(String* const input, int* output) {
	usize_t i;
	char digit;
	int result = 0;

	if (input->size == 0) {
		return 1;
	}
	if (input->str[0] == '-') {
		i = 1;
	}
	else {
		i = 0;
	}
	for (; i < input->size; i++) {
		digit = input->str[i];
		if (digit < '0' || digit > '9') {
			// NaN
			return 1;
		}
		result = result * 10 + (digit - 48);
	}
	// compiler will optimize this fine
	if (input->str[0] == '-') {
		result = -result;
	}
	(*output) = result;
	return 0;
}

static usize_t dc(String* args) {
	int stack[10];
	signed char index = -1;
	String input;
	String badSyntax = newstr("Bad syntax", 10);
	String lackData = newstr("Stack empty", 11);
	String notANumber = newstr("Not a number", 12);
	while (1) {
		get(&input);
		if ((input.str[0] >= '0' && input.str[0] <= '9')
		  || input.str[0] == '-') {
			if(parseNum(&input, stack + index + 1)) {
				println(notANumber);
			}
			else {
				index++;
			}
		}
		else if (input.size == 1) {
			switch(input.str[0]) {
				case '+': if (index >= 1) {
						stack[index - 1] = stack[index - 1] + stack[index];
						index--;
					}
					else {
						println(lackData);
					}
					break;
				case '-': if (index >= 1) {
						stack[index - 1] = stack[index - 1] - stack[index];
						index--;
					}
					else {
						println(lackData);
					}
					break;
				case '*': if (index >= 1) {
						stack[index - 1] = stack[index - 1] * stack[index];
						index--;
					}
					else {
						println(lackData);
					}
					break;
				case '/': if (index >= 1) {
						stack[index - 1] = stack[index - 1] / stack[index];
						index--;
					}
					else {
						println(lackData);
					}
					break;
				case '%': if (index >= 1) {
						stack[index - 1] = stack[index - 1] % stack[index];
						index--;
					}
					else {
						println(lackData);
					}
					break;
				case 'p': if (index >= 0) {
						printNum(stack[index]);
					}
					break;
				case 'c': index = -1;
					break;
				case 'q': return 0;
				default:
					println(badSyntax);
			}
		}
		else {
			println(badSyntax);
		}
	}
}

DEFINE_EXEC(dc, 2)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_dc

static usize_t sh(String* args_) {
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
			println(newstr("Unknown Command", 15));
		}
		else {
            prog->content(&args);
			continue;
		}
	}
}

DEFINE_EXEC(sh, 2)

#undef LAST_COMMAND
#define LAST_COMMAND (File*) &b_sh
