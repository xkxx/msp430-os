/*
 * std.h
 *
 *  Created on: Dec 21, 2013
 *      Author: xkx
 */

#ifndef STD_H_
#define STD_H_

#define NULL 0

// Buffer size for Standard IO

#define BUFFER_SIZE 20

// IO/Timer flags

extern unsigned char flags;

#define isAsleep (flags & (1 << 0))
#define needChar (flags & (1 << 1))
#define needLine (flags & (1 << 2))
#define hasLine (flags & (1 << 3))

#define setAsleep (flags |= (1 << 0))
#define setNeedChar (flags |= (1 << 1))
#define setNeedLine (flags |= (1 << 2))
#define setHasLine (flags |= (1 << 3))

#define clearAsleep (flags &= ~(1 << 0))
#define clearNeedChar (flags &= ~(1 << 1))
#define clearNeedLine (flags &= ~(1 << 2))
#define clearHasLine (flags &= ~(1 << 3))

#define enterSleep() setAsleep; __bis_SR_register(LPM3_bits + GIE) // Enter LPM0, interrupts enabled
#define exitSleep() clearAsleep; LPM3_EXIT

// Type for calculating sizes
typedef unsigned short usize_t;

/**
 * A overflow-safe mutable string type
 *
 * @var str   a char array that contains the content of the string
 *            does not have to be '\0' terminated
 *            is indeterminable if .size == 0 (can be anything)
 * @var size  the size of the string, excluding possible '\0'
 *            .size must be 0 in case the string is empty
 */

typedef struct String {
    char* str;
    usize_t size;
} String;

/// create a new string
#define newstr(str, size) (String){str, size}

// String functions

void tokenize(String* str, char delimiter, String* result);
void tokenize_right(String* str, char delimiter, String* result);
usize_t streq(const String* s1, const String* s2);

// Standard IO

// UART buffer
static volatile char uart_buffer[BUFFER_SIZE];
// UART buffer cursor
static volatile usize_t uart_index = 0;

// Output functions

// For internal use only
void putraw(char const *str, usize_t len);

#define put(string) putraw((string).str, (string).size)

void printline(String* const str);

#define println(string) printline(&(string));

// Input functions
void get(String* in);

char getchar();

// Timer functions
void sleep(unsigned int cycles);

// Debug
void printNum(int x);
void printCharCode(char x);

#endif /* STD_H_ */
