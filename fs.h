/*
 * fs.h
 *
 *  Created on: Dec 20, 2013
 *      Author: xkx
 */

#ifndef FS_H_
#define FS_H_

#include "std.h"

enum file_t {
	t_DIR = 1,
	t_TEXT = 3,
	t_EXEC = 4,
	t_LINK = 5
};

typedef enum file_t file_t;

struct File;

typedef struct File {
	file_t type;
	char* name;
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
    usize_t (*content) (const String* const args);
} Exec;

typedef struct Link {
	File base;
	File* content;
} Link;

File* fopen(const String* const name);
File* fopenLoc(const String* const name, const Dir* const dir);

extern Dir root;

extern volatile Dir* cwd;

#endif /* FS_H_ */
