/*
 * cli.h
 *
 *  Created on: Dec 20, 2013
 *      Author: xkx
 */

#ifndef CLI_H_
#define CLI_H_

#include "msp430.h"
#include "string.h"
#include "std.h"

struct CliCommand;

typedef struct CliCommand {
	const char* key;
	int (* const prog)(const usize_t argc, const char* const args);
	const struct CliCommand * const next;
} CliCommand;

#endif /* CLI_H_ */
