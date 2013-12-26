#include "msp430.h"
#include "std.h"

unsigned char flags = 0;

usize_t nexttoklen(const char* const str, char key, usize_t size) {
    usize_t i;
	for(i = 0; i < size && str[i] != '\0'; i++) {
		if(str[i] == key) {
            break;
		}
	}
    return i;
}

// s1 must be '\0' terminated, while s2 may not
// size = len(s2)
usize_t streq(const char* const s1, const String* const s2) {
	usize_t i;
    for(i = 0; i < s2->size; i++) {
        if(s1[i] != s2->str[i]) {
			return 0;
		}
	}
	if (s1[i] != '\0') {
		return 0;
	}
	return 1;
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
	if (UCA0RXBUF == '\r') {
		put("\r\n", 2);
		uart_buffer[uart_index] = '\0';
		setHasLine;
		if (needLine) {
			clearNeedLine;
			exitSleep();
		}
	}
	else if (UCA0RXBUF == '\b' || (UCA0RXBUF == '\x7F')) {
		if (uart_index > 0) {
		  uart_index--;
		  put("\b \b", 3);
		}
	}
	else if (uart_index < BUFFER_SIZE || !hasLine) {
		uart_buffer[uart_index++] = UCA0RXBUF;
		if (needChar) {
			clearNeedChar;
			exitSleep();
		}
		put(&UCA0RXBUF, 1);
	}
//	put("uart_index: [", 8);
//	printNum(uart_index);
//	put("]\r\n", 3);
//	put("input: [", 8);
//	printCharCode(UCA0RXBUF);
//	put("]\r\n", 3);
}

void put(const char *TxArray, usize_t ArrayLength){

	// Send number of bytes Specified in ArrayLength in the array at using the hardware UART 0
	// Example usage: UARTSendArray("Hello", 5);
	// int data[2]={1023, 235};
	// UARTSendArray(data, 4); // Note because the UART transmits bytes it is necessary to send two bytes for each integer hence the data length is twice the array length

	while(ArrayLength-- && *TxArray != '\0'){ // Loop until StringLength == 0 and post decrement
		while(!(IFG2 & UCA0TXIFG)); // Wait for TX buffer to be ready for new data
			UCA0TXBUF = *TxArray; //Write the character at the location specified by the pointer
			TxArray++; //Increment the TxString pointer to point to the next character
	}
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

char getchar() {
	if (uart_index == 0) {
		setNeedChar;
		enterSleep();
	}
	char result = uart_buffer[0];
	unshift();
	return result;
}

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


void printNum(int x) {
	static const char * const nums = "0123456789";
	while (x >= 10) {
		int y = x % 10;
		put(nums + y, 1);
		x /= 10;
	}
	put(nums + x, 1);
}

void printCharCode(char x) {
	static const char * const nums = "0123456789";
	while (x >= 10) {
		char y = x % 10;
		put(nums + y, 1);
		x /= 10;
	}
	put(nums + x, 1);
}
