#pragma once

#include "types.h"

namespace core
{

struct PRNG
{
	u64 keys[4];							// 4 keys

	static inline u64 rol( u64 r, u8 amt )
	{
		assert( amt != 0 && amt < 64 );
		return (r << amt) | (r >> (64-amt) );
	}
	static inline u64 ror( u64 r, u8 amt )
	{
		assert( amt != 0 && amt < 64 );
		return (r >> amt) | (r << (64-amt) );
	}

	// passes diehard tests!
	// can't test dieharder under windows
	// one round of PRNG
	inline u64 next64()
	{
		u64 tmp = keys[0] + keys[2] - keys[1] - keys[3];
		keys[0] ^= rol( keys[1], 5 );
		keys[1] += ror( keys[2], 27 );
		keys[2] -= rol( keys[3], 41 );
		return keys[3] ^= ror( tmp, 17 ) - 0xfacebabec0ca101aULL;
	}

	PRNG( u64 seed = 0 )
	{
		keys[0] = keys[1] = keys[2] = keys[3] = seed;
		// scramble
		for (int i=0; i<64; i++)
			next64();
	}
};

}
