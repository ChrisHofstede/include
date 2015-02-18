#include <stdlib.h>
/**
 * Proposed FSS-UTF
 * ----------------
 *
 * The proposed UCS transformation format encodes UCS values in the range
 * [0,0x7fffffff] using multibyte characters of lengths 1, 2, 3, 4, 5,
 * and 6 bytes.  For all encodings of more than one byte, the initial
 * byte determines the number of bytes used and the high-order bit in
 * each byte is set.  Every byte that does not start 10xxxxxx is the
 * start of a UCS character sequence.

 * An easy way to remember this transformation format is to note that the
 * number of high-order 1's in the first byte signifies the number of
 * bytes in the multibyte character:
 *
 *    Bits  Hex Min  Hex Max  Byte Sequence in Binary
 * 1    7  00000000 0000007f 0vvvvvvv
 * 2   11  00000080 000007FF 110vvvvv 10vvvvvv
 * 3   16  00000800 0000FFFF 1110vvvv 10vvvvvv 10vvvvvv
 * 4   21  00010000 001FFFFF 11110vvv 10vvvvvv 10vvvvvv 10vvvvvv
 * 5   26  00200000 03FFFFFF 111110vv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
 * 6   31  04000000 7FFFFFFF 1111110v 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
 *
 * The UCS value is just the concatenation of the v bits in the multibyte
 * encoding.  When there are multiple ways to encode a value, for example
 * UCS 0, only the shortest encoding is legal.
 *
 * Below are sample implementations of the C standard wctomb() and
 * mbtowc() functions which demonstrate the algorithms for converting
 * from UCS to the transformation format and converting from the
 * transformation format to UCS. The sample implementations include error
 * checks, some of which may not be necessary for conformance:
 */

typedef struct {
	int	 cmask;
	int	 cval;
	int	 shift;
	long lmask;
	long lval;
} Tab;

static
Tab	tab[] =
{
	{0x80,	0x00,	0*6,	0x7F,		   0},		/* 1 byte sequence */
	{0xE0,	0xC0,	1*6,	0x7FF,		0x80},		/* 2 byte sequence */
	{0xF0,	0xE0,	2*6,	0xFFFF,		0x800},		/* 3 byte sequence */
	{0xF8,	0xF0,	3*6,	0x1FFFFF,	0x10000},	/* 4 byte sequence */
	{0xFC,	0xF8,	4*6,	0x3FFFFFF,	0x200000},	/* 5 byte sequence */
	{0xFE,	0xFC,	5*6,	0x7FFFFFFF,	0x4000000},	/* 6 byte sequence */
	{0},							                    /* end of table */
};

int _mbtowc(wchar_t *p, char *s, size_t n)
{
	long l;
	int c0, c, nc;
	Tab *t;

	if(s == 0)
		return 0;

	nc = 0;
	if((int)n <= nc)
		return -1;
	c0 = *s & 0xff;
	l = c0;
	for(t = tab; t->cmask; t++) {
		nc++;
		if((c0 & t->cmask) == t->cval) {
			l &= t->lmask;
			if(l < t->lval)
				return -1;
			*p = l;
			return nc;
		}
		if((int)n <= nc)
			return -1;
		s++;
		c = (*s ^ 0x80) & 0xFF;
		if(c & 0xC0)
			return -1;
		l = (l<<6) | c;
	}
	return -1;
}

int _wctomb(char *s, wchar_t wc)
{
	long l;
	int c, nc;
	Tab *t;

	if(s == 0)
		return 0;

	l = wc;
	nc = 0;
	for(t = tab; t->cmask; t++) {
		nc++;
		if(l <= t->lmask) {
			c = t->shift;
			*s = t->cval | (l>>c);
			while(c > 0) {
				c -= 6;
				s++;
				*s = 0x80 | ((l>>c) & 0x3F);
			}
			return nc;
		}
	}
	return -1;
}
