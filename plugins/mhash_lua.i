
%enum hashid
	MHASH_CRC32
	MHASH_MD5
	MHASH_SHA1
	MHASH_HAVAL256
	MHASH_RIPEMD160
	MHASH_TIGER192
	MHASH_GOST
	MHASH_CRC32B
	MHASH_HAVAL224
	MHASH_HAVAL192
	MHASH_HAVAL160
	MHASH_HAVAL128
	MHASH_TIGER128
	MHASH_TIGER160
	MHASH_MD4
	MHASH_SHA256
	MHASH_ADLER32
	MHASH_SHA224
	MHASH_SHA512
	MHASH_SHA384
	MHASH_WHIRLPOOL
	MHASH_RIPEMD128
	MHASH_RIPEMD256
	MHASH_RIPEMD320
	MHASH_SNEFRU128
	MHASH_SNEFRU256
	MHASH_MD2
	MHASH_AR
	MHASH_BOOGNISH
	MHASH_CELLHASH
	MHASH_FFT_HASH_I
	MHASH_FFT_HASH_II
	MHASH_NHASH
	MHASH_PANAMA
	MHASH_SMASH
	MHASH_SUBHASH
%endenum


%enum mutils_error_codes
	MUTILS_OK
	MUTILS_SYSTEM_ERROR
	MUTILS_UNSPECIFIED_ERROR
	MUTILS_SYSTEM_RESOURCE_ERROR
	MUTILS_PARAMETER_ERROR
	MUTILS_INVALID_FUNCTION
	MUTILS_INVALID_INPUT_BUFFER
	MUTILS_INVALID_OUTPUT_BUFFER
	MUTILS_INVALID_PASSES
	MUTILS_INVALID_FORMAT
	MUTILS_INVALID_SIZE
	MUTILS_INVALID_RESULT
%endenum

%enum keygenid
	KEYGEN_MCRYPT
	KEYGEN_ASIS
	KEYGEN_HEX
	KEYGEN_PKDES
	KEYGEN_S2K_SIMPLE
	KEYGEN_S2K_SALTED
	KEYGEN_S2K_ISALTED
%endenum

%class %noclassinfo mhash_state
	
%endclass




// information prototypes

%function double mhash_count()
%function double mhash_get_block_size(hashid type)
%function wxString mhash_get_hash_name(hashid type)
%function void mhash_free(void *ptr)

%function wxString mhash_get_hash_name_static(hashid type)

// initializing prototypes

%function MHASH mhash_init(hashid type)

// copy prototypes

%function MHASH mhash_cp(MHASH from)

%function bool mhash(MHASH thread, const void *plaintext, double size)

// finalizing prototype

%function void *mhash_end(MHASH thread)
// %function void *mhash_end_m(MHASH thread, void *(*hash_malloc) (mutils_word32));
%function void mhash_deinit(MHASH thread, void *result)

/* informational */

%function double mhash_get_hash_pblock(hashid type)
%function hashid mhash_get_mhash_algo(MHASH tmp)

/* HMAC */

//%function MHASH mhash_hmac_init(const hashid type, void *key, mutils_word32 keysize, mutils_word32 block);
//%function void *mhash_hmac_end_m(MHASH thread, void *(*hash_malloc) (mutils_word32));
//%function void *mhash_hmac_end(MHASH thread);
//%function mutils_boolean mhash_hmac_deinit(MHASH thread, void *result);

/* Save state functions */

//%function mutils_boolean mhash_save_state_mem(MHASH thread, void *mem, mutils_word32 *mem_size);
//%function MHASH mhash_restore_state_mem(void *mem);

/* Key generation functions */

//%function mutils_error mhash_keygen(keygenid algorithm, hashid opt_algorithm,
//			  mutils_word64 count,
//			  void *keyword, mutils_word32 keysize,
//			  void *salt, mutils_word32 saltsize,
//			  mutils_word8 *password, mutils_word32 passwordlen);

//%function mutils_error mhash_keygen_ext(keygenid algorithm, KEYGEN data,
//			      void *keyword, mutils_word32 keysize,
//			      mutils_word8 *password, mutils_word32 passwordlen);

//%function mutils_word8 *mhash_get_keygen_name(keygenid type);

//%function mutils_word32 mhash_get_keygen_salt_size(keygenid type);
//%function mutils_word32 mhash_get_keygen_max_key_size(keygenid type);
//%function mutils_word32 mhash_keygen_count(void);

//%function mutils_boolean mhash_keygen_uses_salt(keygenid type);
//%function mutils_boolean mhash_keygen_uses_count(keygenid type);
//%function mutils_boolean mhash_keygen_uses_hash_algorithm(keygenid type);
