/*
 * std.h
 *
 *  Created on: Dec 21, 2013
 *      Author: xkx
 */

#ifndef STD_H_
#define STD_H_

#define NULL 0

#define BUFFER_SIZE 20

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

typedef unsigned short usize_t;

typedef struct String {
    char* str;
    usize_t size;
} String;

#define newString(str, size) {str, size}
#define putstr(string) put((string).str, (string).size)

static volatile char uart_buffer[BUFFER_SIZE];
static volatile usize_t uart_index = 0;

usize_t nexttoklen(const char* const str, char key, usize_t size);
usize_t streq(const char* const s1, const String* const s2);

void put(const char *TxArray, usize_t ArrayLength);
void get(String* in);
char getchar_();


void printNum(int x);
void printCharCode(char x);

#endif /* STD_H_ */
