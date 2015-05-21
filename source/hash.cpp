// Copyright (c) 1992-2003. Ken Kahn, Animated Programs, All rights reserved.

#include "defs.h" // for generating error messages

#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>

// uses MD5 to return a 128-bit hash

// based upon an example in http://msdn.microsoft.com/library/default.asp?url=/library/en-us/security/security/cryptcreatehash.asp

//--------------------------------------------------------------------
//  Declare variables.

HCRYPTPROV crypt_provider = NULL;
BOOL failed_to_initialize = FALSE;

BOOL initialize_hash() {
	// Get a handle to a cryptography provider context.
	return(CryptAcquireContext(&crypt_provider,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)); // only using it for hashing
};

void release_hash() {
	if (crypt_provider != NULL) {
		CryptReleaseContext(crypt_provider,0);
		crypt_provider = NULL;
	};
};

BOOL MD5_hash(BYTE *bytes, DWORD length, BYTE *hash_bits, DWORD *hash_length) {
	if (crypt_provider == NULL) {
		if (failed_to_initialize) {
			// warning new on 010903
			tt_error_file() << "Failed to acquire the MD5 provider so support for media files will be impaired. Please report this to support@toontalk.com" << endl;
			return(FALSE);
		};
		if (!initialize_hash()) {
			// warning new on 010903
			tt_error_file() << "Failed to initialize the MD5 hash code so support for media files will be impaired. Please report this to support@toontalk.com" << endl;
			return(FALSE);
		};
	};
	HCRYPTHASH hash_handle;
	// Acquire a hash object handle.
	if (!CryptCreateHash(crypt_provider,CALG_MD5,0,0,&hash_handle)) {
		// warning new on 010903
		tt_error_file() << "Failed to create the MD5 hash code so support for media files will be impaired. Please report this to support@toontalk.com" << endl;
		return(FALSE);
	};
// following based upon http://msdn.microsoft.com/library/default.asp?url=/library/en-us/security/security/crypthashdata.asp
	if (!CryptHashData(hash_handle,bytes,length,0)) {
		DWORD code = GetLastError();
		// warning new on 010903
		tt_error_file() << "Failed to perform the MD5 hash code so support for media files will be impaired. Please report this to support@toontalk.com. Error code = " << code << endl;
		return(FALSE);
	};
//--------------------------------------------------------------------
//  At this point, the content of pbBuffer has been added to the hash.
//  Additional data can be added by repeatedly calling CryptHashData.
//  When the hash object is no longer needed, it should be destroyed.
	if (!CryptGetHashParam(hash_handle,HP_HASHVAL,hash_bits,hash_length,0)) {
		DWORD code = GetLastError();
//#if TT_DEBUG_ON
//		debug_this();
//#endif
		// warning new on 010903
		tt_error_file() << "Failed to get the MD5 hash code so support for media files will be impaired. Please report this to support@toontalk.com. Error code = " << code << endl;
		tt_error_file() << "File length is " << length << " and hash_length is " << hash_length << endl;
		return(FALSE);
	};  
	CryptDestroyHash(hash_handle);
	return(TRUE);
};


