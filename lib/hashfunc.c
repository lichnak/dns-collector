/*
 *	Hyper-super-meta-alt-control-shift extra fast str_len() and hash_*()
 *	routines
 *
 *	It is always at least as fast as the classical strlen() routine and for
 *	strings longer than 100 characters, it is substantially faster.
 *
 *	(c) 2002, Robert Spalek <robert@ucw.cz>
 */

#include "lib/lib.h"
#include "lib/hashfunc.h"
#include "lib/chartype.h"

/* The number of bits the hash in the function hash_*() is rotated by after
 * every pass.  It should be prime with the word size.  */
#define	SHIFT_BITS	5

/* A bit-mask which clears higher bytes than a given threshold.  */
static uns mask_higher_bits[sizeof(uns)];

static void CONSTRUCTOR
hashfunc_init(void)
{
	uns i, j;
	byte *str;
	for (i=0; i<sizeof(uns); i++)
	{
		str = (byte *) (mask_higher_bits + i);
		for (j=0; j<i; j++)
			str[j] = -1;
		for (j=i; j<sizeof(uns); j++)
			str[j] = 0;
	}
}

static inline uns CONST
str_len_uns(uns x)
{
	const uns sub = ((uns) -1) / 0xff;
	const uns and = sub * 0x80;
	uns a, i;
	byte *bytes;
	a = (x ^ (x - sub)) & and;
	/* 
	 * x_2 = x - 0x01010101;
	 * x_3 = x ^ x_2;
	 * a = x_3 & 0x80808080;
	 *
	 * If no byte of x is in {0, 0x80}, then the highest bit of each byte
	 * of x_2 is the same as of x.  Hence x_3 has all these highest bits
	 * cleared.  If a == 0, then we are sure there is no zero byte in x.
	 */
	if (!a)
		return sizeof(uns);
	bytes = (byte *) &x;
	for (i=0; i<sizeof(uns) && bytes[i]; i++);
	return i;
}

inline uns
str_len_aligned(const byte *str)
{
	const uns *u = (const uns *) str;
	uns len = 0;
	while (1)
	{
		uns l = str_len_uns(*u++);
		len += l;
		if (l < sizeof(uns))
			return len;
	}
}

inline uns
hash_string_aligned(const byte *str)
{
	const uns *u = (const uns *) str;
	uns hash = 0;
	while (1)
	{
		uns last_len = str_len_uns(*u);
		hash = ROL(hash, SHIFT_BITS);
		if (last_len < sizeof(uns))
		{
			uns tmp = *u & mask_higher_bits[last_len];
			hash ^= tmp;
			return hash;
		}
		hash ^= *u++;
	}
}

inline uns
hash_block_aligned(const byte *str, uns len)
{
	const uns *u = (const uns *) str;
	uns hash = 0;
	while (len >= sizeof(uns))
	{
		hash = ROL(hash, SHIFT_BITS) ^ *u++;
		len -= sizeof(uns);
	}
	hash = ROL(hash, SHIFT_BITS) ^ (*u & mask_higher_bits[len]);
	return hash;
}

#ifndef	CPU_ALLOW_UNALIGNED
uns
str_len(const byte *str)
{
	uns shift = UNALIGNED_PART(str, uns);
	if (!shift)
		return str_len_aligned(str);
	else
	{
		uns i;
		shift = sizeof(uns) - shift;
		for (i=0; i<shift; i++)
			if (!str[i])
				return i;
		return shift + str_len_aligned(str + shift);
	}
}

uns
hash_string(const byte *str)
{
	uns shift = UNALIGNED_PART(str, uns);
	if (!shift)
		return hash_string_aligned(str);
	else
	{
		uns hash = 0;
		uns i;
		for (i=0; ; i++)
		{
			uns modulo = i % sizeof(uns);
			uns shift;
#ifdef	CPU_LITTLE_ENDIAN
			shift = modulo;
#else
			shift = sizeof(uns) - 1 - modulo;
#endif
			if (!modulo)
				hash = ROL(hash, SHIFT_BITS);
			if (!str[i])
				break;
			hash ^= str[i] << (shift * 8);
		}
		return hash;
	}
}

uns
hash_block(const byte *str, uns len)
{
	uns shift = UNALIGNED_PART(str, uns);
	if (!shift)
		return hash_block_aligned(str, len);
	else
	{
		uns hash = 0;
		uns i;
		for (i=0; ; i++)
		{
			uns modulo = i % sizeof(uns);
			uns shift;
#ifdef	CPU_LITTLE_ENDIAN
			shift = modulo;
#else
			shift = sizeof(uns) - 1 - modulo;
#endif
			if (!modulo)
				hash = ROL(hash, SHIFT_BITS);
			if (i >= len)
				break;
			hash ^= str[i] << (shift * 8);
		}
		return hash;
	}
}
#endif

uns
hash_string_nocase(const byte *str)
{
	uns hash = 0;
	uns i;
	for (i=0; ; i++)
	{
		uns modulo = i % sizeof(uns);
		uns shift;
#ifdef	CPU_LITTLE_ENDIAN
		shift = modulo;
#else
		shift = sizeof(uns) - 1 - modulo;
#endif
		if (!modulo)
			hash = ROL(hash, SHIFT_BITS);
		if (!str[i])
			break;
		hash ^= Cupcase(str[i]) << (shift * 8);
	}
	return hash;
}
