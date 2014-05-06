#ifndef PC
	#include "msp430.h"
#else
	#include <stdio.h>
#endif
#include "std.h"

unsigned char flags = 0;

// String functions

/**
 * Split a string into tokens
 * @param str        string to process (.size may be mutated)
 *
 * @param delimiter  token delimiter
 * @param result     tokenizer output
 *                   == `str` if no `delimiter` is found
 *
 * This function is unpure. `str.size` will be changed.
 * Use a copy of the original string if such is a problem
 */

void tokenize(String* str, char delimiter, String* result) {
    usize_t i;

    // `result` always begins at start of `str`
    result->str = str->str;

    for (i = 0; i < str->size; i++) {
        if(str->str[i] == delimiter) {
            // set `result` to end before the delimiter
            result->size = i;
            // set input `str` to begin after delimiter
            // we don't care if it's out of bound since
            // .size == 0 in that case
            str->str += i + 1;
            str->size -= i + 1;
            return;
        }
    }

    // default case
    // if no delimiter is found, `result` == `str` and `str` == ""
    result->size = str->size;
    str->size = 0;
}

/**
  check if two strings are equal
  @param s1  a string to compare
  @param s2  a string to compare against
  @return 1  if s1 == s2
          0  otherwise
 */

usize_t streq(const String* s1, const String* s2) {
	usize_t i;
	if (s1->size != s2->size) {
        return 0;
	}
    for(i = 0; i < s2->size && i < s1->size; i++) {
        if(s1->str[i] != s2->str[i]) {
			return 0;
		}
	}
	return 1;
}

void tokenize_right(String* str, char delimiter, String* result) {
    int i; // unsigned int rotates

    for (i = str->size - 1; i >= 0; i--) {
        if(str->str[i] == delimiter) {
            // set `result` to begin after the delimiter
            // we don't care if it's out of bound since
            // .size == 0 in that case
            result->size = str->size - i - 1;
            result->str = str->str + i + 1;
            // set input `str` to end before delimiter
            str->size = i;
            return;
        }
    }

    // default case
    // if no delimiter is found, `result` == `str` and `str` == ""
    result->str = str->str;
    result->size = str->size;
    str->size = 0;
}

// Standard IO

#ifndef PC

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
	if (UCA0RXBUF == '\r') {
		putraw("\r\n", 2);
		setHasLine;
		if (needLine) {
			clearNeedLine;
			exitSleep();
		}
	}
	else if (UCA0RXBUF == '\b' || UCA0RXBUF == '\x7F') {
		if (uart_index > 0) {
		  uart_index--;
		  putraw("\b \b", 3);
		}
	}
	else if (uart_index < BUFFER_SIZE || !hasLine) {
		uart_buffer[uart_index++] = UCA0RXBUF;
		if (needChar) {
			clearNeedChar;
			exitSleep();
		}
		putraw(&UCA0RXBUF, 1);
	}
//	put("uart_index: [", 8);
//	printNum(uart_index);
//	put("]\r\n", 3);
//	put("input: [", 8);
//	printCharCode(UCA0RXBUF);
//	put("]\r\n", 3);
}

#endif

void putraw(char const *str, usize_t len) {

	while(len--) { // Loop until StringLength == 0 and post decrement
		while(!(IFG2 & UCA0TXIFG)); // Wait for TX buffer to be ready for new data
			UCA0TXBUF = *str; //Write the character at the location specified by the pointer
			str++; //Increment the TxString pointer to point to the next character
	}
}

void printline(String* const str) {
	if (str != NULL) {
		putraw(str->str, str->size);
	}
	putraw("\r\n", 2);
}

void get(String* in) {
	if (!hasLine) {
		setNeedLine;
		enterSleep();
	}
	clearHasLine;
	in->size = uart_index;
    in->str = uart_buffer;
	uart_index = 0;
}

static void unshift() {
	usize_t i;
	for(i = 0; i < uart_index - 1; i++) {
		uart_buffer[i] = uart_buffer[i+1];
	}
	uart_index--;
}

/**
 * Get the next character from input
 *
 * @return the next input character
 *
 * If there's nothing in buffer, blocks until input is received
 */
char getchar() {
	if (uart_index == 0) {
		setNeedChar;
		enterSleep();
	}
	char result = uart_buffer[0];
	unshift();
	return result;
}

// Timers

void sleep(unsigned int cycles) {
	TACCR0 = cycles;
	TACCTL0 |= CCIE;
	TACTL = TASSEL_1 | MC_1;
	enterSleep();
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {
	exitSleep();
}

// Debug

void printNum(int x) {
	static const char zero = '0';
	char temp[6]; // int goes to 65536
	signed char index = 0;
	if (x < 0) {
		temp[index] = '-';
		index++;
		x = -x;
	}
	while (x >= 10) {
		temp[index] = zero + x % 10;
		x /= 10;
		index++;
	}
	temp[index] = zero + x;
	for (; index >= 0; index--) {
		putraw(temp + index, 1);
	}
	printline(NULL);
}
