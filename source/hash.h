// Copyright (c) 1992-2003. Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_HASH_H)

#define __TT_HASH_H

void release_hash();

BOOL MD5_hash(BYTE *bytes, DWORD length, BYTE *hash_bits, DWORD *hash_length);

#endif



