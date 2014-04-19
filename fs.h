/*
 * fs.h
 *
 *  Created on: Dec 20, 2013
 *      Author: xkx
 */

#ifndef FS_H_
#define FS_H_

#include "std.h"

// File types
enum file_t {
	t_DIR = 1,
	t_TEXT = 3,
	t_EXEC = 4,
	t_LINK = 5
};

typedef enum file_t File_t;

// File Structure definitions

struct File;

typedef struct File {
	File_t type;
	String name;
	struct File* sibling;
} File;

typedef struct Dir {
	File base;
	struct File* content;
} Dir;

typedef struct Text {
	File base;
	char* content;
} Text;

typedef struct Exec {
	File base;
    usize_t (*content) (const String* args);
} Exec;

typedef struct Link {
	File base;
	File* content;
} Link;

// FS-related functions

File* fopen(String name);
File* fopenlocal(const String* name, const Dir*  dir);

// Static variables
extern Dir root;

extern volatile Dir* cwd;

#endif /* FS_H_ */
