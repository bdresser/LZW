// code.c                                         Stan Eisenstat (09/23/09)
//
// Implementation of putBits/getBits described in code.h

#include <stdio.h>
#include <stdlib.h>
#include "/c/cs323/Hwk2/code.h"

// Information shared by putBits() and flushBits()
static int nExtra = 0;                  // #bits from previous byte(s)
static unsigned int extraBits = 0;      // Extra bits from previous byte(s)


// == PUTBITS MODULE =======================================================

// Write CODE (NBITS bits) to standard output
void putBits (int nBits, int code)
{
    unsigned int c;

    if (nBits > (sizeof(int)-1) * CHAR_BIT)
	exit (fprintf (stderr, "putBits: nBits = %d too large\n", nBits));

    code &= (1 << nBits) - 1;                   // Clear high-order bits
    nExtra += nBits;                            // Add new bits to extraBits
    extraBits = (extraBits << nBits) | code;
    while (nExtra >= CHAR_BIT) {                // Output any whole chars
	nExtra -= CHAR_BIT;                     //  and save remaining bits
	c = extraBits >> nExtra;
	putchar (c);
	extraBits ^= c << nExtra;
    }
}

// Flush remaining bits to standard output
void flushBits (void)
{
    if (nExtra != 0)
	putchar (extraBits << (CHAR_BIT - nExtra));
}


// == GETBITS MODULE =======================================================

// Return next code (#bits = NBITS) from input stream or EOF on end-of-file
int getBits (int nBits)
{
    int c;
    static int nExtra = 0;          // #bits from previous byte(s)
    static int unsigned extra = 0;  // Extra bits from previous byte(s)
									      
    if (nBits > (sizeof(extra)-1) * CHAR_BIT)
	exit (fprintf (stderr, "getBits: nBits = %d too large\n", nBits));

    // Read enough new bytes to have at least nBits bits to extract code
    while (nExtra < nBits) {
	if ((c = getchar()) == EOF)
	    return EOF;                         // Return EOF on end-of-file
	nExtra += CHAR_BIT;
	extra = (extra << CHAR_BIT) | c;
    }
    nExtra -= nBits;                            // Return nBits bits
    c = extra >> nExtra;
    extra ^= c << nExtra;                       // Save remainder
    return c;
}
