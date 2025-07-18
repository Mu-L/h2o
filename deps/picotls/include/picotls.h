/*
 * Copyright (c) 2016 DeNA Co., Ltd., Kazuho Oku
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef picotls_h
#define picotls_h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WINDOWS
#include "wincompat.h"
#endif

#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>
#ifndef _WINDOWS
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#if __GNUC__ >= 3
#define PTLS_LIKELY(x) __builtin_expect(!!(x), 1)
#define PTLS_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define PTLS_BUILD_ASSERT_EXPR(cond) (sizeof(char[2 * !!(!__builtin_constant_p(cond) || (cond)) - 1]) != 0)
#define PTLS_BUILD_ASSERT(cond) ((void)PTLS_BUILD_ASSERT_EXPR(cond))
#else
#define PTLS_LIKELY(x) (x)
#define PTLS_UNLIKELY(x) (x)
#define PTLS_BUILD_ASSERT(cond) 1
#endif

/* __builtin_types_compatible_p yields incorrect results when older versions of GCC is used; see #303.
 * Clang with Xcode 9.4 or prior is known to not work correctly when a pointer is const-qualified; see
 * https://github.com/h2o/quicly/pull/306#issuecomment-626037269. Older versions of clang upstream works fine, but we do not need
 * best coverage. This macro is for preventing misuse going into the master branch, having it work one of the compilers supported in
 * our CI is enough.
 */
#if ((defined(__clang__) && __clang_major__ >= 10) || __GNUC__ >= 6) && !defined(__cplusplus)
#define PTLS_ASSERT_IS_ARRAY_EXPR(a) PTLS_BUILD_ASSERT_EXPR(__builtin_types_compatible_p(__typeof__(a[0])[], __typeof__(a)))
#else
#define PTLS_ASSERT_IS_ARRAY_EXPR(a) 1
#endif

#define PTLS_ELEMENTSOF(x) (PTLS_ASSERT_IS_ARRAY_EXPR(x) * sizeof(x) / sizeof((x)[0]))

#ifdef _WINDOWS
#define PTLS_THREADLOCAL __declspec(thread)
#else
#define PTLS_THREADLOCAL __thread
#endif

#ifndef PTLS_HAVE_LOG
#ifdef _WINDOWS
#define PTLS_HAVE_LOG 0
#else
#define PTLS_HAVE_LOG 1
#endif
#endif

#ifndef PTLS_FUZZ_HANDSHAKE
#define PTLS_FUZZ_HANDSHAKE 0
#endif

#define PTLS_HELLO_RANDOM_SIZE 32

#define PTLS_AES128_KEY_SIZE 16
#define PTLS_AES256_KEY_SIZE 32
#define PTLS_AES_BLOCK_SIZE 16
#define PTLS_AES_IV_SIZE 16
#define PTLS_AESGCM_IV_SIZE 12
#define PTLS_AESGCM_TAG_SIZE 16
#define PTLS_AESGCM_CONFIDENTIALITY_LIMIT 0x2000000            /* 2^25 */
#define PTLS_AESGCM_INTEGRITY_LIMIT UINT64_C(0x40000000000000) /* 2^54 */
#define PTLS_AESCCM_CONFIDENTIALITY_LIMIT 0xB504F3             /* 2^23.5 */
#define PTLS_AESCCM_INTEGRITY_LIMIT 0xB504F3                   /* 2^23.5 */

#define PTLS_CHACHA20_KEY_SIZE 32
#define PTLS_CHACHA20_IV_SIZE 16 /* contrary to RFC 7539, follow OpenSSL way of using first 32 bits as ctr and latter 96 as IV */
#define PTLS_CHACHA20POLY1305_IV_SIZE 12
#define PTLS_CHACHA20POLY1305_TAG_SIZE 16
#define PTLS_CHACHA20POLY1305_CONFIDENTIALITY_LIMIT UINT64_MAX       /* at least 2^64 */
#define PTLS_CHACHA20POLY1305_INTEGRITY_LIMIT UINT64_C(0x1000000000) /* 2^36 */

#define PTLS_AEGIS128L_KEY_SIZE 16
#define PTLS_AEGIS128L_IV_SIZE 16
#define PTLS_AEGIS128L_TAG_SIZE 16
#define PTLS_AEGIS128L_CONFIDENTIALITY_LIMIT UINT64_MAX          /* at least 2^64 */
#define PTLS_AEGIS128L_INTEGRITY_LIMIT UINT64_C(0x1000000000000) /* 2^48 */

#define PTLS_AEGIS256_KEY_SIZE 32
#define PTLS_AEGIS256_IV_SIZE 32
#define PTLS_AEGIS256_TAG_SIZE 16
#define PTLS_AEGIS256_CONFIDENTIALITY_LIMIT UINT64_MAX          /* at least 2^64 */
#define PTLS_AEGIS256_INTEGRITY_LIMIT UINT64_C(0x1000000000000) /* 2^48 */

#define PTLS_BLOWFISH_KEY_SIZE 16
#define PTLS_BLOWFISH_BLOCK_SIZE 8

#define PTLS_SHA256_BLOCK_SIZE 64
#define PTLS_SHA256_DIGEST_SIZE 32

#define PTLS_SHA384_BLOCK_SIZE 128
#define PTLS_SHA384_DIGEST_SIZE 48

#define PTLS_SHA512_BLOCK_SIZE 128
#define PTLS_SHA512_DIGEST_SIZE 64

#define PTLS_MAX_SECRET_SIZE 32
#define PTLS_MAX_IV_SIZE 32
#define PTLS_MAX_DIGEST_SIZE 64

/* versions */
#define PTLS_PROTOCOL_VERSION_TLS12 0x0303
#define PTLS_PROTOCOL_VERSION_TLS13 0x0304

/* cipher-suites */
#define PTLS_CIPHER_SUITE_AES_128_GCM_SHA256 0x1301
#define PTLS_CIPHER_SUITE_NAME_AES_128_GCM_SHA256 "TLS_AES_128_GCM_SHA256"
#define PTLS_CIPHER_SUITE_AES_256_GCM_SHA384 0x1302
#define PTLS_CIPHER_SUITE_NAME_AES_256_GCM_SHA384 "TLS_AES_256_GCM_SHA384"
#define PTLS_CIPHER_SUITE_CHACHA20_POLY1305_SHA256 0x1303
#define PTLS_CIPHER_SUITE_NAME_CHACHA20_POLY1305_SHA256 "TLS_CHACHA20_POLY1305_SHA256"
#define PTLS_CIPHER_SUITE_AEGIS256_SHA512 0x1306
#define PTLS_CIPHER_SUITE_NAME_AEGIS256_SHA512 "TLS_AEGIS_256_SHA512"
#define PTLS_CIPHER_SUITE_AEGIS128L_SHA256 0x1307
#define PTLS_CIPHER_SUITE_NAME_AEGIS128L_SHA256 "TLS_AEGIS_128L_SHA256"

/* TLS/1.2 cipher-suites that we support (for compatibility, OpenSSL names are used) */
#define PTLS_CIPHER_SUITE_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 0xc02b
#define PTLS_CIPHER_SUITE_NAME_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 "ECDHE-ECDSA-AES128-GCM-SHA256"
#define PTLS_CIPHER_SUITE_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 0xc02c
#define PTLS_CIPHER_SUITE_NAME_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 "ECDHE-ECDSA-AES256-GCM-SHA384"
#define PTLS_CIPHER_SUITE_ECDHE_RSA_WITH_AES_128_GCM_SHA256 0xc02f
#define PTLS_CIPHER_SUITE_NAME_ECDHE_RSA_WITH_AES_128_GCM_SHA256 "ECDHE-RSA-AES128-GCM-SHA256"
#define PTLS_CIPHER_SUITE_ECDHE_RSA_WITH_AES_256_GCM_SHA384 0xc030
#define PTLS_CIPHER_SUITE_NAME_ECDHE_RSA_WITH_AES_256_GCM_SHA384 "ECDHE-RSA-AES256-GCM-SHA384"
#define PTLS_CIPHER_SUITE_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 0xcca8
#define PTLS_CIPHER_SUITE_NAME_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 "ECDHE-RSA-CHACHA20-POLY1305"
#define PTLS_CIPHER_SUITE_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 0xcca9
#define PTLS_CIPHER_SUITE_NAME_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 "ECDHE-ECDSA-CHACHA20-POLY1305"

/* negotiated_groups */
#define PTLS_GROUP_SECP256R1 23
#define PTLS_GROUP_NAME_SECP256R1 "secp256r1"
#define PTLS_GROUP_SECP384R1 24
#define PTLS_GROUP_NAME_SECP384R1 "secp384r1"
#define PTLS_GROUP_SECP521R1 25
#define PTLS_GROUP_NAME_SECP521R1 "secp521r1"
#define PTLS_GROUP_X25519 29
#define PTLS_GROUP_NAME_X25519 "x25519"
#define PTLS_GROUP_X448 30
#define PTLS_GROUP_NAME_X448 "x448"
#define PTLS_GROUP_SECP256R1MLKEM768 4587
#define PTLS_GROUP_NAME_SECP256R1MLKEM768 "SecP256r1MLKEM768"
#define PTLS_GROUP_X25519MLKEM768 4588
#define PTLS_GROUP_NAME_X25519MLKEM768 "X25519MLKEM768"
#define PTLS_GROUP_SECP384R1MLKEM1024 4589
#define PTLS_GROUP_NAME_SECP384R1MLKEM1024 "SecP384r1MLKEM1024"
#define PTLS_GROUP_MLKEM512 512
#define PTLS_GROUP_NAME_MLKEM512 "MLKEM512"
#define PTLS_GROUP_MLKEM768 513
#define PTLS_GROUP_NAME_MLKEM768 "MLKEM768"
#define PTLS_GROUP_MLKEM1024 514
#define PTLS_GROUP_NAME_MLKEM1024 "MLKEM1024"

/* signature algorithms */
#define PTLS_SIGNATURE_RSA_PKCS1_SHA1 0x0201
#define PTLS_SIGNATURE_RSA_PKCS1_SHA256 0x0401
#define PTLS_SIGNATURE_ECDSA_SECP256R1_SHA256 0x0403
#define PTLS_SIGNATURE_ECDSA_SECP384R1_SHA384 0x0503
#define PTLS_SIGNATURE_ECDSA_SECP521R1_SHA512 0x0603
#define PTLS_SIGNATURE_RSA_PSS_RSAE_SHA256 0x0804
#define PTLS_SIGNATURE_RSA_PSS_RSAE_SHA384 0x0805
#define PTLS_SIGNATURE_RSA_PSS_RSAE_SHA512 0x0806
#define PTLS_SIGNATURE_ED25519 0x0807

/* HPKE */
#define PTLS_HPKE_MODE_BASE 0
#define PTLS_HPKE_MODE_PSK 1
#define PTLS_HPKE_MODE_AUTH 2
#define PTLS_HPKE_MODE_AUTH_PSK 3
#define PTLS_HPKE_KEM_P256_SHA256 16
#define PTLS_HPKE_KEM_P384_SHA384 17
#define PTLS_HPKE_KEM_X25519_SHA256 32
#define PTLS_HPKE_HKDF_SHA256 1
#define PTLS_HPKE_HKDF_SHA384 2
#define PTLS_HPKE_HKDF_SHA512 3
#define PTLS_HPKE_AEAD_AES_128_GCM 1
#define PTLS_HPKE_AEAD_AES_256_GCM 2
#define PTLS_HPKE_AEAD_CHACHA20POLY1305 3

/* error classes and macros */
#define PTLS_ERROR_CLASS_SELF_ALERT 0
#define PTLS_ERROR_CLASS_PEER_ALERT 0x100
#define PTLS_ERROR_CLASS_INTERNAL 0x200

#define PTLS_ERROR_GET_CLASS(e) ((e) & ~0xff)
#define PTLS_ALERT_TO_SELF_ERROR(e) ((e) + PTLS_ERROR_CLASS_SELF_ALERT)
#define PTLS_ALERT_TO_PEER_ERROR(e) ((e) + PTLS_ERROR_CLASS_PEER_ALERT)
#define PTLS_ERROR_TO_ALERT(e) ((e) & 0xff)

/* the HKDF prefix */
#define PTLS_HKDF_EXPAND_LABEL_PREFIX "tls13 "

/* alerts */
#define PTLS_ALERT_LEVEL_WARNING 1
#define PTLS_ALERT_LEVEL_FATAL 2

#define PTLS_ALERT_CLOSE_NOTIFY 0
#define PTLS_ALERT_UNEXPECTED_MESSAGE 10
#define PTLS_ALERT_BAD_RECORD_MAC 20
#define PTLS_ALERT_HANDSHAKE_FAILURE 40
#define PTLS_ALERT_BAD_CERTIFICATE 42
#define PTLS_ALERT_UNSUPPORTED_CERTIFICATE 43
#define PTLS_ALERT_CERTIFICATE_REVOKED 44
#define PTLS_ALERT_CERTIFICATE_EXPIRED 45
#define PTLS_ALERT_CERTIFICATE_UNKNOWN 46
#define PTLS_ALERT_ILLEGAL_PARAMETER 47
#define PTLS_ALERT_UNKNOWN_CA 48
#define PTLS_ALERT_ACCESS_DENIED 49
#define PTLS_ALERT_DECODE_ERROR 50
#define PTLS_ALERT_DECRYPT_ERROR 51
#define PTLS_ALERT_PROTOCOL_VERSION 70
#define PTLS_ALERT_INTERNAL_ERROR 80
#define PTLS_ALERT_USER_CANCELED 90
#define PTLS_ALERT_MISSING_EXTENSION 109
#define PTLS_ALERT_UNSUPPORTED_EXTENSION 110
#define PTLS_ALERT_UNRECOGNIZED_NAME 112
#define PTLS_ALERT_UNKNOWN_PSK_IDENTITY 115
#define PTLS_ALERT_CERTIFICATE_REQUIRED 116
#define PTLS_ALERT_NO_APPLICATION_PROTOCOL 120
#define PTLS_ALERT_ECH_REQUIRED 121

/* TLS 1.2 */
#define PTLS_TLS12_MASTER_SECRET_SIZE 48
#define PTLS_TLS12_AAD_SIZE 13
#define PTLS_TLS12_AESGCM_FIXED_IV_SIZE 4
#define PTLS_TLS12_AESGCM_RECORD_IV_SIZE 8
#define PTLS_TLS12_CHACHAPOLY_FIXED_IV_SIZE 12
#define PTLS_TLS12_CHACHAPOLY_RECORD_IV_SIZE 0

/* internal errors */
#define PTLS_ERROR_NO_MEMORY (PTLS_ERROR_CLASS_INTERNAL + 1)
#define PTLS_ERROR_IN_PROGRESS (PTLS_ERROR_CLASS_INTERNAL + 2)
#define PTLS_ERROR_LIBRARY (PTLS_ERROR_CLASS_INTERNAL + 3)
#define PTLS_ERROR_INCOMPATIBLE_KEY (PTLS_ERROR_CLASS_INTERNAL + 4)
#define PTLS_ERROR_SESSION_NOT_FOUND (PTLS_ERROR_CLASS_INTERNAL + 5)
#define PTLS_ERROR_STATELESS_RETRY (PTLS_ERROR_CLASS_INTERNAL + 6)
#define PTLS_ERROR_NOT_AVAILABLE (PTLS_ERROR_CLASS_INTERNAL + 7)
#define PTLS_ERROR_COMPRESSION_FAILURE (PTLS_ERROR_CLASS_INTERNAL + 8)
#define PTLS_ERROR_REJECT_EARLY_DATA (PTLS_ERROR_CLASS_INTERNAL + 9)
#define PTLS_ERROR_DELEGATE (PTLS_ERROR_CLASS_INTERNAL + 10)
#define PTLS_ERROR_ASYNC_OPERATION (PTLS_ERROR_CLASS_INTERNAL + 11)
#define PTLS_ERROR_BLOCK_OVERFLOW (PTLS_ERROR_CLASS_INTERNAL + 12)

#define PTLS_ERROR_INCORRECT_BASE64 (PTLS_ERROR_CLASS_INTERNAL + 50)
#define PTLS_ERROR_PEM_LABEL_NOT_FOUND (PTLS_ERROR_CLASS_INTERNAL + 51)
#define PTLS_ERROR_BER_INCORRECT_ENCODING (PTLS_ERROR_CLASS_INTERNAL + 52)
#define PTLS_ERROR_BER_MALFORMED_TYPE (PTLS_ERROR_CLASS_INTERNAL + 53)
#define PTLS_ERROR_BER_MALFORMED_LENGTH (PTLS_ERROR_CLASS_INTERNAL + 54)
#define PTLS_ERROR_BER_EXCESSIVE_LENGTH (PTLS_ERROR_CLASS_INTERNAL + 55)
#define PTLS_ERROR_BER_ELEMENT_TOO_SHORT (PTLS_ERROR_CLASS_INTERNAL + 56)
#define PTLS_ERROR_BER_UNEXPECTED_EOC (PTLS_ERROR_CLASS_INTERNAL + 57)
#define PTLS_ERROR_DER_INDEFINITE_LENGTH (PTLS_ERROR_CLASS_INTERNAL + 58)
#define PTLS_ERROR_INCORRECT_ASN1_SYNTAX (PTLS_ERROR_CLASS_INTERNAL + 59)
#define PTLS_ERROR_INCORRECT_PEM_KEY_VERSION (PTLS_ERROR_CLASS_INTERNAL + 60)
#define PTLS_ERROR_INCORRECT_PEM_ECDSA_KEY_VERSION (PTLS_ERROR_CLASS_INTERNAL + 61)
#define PTLS_ERROR_INCORRECT_PEM_ECDSA_CURVE (PTLS_ERROR_CLASS_INTERNAL + 62)
#define PTLS_ERROR_INCORRECT_PEM_ECDSA_KEYSIZE (PTLS_ERROR_CLASS_INTERNAL + 63)
#define PTLS_ERROR_INCORRECT_ASN1_ECDSA_KEY_SYNTAX (PTLS_ERROR_CLASS_INTERNAL + 64)

#define PTLS_HANDSHAKE_TYPE_CLIENT_HELLO 1
#define PTLS_HANDSHAKE_TYPE_SERVER_HELLO 2
#define PTLS_HANDSHAKE_TYPE_NEW_SESSION_TICKET 4
#define PTLS_HANDSHAKE_TYPE_END_OF_EARLY_DATA 5
#define PTLS_HANDSHAKE_TYPE_ENCRYPTED_EXTENSIONS 8
#define PTLS_HANDSHAKE_TYPE_CERTIFICATE 11
#define PTLS_HANDSHAKE_TYPE_CERTIFICATE_REQUEST 13
#define PTLS_HANDSHAKE_TYPE_CERTIFICATE_VERIFY 15
#define PTLS_HANDSHAKE_TYPE_FINISHED 20
#define PTLS_HANDSHAKE_TYPE_KEY_UPDATE 24
#define PTLS_HANDSHAKE_TYPE_COMPRESSED_CERTIFICATE 25
#define PTLS_HANDSHAKE_TYPE_MESSAGE_HASH 254
#define PTLS_HANDSHAKE_TYPE_PSEUDO_HRR -1

#define PTLS_CERTIFICATE_TYPE_X509 0
#define PTLS_CERTIFICATE_TYPE_RAW_PUBLIC_KEY 2

#define PTLS_ZERO_DIGEST_SHA256                                                                                                    \
    {0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,                               \
     0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55}

#define PTLS_ZERO_DIGEST_SHA384                                                                                                    \
    {0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38, 0x4c, 0xd9, 0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a,                               \
     0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43, 0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda,                               \
     0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb, 0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b}

#define PTLS_ZERO_DIGEST_SHA512                                                                                                    \
    {0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd, 0xf1, 0x54, 0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07,                               \
     0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc, 0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce,                               \
     0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0, 0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,                               \
     0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81, 0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e}

#define PTLS_TO__STR(n) #n
#define PTLS_TO_STR(n) PTLS_TO__STR(n)

/**
 * default maximum of tickets to send (see ptls_context_t::ticket_requests.server.max_count)
 */
#define PTLS_DEFAULT_MAX_TICKETS_TO_SERVE 4

typedef struct st_ptls_t ptls_t;
typedef struct st_ptls_context_t ptls_context_t;
typedef struct st_ptls_key_schedule_t ptls_key_schedule_t;

/**
 * represents a sequence of octets
 */
typedef struct st_ptls_iovec_t {
    uint8_t *base;
    size_t len;
} ptls_iovec_t;

/**
 * used for storing output
 */
typedef struct st_ptls_buffer_t {
    uint8_t *base;
    size_t capacity;
    size_t off;
    uint8_t is_allocated; /* boolean */
    uint8_t align_bits;   /* if particular alignment is required, set to log2(alignment); otherwize zero */
} ptls_buffer_t;

/**
 * key exchange context built by ptls_key_exchange_algorithm::create.
 */
typedef struct st_ptls_key_exchange_context_t {
    /**
     * the underlying algorithm
     */
    const struct st_ptls_key_exchange_algorithm_t *algo;
    /**
     * public key of this context
     */
    ptls_iovec_t pubkey;
    /**
     * This function can be used for deriving a shared secret or for destroying the context.
     * When `secret` is non-NULL, this callback derives the shared secret using the private key of the context and the peer key
     * being given, and sets the value in `secret`. The memory pointed to by `secret->base` must be freed by the caller by calling
     * `free`. When `release` is set, the callee frees resources allocated to the context and set *keyex to NULL. Upon failure
     * (i.e., when an PTLS error code is returned), `*pubkey` and `*secret` either remain unchanged or are zero-cleared.
     */
    int (*on_exchange)(struct st_ptls_key_exchange_context_t **keyex, int release, ptls_iovec_t *secret, ptls_iovec_t peerkey);
} ptls_key_exchange_context_t;

/**
 * A key exchange algorithm.
 */
typedef const struct st_ptls_key_exchange_algorithm_t {
    /**
     * ID defined by the TLS specification
     */
    uint16_t id;
    /**
     * Creates a context for asynchronous key exchange. The function is called when ClientHello is generated. The on_exchange
     * callback of the created context is called when the client receives ServerHello.
     */
    int (*create)(const struct st_ptls_key_exchange_algorithm_t *algo, ptls_key_exchange_context_t **ctx);
    /**
     * Implements synchronous key exchange. Called when ServerHello is generated.
     * Given a public key provided by the peer (`peerkey`), this callback generates an ephemeral private and public key, and returns
     * the public key (`pubkey`) and a secret (`secret`) derived from the peerkey and private key.
     * Upon failure (i.e., when an PTLS error code is returned), `*pubkey` and `*secret` either remain unchanged or are
     * zero-cleared.
     */
    int (*exchange)(const struct st_ptls_key_exchange_algorithm_t *algo, ptls_iovec_t *pubkey, ptls_iovec_t *secret,
                    ptls_iovec_t peerkey);
    /**
     * crypto-specific data
     */
    intptr_t data;
    /**
     * Description as defined in the IANA TLS registry
     */
    const char *name;
} ptls_key_exchange_algorithm_t;

/**
 * context of a symmetric cipher
 */
typedef struct st_ptls_cipher_context_t {
    const struct st_ptls_cipher_algorithm_t *algo;
    /* field above this line must not be altered by the crypto binding */
    void (*do_dispose)(struct st_ptls_cipher_context_t *ctx);
    void (*do_init)(struct st_ptls_cipher_context_t *ctx, const void *iv);
    void (*do_transform)(struct st_ptls_cipher_context_t *ctx, void *output, const void *input, size_t len);
} ptls_cipher_context_t;

/**
 * a symmetric cipher
 */
typedef const struct st_ptls_cipher_algorithm_t {
    const char *name;
    size_t key_size;
    size_t block_size;
    size_t iv_size;
    size_t context_size;
    int (*setup_crypto)(ptls_cipher_context_t *ctx, int is_enc, const void *key);
} ptls_cipher_algorithm_t;

/**
 * This object specifies symmetric cipher to be calculated alongside the AEAD encryption.
 * QUIC stacks can use this object to apply QUIC header protection and AEAD encryption in one shot.
 */
typedef struct st_ptls_aead_supplementary_encryption_t {
    /**
     * Cipher context to be used.
     */
    ptls_cipher_context_t *ctx;
    /**
     * Input to the cipher.
     * This field may point to the output of AEAD encryption, in which case the input will be read after AEAD encryption is
     * complete.
     */
    const void *input;
    /**
     * Output.
     */
    uint8_t output[16];
} ptls_aead_supplementary_encryption_t;

/**
 * AEAD context.
 * AEAD implementations are allowed to stuff data at the end of the struct; see `ptls_aead_algorithm_t::setup_crypto`.
 * Ciphers for TLS over TCP MUST implement `do_encrypt`, `do_encrypt_v`, `do_decrypt`.
 * `do_encrypt_init`, `~update`, `~final` are obsolete, and therefore may not be available.
 */
typedef struct st_ptls_aead_context_t {
    /**
     * Points to the algorithm. This field is governed by picotls core; backends must not alter.
     */
    const struct st_ptls_aead_algorithm_t *algo;
    /**
     * Mandatory callback that disposes of all the backend-specific data.
     */
    void (*dispose_crypto)(struct st_ptls_aead_context_t *ctx);
    /**
     * Mandatory callback that returns the static IV. The size of IV is available as `ptls_aead_algorithm_t::iv_size`.
     */
    void (*do_get_iv)(struct st_ptls_aead_context_t *ctx, void *iv);
    /**
     * Mandatory callback that sets the static IV. The size of IV is available as `ptls_aead_algorithm_t::iv_size`.
     */
    void (*do_set_iv)(struct st_ptls_aead_context_t *ctx, const void *iv);
    /**
     * Deprecated.
     */
    void (*do_encrypt_init)(struct st_ptls_aead_context_t *ctx, uint64_t seq, const void *aad, size_t aadlen);
    /**
     * Deprecated.
     */
    size_t (*do_encrypt_update)(struct st_ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen);
    /**
     * Deprecated.
     */
    size_t (*do_encrypt_final)(struct st_ptls_aead_context_t *ctx, void *output);
    /**
     * Mandatory callback that does "one-shot" encryption of an AEAD block.
     * When `supp` is set to non-NULL, the callback must also encrypt the supplementary block.
     * Backends may set this field to `ptls_aead__do_encrypt` that calls `do_encrypt_v` and `ptls_cipher_*` functions for handling
     * the supplimentary block.
     */
    void (*do_encrypt)(struct st_ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen, uint64_t seq,
                       const void *aad, size_t aadlen, ptls_aead_supplementary_encryption_t *supp);
    /**
     * Variant of `do_encrypt` that gathers input from multiple blocks. Support for this callback is also mandatory.
     * Legacy backends may set this field to `ptls_aead__do_encrypt_v` that calls `do_encrypt_init`, `do_encrypt_update`,
     * `do_encrypt_final`.
     */
    void (*do_encrypt_v)(struct st_ptls_aead_context_t *ctx, void *output, ptls_iovec_t *input, size_t incnt, uint64_t seq,
                         const void *aad, size_t aadlen);
    /**
     * Mandatory callback for decrypting an AEAD block.
     * If successful, returns the amount of cleartext bytes being written to output. Otherwise, returns SIZE_MAX.
     */
    size_t (*do_decrypt)(struct st_ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen, uint64_t seq,
                         const void *aad, size_t aadlen);
} ptls_aead_context_t;

/**
 * An AEAD cipher.
 */
typedef const struct st_ptls_aead_algorithm_t {
    /**
     * name (following the convention of `openssl ciphers -v ALL`)
     */
    const char *name;
    /**
     * confidentiality_limit (max records / packets sent before re-key)
     */
    const uint64_t confidentiality_limit;
    /**
     * integrity_limit (max decryption failure records / packets before re-key)
     */
    const uint64_t integrity_limit;
    /**
     * the underlying key stream
     */
    ptls_cipher_algorithm_t *ctr_cipher;
    /**
     * the underlying ecb cipher (might not be available)
     */
    ptls_cipher_algorithm_t *ecb_cipher;
    /**
     * key size
     */
    size_t key_size;
    /**
     * size of the IV
     */
    size_t iv_size;
    /**
     * size of the tag
     */
    size_t tag_size;
    /**
     * TLS/1.2 Security Parameters (AEAD without support for TLS 1.2 must set both values to 0)
     */
    struct {
        size_t fixed_iv_size;
        size_t record_iv_size;
    } tls12;
    /**
     * if encrypted bytes are going to be written using non-temporal store instructions (i.e., skip cache)
     */
    unsigned non_temporal : 1;
    /**
     * log2(alignment) being required
     */
    uint8_t align_bits;
    /**
     * size of memory allocated for `ptls_aead_context_t`
     */
    size_t context_size;
    /**
     * Backend callback called to setup `ptls_aead_context_t`.
     * Backends are allowed to stuff arbitrary data at the end of `ptls_aead_context_t`; actual size of the memory chunk being
     * allocated is that specified by `ptls_aead_algorithm_t::context_size`. When the `setup_crypto` callback is called, all the
     * fields outside of `ptls_aead_context_t` will be in undefined state; it is the responsibility of the callback to initialize
     * them, as well as the callbacks of `ptls_aead_context_t` that the backend supports.
     * A non-zero return value indicates failure, in which case the error will propagate as `ptls_aead_new` returning NULL.
     */
    int (*setup_crypto)(ptls_aead_context_t *ctx, int is_enc, const void *key, const void *iv);
} ptls_aead_algorithm_t;

/**
 *
 */
typedef enum en_ptls_hash_final_mode_t {
    /**
     * obtains the digest and frees the context
     */
    PTLS_HASH_FINAL_MODE_FREE = 0,
    /**
     * obtains the digest and reset the context to initial state
     */
    PTLS_HASH_FINAL_MODE_RESET = 1,
    /**
     * obtains the digest while leaving the context as-is
     */
    PTLS_HASH_FINAL_MODE_SNAPSHOT = 2
} ptls_hash_final_mode_t;

/**
 * A hash context.
 */
typedef struct st_ptls_hash_context_t {
    /**
     * feeds additional data into the hash context
     */
    void (*update)(struct st_ptls_hash_context_t *ctx, const void *src, size_t len);
    /**
     * returns the digest and performs necessary operation specified by mode
     */
    void (*final)(struct st_ptls_hash_context_t *ctx, void *md, ptls_hash_final_mode_t mode);
    /**
     * creates a copy of the hash context
     */
    struct st_ptls_hash_context_t *(*clone_)(struct st_ptls_hash_context_t *src);
} ptls_hash_context_t;

/**
 * A hash algorithm and its properties.
 */
typedef const struct st_ptls_hash_algorithm_t {
    /**
     * name of the hash algorithm
     */
    const char *name;
    /**
     * block size
     */
    size_t block_size;
    /**
     * digest size
     */
    size_t digest_size;
    /**
     * constructor that creates the hash context
     */
    ptls_hash_context_t *(*create)(void);
    /**
     * digest of zero-length octets
     */
    uint8_t empty_digest[PTLS_MAX_DIGEST_SIZE];
} ptls_hash_algorithm_t;

typedef const struct st_ptls_cipher_suite_t {
    /**
     * ID as defined by the TLS Cipher Suites registry
     */
    uint16_t id;
    /**
     * underlying AEAD algorithm
     */
    ptls_aead_algorithm_t *aead;
    /**
     * underlying hash algorithm
     */
    ptls_hash_algorithm_t *hash;
    /**
     * value of the "Description" field of the TLS Cipher Suites registry
     */
    const char *name;
} ptls_cipher_suite_t;

struct st_ptls_traffic_protection_t;

typedef struct st_ptls_message_emitter_t {
    ptls_buffer_t *buf;
    struct st_ptls_traffic_protection_t *enc;
    size_t record_header_length;
    int (*begin_message)(struct st_ptls_message_emitter_t *self);
    int (*commit_message)(struct st_ptls_message_emitter_t *self);
} ptls_message_emitter_t;

/**
 * HPKE KEM
 */
typedef const struct st_ptls_hpke_kem_t {
    uint16_t id;
    ptls_key_exchange_algorithm_t *keyex;
    ptls_hash_algorithm_t *hash;
} ptls_hpke_kem_t;

typedef struct st_ptls_hpke_cipher_suite_id_t {
    uint16_t kdf;
    uint16_t aead;
} ptls_hpke_cipher_suite_id_t;

typedef const struct st_ptls_hpke_cipher_suite_t {
    ptls_hpke_cipher_suite_id_t id;
    const char *name; /* in form of "<kdf>/<aead>" using the sames specified in IANA HPKE registry */
    ptls_hash_algorithm_t *hash;
    ptls_aead_algorithm_t *aead;
} ptls_hpke_cipher_suite_t;

#define PTLS_CALLBACK_TYPE0(ret, name)                                                                                             \
    typedef struct st_ptls_##name##_t {                                                                                            \
        ret (*cb)(struct st_ptls_##name##_t * self);                                                                               \
    } ptls_##name##_t

#define PTLS_CALLBACK_TYPE(ret, name, ...)                                                                                         \
    typedef struct st_ptls_##name##_t {                                                                                            \
        ret (*cb)(struct st_ptls_##name##_t * self, __VA_ARGS__);                                                                  \
    } ptls_##name##_t

typedef struct st_ptls_client_hello_psk_identity_t {
    ptls_iovec_t identity;
    uint32_t obfuscated_ticket_age;
    ptls_iovec_t binder;
} ptls_client_hello_psk_identity_t;

/**
 * arguments passsed to the on_client_hello callback
 */
typedef struct st_ptls_on_client_hello_parameters_t {
    /**
     * SNI value received from the client. The value is {NULL, 0} if the extension was absent.
     */
    ptls_iovec_t server_name;
    /**
     * Raw value of the client_hello message.
     */
    ptls_iovec_t raw_message;
    /**
     * points to the cipher-suites section of the raw_message (see above)
     */
    ptls_iovec_t cipher_suites;
    /**
     *
     */
    struct {
        ptls_iovec_t *list;
        size_t count;
    } negotiated_protocols;
    struct {
        const uint16_t *list;
        size_t count;
    } signature_algorithms;
    struct {
        const uint16_t *list;
        size_t count;
    } certificate_compression_algorithms;
    struct {
        const uint8_t *list;
        size_t count;
    } server_certificate_types;
    struct {
        const ptls_client_hello_psk_identity_t *list;
        size_t count;
    } psk_identities;
    /**
     * set to 1 if ClientHello is too old (or too new) to be handled by picotls
     */
    unsigned incompatible_version : 1;
} ptls_on_client_hello_parameters_t;

/**
 * returns current time in milliseconds (ptls_get_time can be used to return the physical time)
 */
PTLS_CALLBACK_TYPE0(uint64_t, get_time);
/**
 * after receiving ClientHello, the core calls the optional callback to give a chance to the swap the context depending on the input
 * values. The callback is required to call `ptls_set_server_name` if an SNI extension needs to be sent to the client.
 */
PTLS_CALLBACK_TYPE(int, on_client_hello, ptls_t *tls, ptls_on_client_hello_parameters_t *params);
/**
 * callback to generate the certificate message. `ptls_context::certificates` are set when the callback is set to NULL.
 */
PTLS_CALLBACK_TYPE(int, emit_certificate, ptls_t *tls, ptls_message_emitter_t *emitter, ptls_key_schedule_t *key_sched,
                   ptls_iovec_t context, int push_status_request, const uint16_t *compress_algos, size_t num_compress_algos);
/**
 * An object that represents an asynchronous task (e.g., RSA signature generation).
 * When `ptls_handshake` returns `PTLS_ERROR_ASYNC_OPERATION`, it has an associated task in flight. The user should obtain the
 * reference to the associated task by calling `ptls_get_async_job`, then either wait for the file descriptor obtained from
 * the `get_fd` callback to become readable, or set a completion callback via `set_completion_callback` and wait for its
 * invocation. Once notified, the user should invoke `ptls_handshake` again.
 * Async jobs typically provide support for only one of the two methods.
 */
typedef struct st_ptls_async_job_t {
    void (*destroy_)(struct st_ptls_async_job_t *self);
    /**
     * optional callback returning a file descriptor that becomes readable when the job is complete
     */
    int (*get_fd)(struct st_ptls_async_job_t *self);
    /**
     * optional callback for setting a completion callback
     */
    void (*set_completion_callback)(struct st_ptls_async_job_t *self, void (*cb)(void *), void *cbdata);
} ptls_async_job_t;
/**
 * When gerenating CertificateVerify, the core calls the callback to sign the handshake context using the certificate. This callback
 * supports asynchronous mode; see `ptls_openssl_sign_certificate_t` for more information.
 */
PTLS_CALLBACK_TYPE(int, sign_certificate, ptls_t *tls, ptls_async_job_t **async, uint16_t *selected_algorithm,
                   ptls_buffer_t *output, ptls_iovec_t input, const uint16_t *algorithms, size_t num_algorithms);
/**
 * after receiving Certificate, the core calls the callback to verify the certificate chain and to obtain a pointer to a
 * callback that should be used for verifying CertificateVerify. If an error occurs between a successful return from this
 * callback to the invocation of the verify_sign callback, verify_sign is called with both data and sign set to an empty buffer.
 * The implementor of the callback should use that as the opportunity to free any temporary data allocated for the verify_sign
 * callback.
 * The name of the server to be verified, if any, is provided explicitly as `server_name`. When ECH is offered by the client but
 * the was rejected by the server, this value can be different from that being sent via `ptls_get_server_name`.
 */
typedef struct st_ptls_verify_certificate_t {
    int (*cb)(struct st_ptls_verify_certificate_t *self, ptls_t *tls, const char *server_name,
              int (**verify_sign)(void *verify_ctx, uint16_t algo, ptls_iovec_t data, ptls_iovec_t sign), void **verify_data,
              ptls_iovec_t *certs, size_t num_certs);
    /**
     * list of signature algorithms being supported, terminated by UINT16_MAX
     */
    const uint16_t *algos;
} ptls_verify_certificate_t;
/**
 * Encrypt-and-signs (or verify-and-decrypts) a ticket (server-only).
 * When used for encryption (i.e., is_encrypt being set), the function should return 0 if successful, or else a non-zero value.
 * When used for decryption, the function should return 0 (successful), PTLS_ERROR_REJECT_EARLY_DATA (successful, but 0-RTT is
 * forbidden), or any other value to indicate failure.
 */
PTLS_CALLBACK_TYPE(int, encrypt_ticket, ptls_t *tls, int is_encrypt, ptls_buffer_t *dst, ptls_iovec_t src);
/**
 * saves a ticket (client-only)
 */
PTLS_CALLBACK_TYPE(int, save_ticket, ptls_t *tls, ptls_iovec_t input);
/**
 * event logging (incl. secret logging)
 */
typedef struct st_ptls_log_event_t {
    void (*cb)(struct st_ptls_log_event_t *self, ptls_t *tls, const char *type, const char *fmt, ...)
        __attribute__((format(printf, 4, 5)));
} ptls_log_event_t;
/**
 * reference counting
 */
PTLS_CALLBACK_TYPE(void, update_open_count, ssize_t delta);
/**
 * applications that have their own record layer can set this function to derive their own traffic keys from the traffic secret.
 * The cipher-suite that is being associated to the connection can be obtained by calling the ptls_get_cipher function.
 */
PTLS_CALLBACK_TYPE(int, update_traffic_key, ptls_t *tls, int is_enc, size_t epoch, const void *secret);
/**
 * callback for every extension detected during decoding
 */
PTLS_CALLBACK_TYPE(int, on_extension, ptls_t *tls, uint8_t hstype, uint16_t exttype, ptls_iovec_t extdata);
/**
 *
 */
typedef struct st_ptls_decompress_certificate_t {
    /**
     * list of supported algorithms terminated by UINT16_MAX
     */
    const uint16_t *supported_algorithms;
    /**
     * callback that decompresses the message
     */
    int (*cb)(struct st_ptls_decompress_certificate_t *self, ptls_t *tls, uint16_t algorithm, ptls_iovec_t output,
              ptls_iovec_t input);
} ptls_decompress_certificate_t;
/**
 * ECH: creates the AEAD context to be used for "Open"-ing inner CH. Given `config_id`, the callback looks up the ECH config and the
 * corresponding private key, invokes `ptls_hpke_setup_base_r` with provided `cipher`, `enc`, and `info_prefix` (which will be
 * "tls ech" || 00).
 */
PTLS_CALLBACK_TYPE(ptls_aead_context_t *, ech_create_opener, ptls_hpke_kem_t **kem, ptls_hpke_cipher_suite_t **cipher, ptls_t *tls,
                   uint8_t config_id, ptls_hpke_cipher_suite_id_t cipher_id, ptls_iovec_t enc, ptls_iovec_t info_prefix);

/**
 * the configuration
 */
struct st_ptls_context_t {
    /**
     * PRNG to be used
     */
    void (*random_bytes)(void *buf, size_t len);
    /**
     *
     */
    ptls_get_time_t *get_time;
    /**
     * list of supported key-exchange algorithms terminated by NULL
     */
    ptls_key_exchange_algorithm_t **key_exchanges;
    /**
     * list of supported cipher-suites terminated by NULL
     */
    ptls_cipher_suite_t **cipher_suites;
    /**
     * list of certificates
     */
    struct {
        ptls_iovec_t *list;
        size_t count;
    } certificates;
    /**
     * External pre-shared key used for mutual authentication. Unless when using PSK, all the fields must be set to NULL / 0.
     */
    struct {
        ptls_iovec_t identity;
        ptls_iovec_t secret;
        /**
         * (mandatory) hash algorithm associated to the PSK; cipher-suites not sharing the same `ptls_hash_algorithm_t` will be
         * ignored
         */
        ptls_hash_algorithm_t *hash;
    } pre_shared_key;
    /**
     * ECH
     */
    struct {
        struct {
            /**
             * list of HPKE symmetric cipher-suites (set to NULL to disable ECH altogether)
             */
            ptls_hpke_cipher_suite_t **ciphers;
            /**
             * KEMs being supported
             */
            ptls_hpke_kem_t **kems;
        } client;
        struct {
            /**
             * callback that does ECDH key exchange and returns the AEAD context
             */
            ptls_ech_create_opener_t *create_opener;
            /**
             * ECHConfigList to be sent to the client when there is mismatch (or when the client sends a grease)
             */
            ptls_iovec_t retry_configs;
        } server;
    } ech;
    /**
     *
     */
    ptls_on_client_hello_t *on_client_hello;
    /**
     *
     */
    ptls_emit_certificate_t *emit_certificate;
    /**
     *
     */
    ptls_sign_certificate_t *sign_certificate;
    /**
     *
     */
    ptls_verify_certificate_t *verify_certificate;
    /**
     * lifetime of a session ticket (server-only)
     */
    uint32_t ticket_lifetime;
    /**
     * maximum permitted size of early data (server-only)
     */
    uint32_t max_early_data_size;
    /**
     * maximum size of the message buffer (default: 0 = unlimited = 3 + 2^24 bytes)
     */
    size_t max_buffer_size;
    /**
     * this field is obsolete and ignored
     */
    const char *hkdf_label_prefix__obsolete;
    /**
     * if set, psk handshakes use (ec)dhe
     */
    unsigned require_dhe_on_psk : 1;
    /**
     * if exporter master secrets should be recorded
     */
    unsigned use_exporter : 1;
    /**
     * if ChangeCipherSpec record should be sent during handshake. If the client sends CCS, the server sends one in response
     * regardless of the value of this flag. See RFC 8446 Appendix D.3.
     */
    unsigned send_change_cipher_spec : 1;
    /**
     * if set, the server requests client certificates to authenticate the client
     */
    unsigned require_client_authentication : 1;
    /**
     * if set, EOED will not be emitted or accepted
     */
    unsigned omit_end_of_early_data : 1;
    /**
     * This option turns on support for Raw Public Keys (RFC 7250).
     *
     * When running as a client, this option instructs the client to request the server to send raw public keys in place of X.509
     * certificate chain. The client should set its `certificate_verify` callback to one that is capable of validating the raw
     * public key that will be sent by the server.
     *
     * When running as a server, this option instructs the server to only handle clients requesting the use of raw public keys. If
     * the client does not, the handshake is rejected. Note however that the rejection happens only after the `on_client_hello`
     * callback is being called. Therefore, applications can support both X.509 and raw public keys by swapping `ptls_context_t` to
     * the correct one when that callback is being called (like handling swapping the contexts based on the value of SNI).
     */
    unsigned use_raw_public_keys : 1;
    /**
     * boolean indicating if the cipher-suite should be chosen based on server's preference
     */
    unsigned server_cipher_preference : 1;
    /**
     * boolean indicating if ChaCha20-Poly1305 should be reprioritized to the top of the server cipher list if a ChaCha20-Poly1305
     * cipher is at the top of the client cipher list
     */
    unsigned server_cipher_chacha_priority : 1;
    /**
     *
     */
    ptls_encrypt_ticket_t *encrypt_ticket;
    /**
     *
     */
    ptls_save_ticket_t *save_ticket;
    /**
     *
     */
    ptls_log_event_t *log_event;
    /**
     *
     */
    ptls_update_open_count_t *update_open_count;
    /**
     *
     */
    ptls_update_traffic_key_t *update_traffic_key;
    /**
     *
     */
    ptls_decompress_certificate_t *decompress_certificate;
    /**
     *
     */
    ptls_on_extension_t *on_extension;
    /**
     * (optional) list of supported tls12 cipher-suites terminated by NULL
     */
    ptls_cipher_suite_t **tls12_cipher_suites;
    /**
     * (optional) session ID Context to segment resumption
     */
    struct {
        uint8_t bytes[PTLS_SHA256_DIGEST_SIZE];
        unsigned is_set : 1;
    } ticket_context;
    /**
     * (optional) list of CAs advertised to clients as supported in the CertificateRequest message; each item must be DNs in DER
     * format. The values are sent to the client only when `ptls_context_t::require_client_authentication` is set to true.
     */
    struct {
        const ptls_iovec_t *list;
        size_t count;
    } client_ca_names;
    /**
     * (optional)
     */
    struct {
        /**
         * if set to non-zero and if the save_ticket callback is provided, a ticket_request extension containing the specified
         * values is sent
         */
        struct {
            uint8_t new_session_count;
            uint8_t resumption_count;
        } client;
        /**
         * if set to non-zero, the maximum number of tickets being sent is capped to the specifed value; if set to zero, the maximum
         * adopted is PTLS_DEFAULT_MAX_TICKETS_TO_SERVE.
         */
        struct {
            uint8_t max_count;
        } server;
    } ticket_requests;
};

typedef struct st_ptls_raw_extension_t {
    uint16_t type;
    ptls_iovec_t data;
} ptls_raw_extension_t;

typedef enum en_ptls_early_data_acceptance_t {
    PTLS_EARLY_DATA_ACCEPTANCE_UNKNOWN = 0,
    PTLS_EARLY_DATA_REJECTED,
    PTLS_EARLY_DATA_ACCEPTED
} ptls_early_data_acceptance_t;

/**
 * optional arguments to client-driven handshake
 */
#ifdef _WINDOWS
/* suppress warning C4201: nonstandard extension used: nameless struct/union */
#pragma warning(push)
#pragma warning(disable : 4201)
#endif
typedef struct st_ptls_handshake_properties_t {
    union {
        struct {
            /**
             * list of protocols offered through ALPN
             */
            struct {
                const ptls_iovec_t *list;
                size_t count;
            } negotiated_protocols;
            /**
             * session ticket sent to the application via save_ticket callback
             */
            ptls_iovec_t session_ticket;
            /**
             * pointer to store the maximum size of early-data that can be sent immediately. If set to non-NULL, the first call to
             * ptls_handshake (or ptls_handle_message) will set `*max_early_data` to the value obtained from the session ticket, or
             * to zero if early-data cannot be sent. If NULL, early data will not be used.
             */
            size_t *max_early_data_size;
            /**
             * If early-data has been accepted by peer, or if the state is still unknown. The state changes anytime after handshake
             * keys become available. Applications can peek the tri-state variable every time it calls `ptls_hanshake` or
             * `ptls_handle_message` to determine the result at the earliest moment. This is an output parameter.
             */
            ptls_early_data_acceptance_t early_data_acceptance;
            /**
             * negotiate the key exchange method before sending key_share
             */
            unsigned negotiate_before_key_exchange : 1;
            /**
             * ECH
             */
            struct {
                /**
                 * Config offered by server e.g., by HTTPS RR. If config.base is non-NULL but config.len is zero, a grease ECH will
                 * be sent, assuming that X25519-SHA256 KEM and SHA256-AES-128-GCM HPKE cipher is available.
                 */
                ptls_iovec_t configs;
                /**
                 * slot to save the config obtained from server on mismatch; user must free the returned blob by calling `free`
                 */
                ptls_iovec_t *retry_configs;
            } ech;
        } client;
        struct {
            /**
             * psk binder being selected (len is set to zero if none)
             */
            struct {
                uint8_t base[PTLS_MAX_DIGEST_SIZE];
                size_t len;
            } selected_psk_binder;
            /**
             * parameters related to use of the Cookie extension
             */
            struct {
                /**
                 * HMAC key to protect the integrity of the cookie. The key should be as long as the digest size of the first
                 * ciphersuite specified in ptls_context_t (i.e. the hash algorithm of the best ciphersuite that can be chosen).
                 */
                const void *key;
                /**
                 * additional data to be used for verifying the cookie
                 */
                ptls_iovec_t additional_data;
            } cookie;
            /**
             * if HRR should always be sent
             */
            unsigned enforce_retry : 1;
            /**
             * if retry should be stateless (cookie.key MUST be set when this option is used)
             */
            unsigned retry_uses_cookie : 1;
        } server;
    };
    /**
     * an optional list of additional extensions to send either in CH or EE, terminated by type == UINT16_MAX
     */
    ptls_raw_extension_t *additional_extensions;
    /**
     * an optional callback that returns a boolean value indicating if a particular extension should be collected
     */
    int (*collect_extension)(ptls_t *tls, struct st_ptls_handshake_properties_t *properties, uint16_t type);
    /**
     * an optional callback that reports the extensions being collected
     */
    int (*collected_extensions)(ptls_t *tls, struct st_ptls_handshake_properties_t *properties, ptls_raw_extension_t *extensions);
} ptls_handshake_properties_t;
#ifdef _WINDOWS
#pragma warning(pop)
#endif
#ifdef _WINDOWS
/* suppress warning C4293: >> shift count negative or too big */
#pragma warning(disable : 4293)
#endif
/**
 * builds a new ptls_iovec_t instance using the supplied parameters
 */
static ptls_iovec_t ptls_iovec_init(const void *p, size_t len);
/**
 * initializes a buffer, setting the default destination to the small buffer provided as the argument.
 */
static void ptls_buffer_init(ptls_buffer_t *buf, void *smallbuf, size_t smallbuf_size);
/**
 * disposes a buffer, freeing resources allocated by the buffer itself (if any)
 */
static void ptls_buffer_dispose(ptls_buffer_t *buf);
/**
 * internal
 */
void ptls_buffer__release_memory(ptls_buffer_t *buf);
/**
 * reserves space for additional amount of memory
 */
int ptls_buffer_reserve(ptls_buffer_t *buf, size_t delta);
/**
 * reserves space for additional amount of memory, requiring `buf->base` to follow specified alignment
 */
int ptls_buffer_reserve_aligned(ptls_buffer_t *buf, size_t delta, uint8_t align_bits);
/**
 * internal
 */
int ptls_buffer__do_pushv(ptls_buffer_t *buf, const void *src, size_t len);
/**
 * internal
 */
int ptls_buffer__adjust_quic_blocksize(ptls_buffer_t *buf, size_t body_size);
/**
 * internal
 */
int ptls_buffer__adjust_asn1_blocksize(ptls_buffer_t *buf, size_t body_size);
/**
 * pushes an unsigned bigint
 */
int ptls_buffer_push_asn1_ubigint(ptls_buffer_t *buf, const void *bignum, size_t size);
/**
 * encodes a quic varint (maximum length is PTLS_ENCODE_QUICINT_CAPACITY)
 */
static uint8_t *ptls_encode_quicint(uint8_t *p, uint64_t v);
#define PTLS_ENCODE_QUICINT_CAPACITY 8

#define PTLS_QUICINT_MAX 4611686018427387903 // (1 << 62) - 1
#define PTLS_QUICINT_LONGEST_STR "4611686018427387903"

#define ptls_buffer_pushv(buf, src, len)                                                                                           \
    do {                                                                                                                           \
        if ((ret = ptls_buffer__do_pushv((buf), (src), (len))) != 0)                                                               \
            goto Exit;                                                                                                             \
    } while (0)

#define ptls_buffer_push(buf, ...)                                                                                                 \
    do {                                                                                                                           \
        if ((ret = ptls_buffer__do_pushv((buf), (uint8_t[]){__VA_ARGS__}, sizeof((uint8_t[]){__VA_ARGS__}))) != 0)                 \
            goto Exit;                                                                                                             \
    } while (0)

#define ptls_buffer_push16(buf, v)                                                                                                 \
    do {                                                                                                                           \
        uint16_t _v = (v);                                                                                                         \
        ptls_buffer_push(buf, (uint8_t)(_v >> 8), (uint8_t)_v);                                                                    \
    } while (0)

#define ptls_buffer_push24(buf, v)                                                                                                 \
    do {                                                                                                                           \
        uint32_t _v = (v);                                                                                                         \
        ptls_buffer_push(buf, (uint8_t)(_v >> 16), (uint8_t)(_v >> 8), (uint8_t)_v);                                               \
    } while (0)

#define ptls_buffer_push32(buf, v)                                                                                                 \
    do {                                                                                                                           \
        uint32_t _v = (v);                                                                                                         \
        ptls_buffer_push(buf, (uint8_t)(_v >> 24), (uint8_t)(_v >> 16), (uint8_t)(_v >> 8), (uint8_t)_v);                          \
    } while (0)

#define ptls_buffer_push64(buf, v)                                                                                                 \
    do {                                                                                                                           \
        uint64_t _v = (v);                                                                                                         \
        ptls_buffer_push(buf, (uint8_t)(_v >> 56), (uint8_t)(_v >> 48), (uint8_t)(_v >> 40), (uint8_t)(_v >> 32),                  \
                         (uint8_t)(_v >> 24), (uint8_t)(_v >> 16), (uint8_t)(_v >> 8), (uint8_t)_v);                               \
    } while (0)

#define ptls_buffer_push_quicint(buf, v)                                                                                           \
    do {                                                                                                                           \
        if ((ret = ptls_buffer_reserve((buf), PTLS_ENCODE_QUICINT_CAPACITY)) != 0)                                                 \
            goto Exit;                                                                                                             \
        uint8_t *d = ptls_encode_quicint((buf)->base + (buf)->off, (v));                                                           \
        (buf)->off = d - (buf)->base;                                                                                              \
    } while (0)

#define ptls_buffer_push_block(buf, _capacity, block)                                                                              \
    do {                                                                                                                           \
        size_t capacity = (_capacity);                                                                                             \
        ptls_buffer_pushv((buf), (uint8_t *)"\0\0\0\0\0\0\0", capacity != -1 ? capacity : 1);                                      \
        size_t body_start = (buf)->off;                                                                                            \
        do {                                                                                                                       \
            block                                                                                                                  \
        } while (0);                                                                                                               \
        size_t body_size = (buf)->off - body_start;                                                                                \
        if (capacity != -1) {                                                                                                      \
            if (capacity < sizeof(size_t) && body_size >= (size_t)1 << (capacity * 8)) {                                           \
                ret = PTLS_ERROR_BLOCK_OVERFLOW;                                                                                   \
                goto Exit;                                                                                                         \
            }                                                                                                                      \
            for (; capacity != 0; --capacity)                                                                                      \
                (buf)->base[body_start - capacity] = (uint8_t)(body_size >> (8 * (capacity - 1)));                                 \
        } else {                                                                                                                   \
            if ((ret = ptls_buffer__adjust_quic_blocksize((buf), body_size)) != 0)                                                 \
                goto Exit;                                                                                                         \
        }                                                                                                                          \
    } while (0)

#define ptls_buffer_push_asn1_block(buf, block)                                                                                    \
    do {                                                                                                                           \
        ptls_buffer_push((buf), 0xff); /* dummy */                                                                                 \
        size_t body_start = (buf)->off;                                                                                            \
        do {                                                                                                                       \
            block                                                                                                                  \
        } while (0);                                                                                                               \
        size_t body_size = (buf)->off - body_start;                                                                                \
        if (body_size < 128) {                                                                                                     \
            (buf)->base[body_start - 1] = (uint8_t)body_size;                                                                      \
        } else {                                                                                                                   \
            if ((ret = ptls_buffer__adjust_asn1_blocksize((buf), body_size)) != 0)                                                 \
                goto Exit;                                                                                                         \
        }                                                                                                                          \
    } while (0)

#define ptls_buffer_push_asn1_sequence(buf, block)                                                                                 \
    do {                                                                                                                           \
        ptls_buffer_push((buf), 0x30);                                                                                             \
        ptls_buffer_push_asn1_block((buf), block);                                                                                 \
    } while (0)

#define ptls_buffer_push_message_body(buf, key_sched, type, block)                                                                 \
    do {                                                                                                                           \
        ptls_buffer_t *_buf = (buf);                                                                                               \
        ptls_key_schedule_t *_key_sched = (key_sched);                                                                             \
        size_t mess_start = _buf->off;                                                                                             \
        ptls_buffer_push(_buf, (type));                                                                                            \
        ptls_buffer_push_block(_buf, 3, block);                                                                                    \
        if (_key_sched != NULL)                                                                                                    \
            ptls__key_schedule_update_hash(_key_sched, _buf->base + mess_start, _buf->off - mess_start, 0);                        \
    } while (0)

#define ptls_push_message(emitter, key_sched, type, block)                                                                         \
    do {                                                                                                                           \
        ptls_message_emitter_t *_emitter = (emitter);                                                                              \
        if ((ret = _emitter->begin_message(_emitter)) != 0)                                                                        \
            goto Exit;                                                                                                             \
        ptls_buffer_push_message_body(_emitter->buf, (key_sched), (type), block);                                                  \
        if ((ret = _emitter->commit_message(_emitter)) != 0)                                                                       \
            goto Exit;                                                                                                             \
    } while (0)

int ptls_decode8(uint8_t *value, const uint8_t **src, const uint8_t *end);
int ptls_decode16(uint16_t *value, const uint8_t **src, const uint8_t *end);
int ptls_decode24(uint32_t *value, const uint8_t **src, const uint8_t *end);
int ptls_decode32(uint32_t *value, const uint8_t **src, const uint8_t *end);
int ptls_decode64(uint64_t *value, const uint8_t **src, const uint8_t *end);
uint64_t ptls_decode_quicint(const uint8_t **src, const uint8_t *end);

#define ptls_decode_open_block(src, end, capacity, block)                                                                          \
    do {                                                                                                                           \
        size_t _capacity = (capacity);                                                                                             \
        size_t _block_size;                                                                                                        \
        if (_capacity == -1) {                                                                                                     \
            uint64_t _block_size64;                                                                                                \
            const uint8_t *_src = (src);                                                                                           \
            if ((_block_size64 = ptls_decode_quicint(&_src, end)) == UINT64_MAX ||                                                 \
                (sizeof(size_t) < 8 && (_block_size64 >> (8 * sizeof(size_t))) != 0)) {                                            \
                ret = PTLS_ALERT_DECODE_ERROR;                                                                                     \
                goto Exit;                                                                                                         \
            }                                                                                                                      \
            (src) = _src;                                                                                                          \
            _block_size = (size_t)_block_size64;                                                                                   \
        } else {                                                                                                                   \
            if (_capacity > (size_t)(end - (src))) {                                                                               \
                ret = PTLS_ALERT_DECODE_ERROR;                                                                                     \
                goto Exit;                                                                                                         \
            }                                                                                                                      \
            _block_size = 0;                                                                                                       \
            do {                                                                                                                   \
                _block_size = _block_size << 8 | *(src)++;                                                                         \
            } while (--_capacity != 0);                                                                                            \
        }                                                                                                                          \
        if (_block_size > (size_t)(end - (src))) {                                                                                 \
            ret = PTLS_ALERT_DECODE_ERROR;                                                                                         \
            goto Exit;                                                                                                             \
        }                                                                                                                          \
        do {                                                                                                                       \
            const uint8_t *const end = (src) + _block_size;                                                                        \
            do {                                                                                                                   \
                block                                                                                                              \
            } while (0);                                                                                                           \
            if ((src) != end) {                                                                                                    \
                ret = PTLS_ALERT_DECODE_ERROR;                                                                                     \
                goto Exit;                                                                                                         \
            }                                                                                                                      \
        } while (0);                                                                                                               \
    } while (0)

#define ptls_decode_assert_block_close(src, end)                                                                                   \
    do {                                                                                                                           \
        if ((src) != end) {                                                                                                        \
            ret = PTLS_ALERT_DECODE_ERROR;                                                                                         \
            goto Exit;                                                                                                             \
        }                                                                                                                          \
    } while (0);

#define ptls_decode_block(src, end, capacity, block)                                                                               \
    do {                                                                                                                           \
        ptls_decode_open_block((src), end, capacity, block);                                                                       \
        ptls_decode_assert_block_close((src), end);                                                                                \
    } while (0)

#if PTLS_HAVE_LOG
#define PTLS_LOG__DO_LOG(module, name, conn_state, get_sni, get_sni_arg, add_time, block)                                          \
    do {                                                                                                                           \
        int ptlslog_include_appdata = 0;                                                                                           \
        do {                                                                                                                       \
            ptls_log__do_write_start(&logpoint, (add_time));                                                                       \
            do {                                                                                                                   \
                block                                                                                                              \
            } while (0);                                                                                                           \
            ptlslog_include_appdata =                                                                                              \
                ptls_log__do_write_end(&logpoint, (conn_state), (get_sni), (get_sni_arg), ptlslog_include_appdata);                \
        } while (PTLS_UNLIKELY(ptlslog_include_appdata));                                                                          \
    } while (0)
#else
#define PTLS_LOG__DO_LOG(module, name, conn_state, get_sni, get_sni_arg, add_time, block) /* don't generate code */
#endif

#define PTLS_LOG_DEFINE_POINT(_module, _name, _var)                                                                                \
    static struct st_ptls_log_point_t _var = {.name = PTLS_TO_STR(_module) ":" PTLS_TO_STR(_name)}

#define PTLS_LOG(module, name, block)                                                                                              \
    do {                                                                                                                           \
        PTLS_LOG_DEFINE_POINT(module, name, logpoint);                                                                             \
        if (PTLS_LIKELY(ptls_log_point_maybe_active(&logpoint) == 0))                                                              \
            break;                                                                                                                 \
        PTLS_LOG__DO_LOG(module, name, NULL, NULL, NULL, 1, {block});                                                              \
    } while (0)

#define PTLS_LOG_CONN(name, tls, block)                                                                                            \
    do {                                                                                                                           \
        PTLS_LOG_DEFINE_POINT(picotls, name, logpoint);                                                                            \
        uint32_t active = ptls_log_point_maybe_active(&logpoint);                                                                  \
        if (PTLS_LIKELY(active == 0))                                                                                              \
            break;                                                                                                                 \
        ptls_t *_tls = (tls);                                                                                                      \
        ptls_log_conn_state_t *conn_state = ptls_get_log_state(_tls);                                                              \
        active &= ptls_log_conn_maybe_active(conn_state, (const char *(*)(void *))ptls_get_server_name, _tls);                     \
        if (PTLS_LIKELY(active == 0))                                                                                              \
            break;                                                                                                                 \
        PTLS_LOG__DO_LOG(picotls, name, conn_state, (const char *(*)(void *))ptls_get_server_name, _tls, 1, {                      \
            PTLS_LOG_ELEMENT_PTR(tls, _tls);                                                                                       \
            do {                                                                                                                   \
                block                                                                                                              \
            } while (0);                                                                                                           \
        });                                                                                                                        \
    } while (0)

#define PTLS_LOG__ELEMENT_PREFIX_CORE(lit) ",\"" lit "\":"
#define PTLS_LOG__ELEMENT_PREFIX(lit) PTLS_LOG__ELEMENT_PREFIX_CORE(lit), sizeof(PTLS_LOG__ELEMENT_PREFIX_CORE(lit)) - 1
#define PTLS_LOG_ELEMENT_SAFESTR(name, value)                                                                                      \
    do {                                                                                                                           \
        const char *value_ = (value);                                                                                              \
        ptls_log__do_push_element_safestr(PTLS_LOG__ELEMENT_PREFIX(PTLS_TO_STR(name)), (value_), strlen(value_));                  \
    } while (0)
#define PTLS_LOG_ELEMENT_UNSAFESTR(name, value, value_len)                                                                         \
    ptls_log__do_push_element_unsafestr(PTLS_LOG__ELEMENT_PREFIX(PTLS_TO_STR(name)), (value), (value_len))
#define PTLS_LOG_ELEMENT_HEXDUMP(name, value, value_len)                                                                           \
    ptls_log__do_push_element_hexdump(PTLS_LOG__ELEMENT_PREFIX(PTLS_TO_STR(name)), (value), (value_len))
#define PTLS_LOG_ELEMENT_PTR(name, value) PTLS_LOG_ELEMENT_UNSIGNED(name, (uint64_t)(value))
#define PTLS_LOG_ELEMENT_SIGNED(name, value)                                                                                       \
    do {                                                                                                                           \
        if (sizeof(value) <= sizeof(int32_t)) {                                                                                    \
            ptls_log__do_push_element_signed32(PTLS_LOG__ELEMENT_PREFIX(PTLS_TO_STR(name)), (value));                              \
        } else {                                                                                                                   \
            ptls_log__do_push_element_signed64(PTLS_LOG__ELEMENT_PREFIX(PTLS_TO_STR(name)), (value));                              \
        }                                                                                                                          \
    } while (0)
#define PTLS_LOG__DO_ELEMENT_UNSIGNED(lit, value)                                                                                  \
    do {                                                                                                                           \
        if (sizeof(value) <= sizeof(uint32_t)) {                                                                                   \
            ptls_log__do_push_element_unsigned32(PTLS_LOG__ELEMENT_PREFIX(lit), (value));                                          \
        } else {                                                                                                                   \
            ptls_log__do_push_element_unsigned64(PTLS_LOG__ELEMENT_PREFIX(lit), (value));                                          \
        }                                                                                                                          \
    } while (0)
#define PTLS_LOG_ELEMENT_UNSIGNED(name, value) PTLS_LOG__DO_ELEMENT_UNSIGNED(PTLS_TO_STR(name), (value))
#define PTLS_LOG_ELEMENT_BOOL(name, value) ptls_log__do_push_element_bool(PTLS_LOG__ELEMENT_PREFIX(PTLS_TO_STR(name)), (value))
#define PTLS_LOG_APPDATA_ELEMENT_UNSAFESTR(name, value, value_len)                                                                 \
    do {                                                                                                                           \
        if (ptlslog_include_appdata) {                                                                                             \
            PTLS_LOG_ELEMENT_UNSAFESTR(name, value, value_len);                                                                    \
        } else {                                                                                                                   \
            PTLS_LOG__DO_ELEMENT_UNSIGNED(PTLS_TO_STR(name) "_len", value_len);                                                    \
        }                                                                                                                          \
    } while (0)
#define PTLS_LOG_APPDATA_ELEMENT_HEXDUMP(name, value, value_len)                                                                   \
    do {                                                                                                                           \
        if (ptlslog_include_appdata) {                                                                                             \
            PTLS_LOG_ELEMENT_HEXDUMP(name, value, value_len);                                                                      \
        } else {                                                                                                                   \
            PTLS_LOG__DO_ELEMENT_UNSIGNED(PTLS_TO_STR(name) "_len", value_len);                                                    \
        }                                                                                                                          \
    } while (0)

/**
 * retains a list of connections that are bound to the object
 */
struct st_ptls_log_state_t {
    /**
     * bit array of connections (1 is active)
     */
    uint32_t active_conns;
    /**
     * generation counter used for staleness check; see `ptls_log._generation`
     */
    uint64_t generation;
};

/**
 * represents a log point identified by name (`module:type`)
 */
struct st_ptls_log_point_t {
    const char *name;
    struct st_ptls_log_state_t state;
};

/**
 * represents a logging state of each connection
 */
typedef struct st_ptls_log_conn_state_t {
    /**
     * random value between 0 (inclusive) and 1 (non-inclusive) used to determine the ratio of sampling-based logging; see
     * `ptls_add_fd'`. To disable logging entirely, use `ptls_log.dummy_conn_state`, or set the value exactly to 1.
     */
    float random_;
    /**
     * represents peer address; ipv4 addresses are stored using the mapped form (::ffff:192.0.2.1)
     */
    struct in6_addr address;
    struct st_ptls_log_state_t state;
} ptls_log_conn_state_t;

/**
 * see `ptls_get_log_state`
 */
extern PTLS_THREADLOCAL ptls_log_conn_state_t *ptls_log_conn_state_override;

/**
 * global variables exposed
 */
extern struct st_ptls_log_t {
    /**
     * if application-data (e.g., payload) should be emitted as well
     */
    volatile unsigned may_include_appdata : 1;
    /**
     * endpoints that want to disable logging entirely can provide this value to the loggers
     */
    ptls_log_conn_state_t dummy_conn_state;
    /**
     * generation counter that is incremented whenever the state of loggers change; see `st_ptls_log_state_t::generation`
     */
    volatile uint64_t _generation;
} ptls_log;

/**
 * initializes a ptls_log_conn_state_t
 */
void ptls_log_init_conn_state(ptls_log_conn_state_t *state, void (*random_bytes)(void *, size_t));
/**
 * forces recalculation of the log state (should be called when SNI is determined)
 */
static void ptls_log_recalc_conn_state(ptls_log_conn_state_t *state);
/**
 * returns a bitmap indicating the loggers active for given log point
 */
static uint32_t ptls_log_point_maybe_active(struct st_ptls_log_point_t *point);
/**
 * returns a bitmap indicating the loggers active for given connection
 */
static uint32_t ptls_log_conn_maybe_active(ptls_log_conn_state_t *conn, const char *(*get_sni)(void *), void *get_sni_arg);

/**
 * Returns the number of log events that were unable to be emitted.
 */
size_t ptls_log_num_lost(void);
/**
 * Registers an fd to the logger. A registered fd is automatically closed and removed when it is closed by the peer.
 * @param sample_ratio  sampling ratio between 0 and 1
 * @param points        list of points to log, in the form of p1\0p2\0\0 (i.e., concatenated list of C strings with an empty string
 *                      marking the end). An empty list means attach to all.
 * @param snis          list of SNIs to log, using the same form as points
 * @param addresses     list of IPv4/v6 addresses to log, using the same form as points
 */
int ptls_log_add_fd(int fd, float sample_ratio, const char *points, const char *snis, const char *addresses, int appdata);

void ptls_log__recalc_point(int caller_locked, struct st_ptls_log_point_t *point);
void ptls_log__recalc_conn(int caller_locked, struct st_ptls_log_conn_state_t *conn, const char *(*get_sni)(void *),
                           void *get_sni_arg);
void ptls_log__do_push_element_safestr(const char *prefix, size_t prefix_len, const char *s, size_t l);
void ptls_log__do_push_element_unsafestr(const char *prefix, size_t prefix_len, const char *s, size_t l);
void ptls_log__do_push_element_hexdump(const char *prefix, size_t prefix_len, const void *s, size_t l);
void ptls_log__do_push_element_signed32(const char *prefix, size_t prefix_len, int32_t v);
void ptls_log__do_push_element_signed64(const char *prefix, size_t prefix_len, int64_t v);
void ptls_log__do_push_element_unsigned32(const char *prefix, size_t prefix_len, uint32_t v);
void ptls_log__do_push_element_unsigned64(const char *prefix, size_t prefix_len, uint64_t v);
void ptls_log__do_push_element_bool(const char *prefix, size_t prefix_len, int v);
void ptls_log__do_push_appdata_element_unsafestr(int includes_appdata, const char *prefix, size_t prefix_len, const char *s,
                                                 size_t l);
void ptls_log__do_push_appdata_element_hexdump(int includes_appdata, const char *prefix, size_t prefix_len, const void *s,
                                               size_t l);
void ptls_log__do_write_start(struct st_ptls_log_point_t *point, int add_time);
int ptls_log__do_write_end(struct st_ptls_log_point_t *point, struct st_ptls_log_conn_state_t *conn, const char *(*get_sni)(void *),
                           void *get_sni_arg, int includes_appdata);

/**
 * create a client object to handle new TLS connection
 */
ptls_t *ptls_client_new(ptls_context_t *ctx);
/**
 * create a server object to handle new TLS connection
 */
ptls_t *ptls_server_new(ptls_context_t *ctx);
/**
 * creates an object handle new TLS connection
 */
static ptls_t *ptls_new(ptls_context_t *ctx, int is_server);
/**
 * creates TLS 1.2 record layer for post-handshake communication
 */
int ptls_build_tls12_export_params(ptls_context_t *ctx, ptls_buffer_t *output, int is_server, int session_reused,
                                   ptls_cipher_suite_t *cipher, const void *master_secret, const void *hello_randoms,
                                   uint64_t next_send_record_iv, const char *server_name, ptls_iovec_t negotiated_protocol);
/**
 * store the parameters of a post-handshake TLS connection so that it can be reconstructed later
 */
int ptls_export(ptls_t *tls, ptls_buffer_t *output);
/**
 * create a post-handshake TLS connection object using given parameters
 */
int ptls_import(ptls_context_t *ctx, ptls_t **tls, ptls_iovec_t params);
/**
 * releases all resources associated to the object
 */
void ptls_free(ptls_t *tls);
/**
 * returns address of the crypto callbacks that the connection is using
 */
ptls_context_t *ptls_get_context(ptls_t *tls);
/**
 * updates the context of a connection. Can be called from `on_client_hello` callback.
 */
void ptls_set_context(ptls_t *tls, ptls_context_t *ctx);
/**
 * get the signature context
 */
ptls_async_job_t *ptls_get_async_job(ptls_t *tls);
/**
 * returns the client-random
 */
ptls_iovec_t ptls_get_client_random(ptls_t *tls);
/**
 * returns the cipher-suite being used
 */
ptls_cipher_suite_t *ptls_get_cipher(ptls_t *tls);
/**
 * returns a supported cipher-suite given an id
 */
ptls_cipher_suite_t *ptls_find_cipher_suite(ptls_cipher_suite_t **cipher_suites, uint16_t id);
/**
 * Returns protocol version (e.g., 0x0303 for TLS 1.2, 0x0304 for TLS 1.3). The result may be unstable prior to handshake
 * completion.
 */
uint16_t ptls_get_protocol_version(ptls_t *tls);
/**
 * Returns current state of traffic keys. The cipher-suite being used, as well as the length of the traffic keys, can be obtained
 * via `ptls_get_cipher`.
 * TODO: Even in case of offloading just the TX side, there should be API for handling key updates, sending Close aleart.
 */
int ptls_get_traffic_keys(ptls_t *tls, int is_enc, uint8_t *key, uint8_t *iv, uint64_t *seq);
/**
 * returns the server-name (NULL if SNI is not used or failed to negotiate)
 */
const char *ptls_get_server_name(ptls_t *tls);
/**
 * sets the server-name associated to the TLS connection. If server_name_len is zero, then strlen(server_name) is called to
 * determine the length of the name.
 * On the client-side, the value is used for certificate validation. The value will be also sent as an SNI extension, if it looks
 * like a DNS name.
 * On the server-side, it can be called from on_client_hello to indicate the acceptance of the SNI extension to the client.
 */
int ptls_set_server_name(ptls_t *tls, const char *server_name, size_t server_name_len);
/**
 * returns the negotiated protocol (or NULL)
 */
const char *ptls_get_negotiated_protocol(ptls_t *tls);
/**
 * sets the negotiated protocol. If protocol_len is zero, strlen(protocol) is called to determine the length of the protocol name.
 */
int ptls_set_negotiated_protocol(ptls_t *tls, const char *protocol, size_t protocol_len);
/**
 * returns if the handshake has been completed
 */
int ptls_handshake_is_complete(ptls_t *tls);
/**
 * returns if a PSK (or PSK-DHE) handshake was performed
 */
int ptls_is_psk_handshake(ptls_t *tls);
/**
 * return if a ECH handshake was performed, as well as optionally the kem and cipher-suite being used
 * FIXME: this function always return false when the TLS session is exported and imported
 */
int ptls_is_ech_handshake(ptls_t *tls, uint8_t *config_id, ptls_hpke_kem_t **kem, ptls_hpke_cipher_suite_t **cipher);
/**
 * returns a pointer to user data pointer (client is reponsible for freeing the associated data prior to calling ptls_free)
 */
void **ptls_get_data_ptr(ptls_t *tls);
/**
 * Returns `ptls_log_conn_state_t` of `ptls_t`. By default, the state is initialized by calling `ptls_log_init_conn_state`, but the
 * behavior can be overidden by setting `ptls_log_conn_state_override`.
 * This value can be changed by setting `ptls_log_random_override` or by calling `ptls_set_log_random`.
 */
ptls_log_conn_state_t *ptls_get_log_state(ptls_t *tls);
/**
 * proceeds with the handshake, optionally taking some input from peer. The function returns zero in case the handshake completed
 * successfully. PTLS_ERROR_IN_PROGRESS is returned in case the handshake is incomplete. Otherwise, an error value is returned. The
 * contents of sendbuf should be sent to the client, regardless of whether if an error is returned. inlen is an argument used for
 * both input and output. As an input, the arguments takes the size of the data available as input. Upon return the value is updated
 * to the number of bytes consumed by the handshake. In case the returned value is PTLS_ERROR_IN_PROGRESS there is a guarantee that
 * all the input are consumed (i.e. the value of inlen does not change).
 */
int ptls_handshake(ptls_t *tls, ptls_buffer_t *sendbuf, const void *input, size_t *inlen, ptls_handshake_properties_t *args);
/**
 * decrypts the first record within given buffer
 */
int ptls_receive(ptls_t *tls, ptls_buffer_t *plaintextbuf, const void *input, size_t *len);
/**
 * encrypts given buffer into multiple TLS records
 */
int ptls_send(ptls_t *tls, ptls_buffer_t *sendbuf, const void *input, size_t inlen);
/**
 * updates the send traffic key (as well as asks the peer to update)
 */
int ptls_update_key(ptls_t *tls, int request_update);
/**
 * Returns if the context is a server context.
 */
int ptls_is_server(ptls_t *tls);
/**
 * returns per-record overhead
 */
size_t ptls_get_record_overhead(ptls_t *tls);
/**
 * sends an alert
 */
int ptls_send_alert(ptls_t *tls, ptls_buffer_t *sendbuf, uint8_t level, uint8_t description);
/**
 *
 */
int ptls_export_secret(ptls_t *tls, void *output, size_t outlen, const char *label, ptls_iovec_t context_value, int is_early);
/**
 * build the body of a Certificate message. Can be called with tls set to NULL in order to create a precompressed message.
 */
int ptls_build_certificate_message(ptls_buffer_t *buf, ptls_iovec_t request_context, ptls_iovec_t *certificates,
                                   size_t num_certificates, ptls_iovec_t ocsp_status);
/**
 *
 */
int ptls_calc_hash(ptls_hash_algorithm_t *algo, void *output, const void *src, size_t len);
/**
 *
 */
ptls_hash_context_t *ptls_hmac_create(ptls_hash_algorithm_t *algo, const void *key, size_t key_size);
/**
 *
 */
int ptls_hkdf_extract(ptls_hash_algorithm_t *hash, void *output, ptls_iovec_t salt, ptls_iovec_t ikm);
/**
 *
 */
int ptls_hkdf_expand(ptls_hash_algorithm_t *hash, void *output, size_t outlen, ptls_iovec_t prk, ptls_iovec_t info);
/**
 *
 */
int ptls_hkdf_expand_label(ptls_hash_algorithm_t *algo, void *output, size_t outlen, ptls_iovec_t secret, const char *label,
                           ptls_iovec_t hash_value, const char *label_prefix);
/**
 * The expansion function of TLS 1.2 defined in RFC 5426 section 5. When `label` is NULL, acts as P_<hash>, or if non-NULL, as PRF.
 */
int ptls_tls12_phash(ptls_hash_algorithm_t *algo, void *output, size_t outlen, ptls_iovec_t secret, const char *label,
                     ptls_iovec_t seed);
/**
 * instantiates a symmetric cipher
 */
ptls_cipher_context_t *ptls_cipher_new(ptls_cipher_algorithm_t *algo, int is_enc, const void *key);
/**
 * destroys a symmetric cipher
 */
void ptls_cipher_free(ptls_cipher_context_t *ctx);
/**
 * initializes the IV; this function must be called prior to calling ptls_cipher_encrypt
 */
static void ptls_cipher_init(ptls_cipher_context_t *ctx, const void *iv);
/**
 * Encrypts given text. The function must be used in a way that the output length would be equal to the input length. For example,
 * when using a block cipher in ECB mode, `len` must be a multiple of the block size when using a block cipher. The length can be
 * of any value when using a stream cipher or a block cipher in CTR mode.
 */
static void ptls_cipher_encrypt(ptls_cipher_context_t *ctx, void *output, const void *input, size_t len);
/**
 * instantiates an AEAD cipher given a secret, which is expanded using hkdf to a set of key and iv
 * @param aead
 * @param hash
 * @param is_enc 1 if creating a context for encryption, 0 if creating a context for decryption
 * @param secret the secret. The size must be the digest length of the hash algorithm
 * @return pointer to an AEAD context if successful, otherwise NULL
 */
ptls_aead_context_t *ptls_aead_new(ptls_aead_algorithm_t *aead, ptls_hash_algorithm_t *hash, int is_enc, const void *secret,
                                   const char *label_prefix);
/**
 * instantiates an AEAD cipher given key and iv
 * @param aead
 * @param is_enc 1 if creating a context for encryption, 0 if creating a context for decryption
 * @return pointer to an AEAD context if successful, otherwise NULL
 */
ptls_aead_context_t *ptls_aead_new_direct(ptls_aead_algorithm_t *aead, int is_enc, const void *key, const void *iv);
/**
 * destroys an AEAD cipher context
 */
void ptls_aead_free(ptls_aead_context_t *ctx);
/**
 * Permutes the static IV by applying given bytes using bit-wise XOR. This API can be used for supplying nonces longer than 64-
 * bits.
 */
void ptls_aead_xor_iv(ptls_aead_context_t *ctx, const void *bytes, size_t len);
static void ptls_aead_get_iv(ptls_aead_context_t *ctx, void *iv);
static void ptls_aead_set_iv(ptls_aead_context_t *ctx, const void *iv);
/**
 * Encrypts one AEAD block, given input and output vectors.
 */
static size_t ptls_aead_encrypt(ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen, uint64_t seq,
                                const void *aad, size_t aadlen);
/**
 * Encrypts one AEAD block, as well as one block of ECB (for QUIC / DTLS packet number encryption). Depending on the AEAD engine
 * being used, the two operations might run simultaneously.
 */
static void ptls_aead_encrypt_s(ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen, uint64_t seq,
                                const void *aad, size_t aadlen, ptls_aead_supplementary_encryption_t *supp);
/**
 * Encrypts one AEAD block, given a vector of vectors.
 */
static void ptls_aead_encrypt_v(ptls_aead_context_t *ctx, void *output, ptls_iovec_t *input, size_t incnt, uint64_t seq,
                                const void *aad, size_t aadlen);
/**
 * Obsolete; new applications should use one of: `ptls_aead_encrypt`, `ptls_aead_encrypt_s`, `ptls_aead_encrypt_v`.
 */
static void ptls_aead_encrypt_init(ptls_aead_context_t *ctx, uint64_t seq, const void *aad, size_t aadlen);
/**
 * Obsolete; see `ptls_aead_encrypt_init`.
 */
static size_t ptls_aead_encrypt_update(ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen);
/**
 * Obsolete; see `ptls_aead_encrypt_init`.
 */
static size_t ptls_aead_encrypt_final(ptls_aead_context_t *ctx, void *output);
/**
 * decrypts an AEAD record
 * @return number of bytes emitted to output if successful, or SIZE_MAX if the input is invalid (e.g. broken MAC)
 */
static size_t ptls_aead_decrypt(ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen, uint64_t seq,
                                const void *aad, size_t aadlen);
/**
 * Return the current read epoch (i.e., that of the message being received or to be)
 */
size_t ptls_get_read_epoch(ptls_t *tls);
/**
 * Runs the handshake by dealing directly with handshake messages. Callers MUST delay supplying input to this function until the
 * epoch of the input becomes equal to the value returned by `ptls_get_read_epoch()`.
 * @param tls            the TLS context
 * @param sendbuf        buffer to which the output will be written
 * @param epoch_offsets  start and end offset of the messages in each epoch. For example, when the server emits ServerHello between
 *                       offset 0 and 38, the following handshake messages between offset 39 and 348, and a post-handshake message
 *                       between 349 and 451, epoch_offsets will be {0,39,39,349,452} and the length of the sendbuf will be 452.
 *                       This argument is an I/O argument. Applications can either reset sendbuf to empty and epoch_offsets and to
 *                       all zero every time they invoke the function, or retain the values until the handshake completes so that
 *                       data will be appended to sendbuf and epoch_offsets will be adjusted.
 * @param in_epoch       epoch of the input
 * @param input          input bytes (must be NULL when starting the handshake on the client side)
 * @param inlen          length of the input
 * @param properties     properties specific to the running handshake
 * @return same as `ptls_handshake`
 */
int ptls_handle_message(ptls_t *tls, ptls_buffer_t *sendbuf, size_t epoch_offsets[5], size_t in_epoch, const void *input,
                        size_t inlen, ptls_handshake_properties_t *properties);
int ptls_client_handle_message(ptls_t *tls, ptls_buffer_t *sendbuf, size_t epoch_offsets[5], size_t in_epoch, const void *input,
                               size_t inlen, ptls_handshake_properties_t *properties);
int ptls_server_handle_message(ptls_t *tls, ptls_buffer_t *sendbuf, size_t epoch_offsets[5], size_t in_epoch, const void *input,
                               size_t inlen, ptls_handshake_properties_t *properties);
/**
 * internal
 */
void ptls_aead__build_iv(ptls_aead_algorithm_t *algo, uint8_t *iv, const uint8_t *static_iv, uint64_t seq);
/**
 *
 */
static void ptls_aead__do_encrypt(ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen, uint64_t seq,
                                  const void *aad, size_t aadlen, ptls_aead_supplementary_encryption_t *supp);
/**
 *
 */
static void ptls_aead__do_encrypt_v(ptls_aead_context_t *ctx, void *_output, ptls_iovec_t *input, size_t incnt, uint64_t seq,
                                    const void *aad, size_t aadlen);
/**
 * internal
 */
void ptls__key_schedule_update_hash(ptls_key_schedule_t *sched, const uint8_t *msg, size_t msglen, int use_outer);
/**
 * clears memory
 */
extern void (*volatile ptls_clear_memory)(void *p, size_t len);
/**
 * constant-time memcmp
 */
extern int (*volatile ptls_mem_equal)(const void *x, const void *y, size_t len);
/**
 * checks if a server name is an IP address.
 */
int ptls_server_name_is_ipaddr(const char *name);
/**
 * encodes one ECH Config
 */
int ptls_ech_encode_config(ptls_buffer_t *buf, uint8_t config_id, ptls_hpke_kem_t *kem, ptls_iovec_t public_key,
                           ptls_hpke_cipher_suite_t **ciphers, uint8_t max_name_length, const char *public_name);
/**
 * loads a certificate chain to ptls_context_t::certificates. `certificate.list` and each element of the list is allocated by
 * malloc.  It is the responsibility of the user to free them when discarding the TLS context.
 */
int ptls_load_certificates(ptls_context_t *ctx, char const *cert_pem_file);
/**
 * SetupBaseS function of RFC 9180. Given `kem`, `algo`, `info`, and receiver's public key, returns an ephemeral public key and an
 * AEAD context used for encrypting data.
 */
int ptls_hpke_setup_base_s(ptls_hpke_kem_t *kem, ptls_hpke_cipher_suite_t *cipher, ptls_iovec_t *pk_s, ptls_aead_context_t **ctx,
                           ptls_iovec_t pk_r, ptls_iovec_t info);
/**
 * SetupBaseR function of RFC 9180. Given `kem`, `algo`, `info`, receiver's private key (`keyex`), and the esnder's public key,
 * returns the AEAD context to be used for decrypting data.
 */
int ptls_hpke_setup_base_r(ptls_hpke_kem_t *kem, ptls_hpke_cipher_suite_t *cipher, ptls_key_exchange_context_t *keyex,
                           ptls_aead_context_t **ctx, ptls_iovec_t pk_s, ptls_iovec_t info);
/**
 *
 */
char *ptls_hexdump(char *dst, const void *src, size_t len);
/**
 * Builds a JSON-safe string without double quotes. Supplied buffer MUST be at least 6x + 1 bytes larger than the input.
 */
char *ptls_jsonescape(char *buf, const char *s, size_t len);
/**
 * builds a v4-mapped address (i.e., ::ffff:192.0.2.1)
 */
void ptls_build_v4_mapped_v6_address(struct in6_addr *v6, const struct in_addr *v4);

/**
 * the default get_time callback
 */
extern ptls_get_time_t ptls_get_time;
/**
 * default hash clone function that calls memcpy
 */
static void ptls_hash_clone_memcpy(void *dst, const void *src, size_t size);

/* inline functions */

inline uint32_t ptls_log_point_maybe_active(struct st_ptls_log_point_t *point)
{
#if PTLS_HAVE_LOG
    if (PTLS_UNLIKELY(point->state.generation != ptls_log._generation))
        ptls_log__recalc_point(0, point);
    return point->state.active_conns;
#else
    return 0;
#endif
}

inline void ptls_log_recalc_conn_state(ptls_log_conn_state_t *state)
{
    state->state.generation = 0;
}

inline uint32_t ptls_log_conn_maybe_active(ptls_log_conn_state_t *conn, const char *(*get_sni)(void *), void *get_sni_arg)
{
#if PTLS_HAVE_LOG
    if (PTLS_UNLIKELY(conn->state.generation != ptls_log._generation))
        ptls_log__recalc_conn(0, conn, get_sni, get_sni_arg);
    return conn->state.active_conns;
#else
    return 0;
#endif
}

inline ptls_t *ptls_new(ptls_context_t *ctx, int is_server)
{
    return is_server ? ptls_server_new(ctx) : ptls_client_new(ctx);
}

inline ptls_iovec_t ptls_iovec_init(const void *p, size_t len)
{
    /* avoid the "return (ptls_iovec_t){(uint8_t *)p, len};" construct because it requires C99
     * and triggers a warning "C4204: nonstandard extension used: non-constant aggregate initializer"
     * in Visual Studio */
    ptls_iovec_t r;
    r.base = (uint8_t *)p;
    r.len = len;
    return r;
}

inline void ptls_buffer_init(ptls_buffer_t *buf, void *smallbuf, size_t smallbuf_size)
{
    assert(smallbuf != NULL);
    buf->base = (uint8_t *)smallbuf;
    buf->off = 0;
    buf->capacity = smallbuf_size;
    buf->is_allocated = 0;
    buf->align_bits = 0;
}

inline void ptls_buffer_dispose(ptls_buffer_t *buf)
{
    ptls_buffer__release_memory(buf);
    *buf = (ptls_buffer_t){NULL, 0, 0, 0, 0};
}

inline uint8_t *ptls_encode_quicint(uint8_t *p, uint64_t v)
{
    if (PTLS_UNLIKELY(v > 63)) {
        if (PTLS_UNLIKELY(v > 16383)) {
            unsigned sb;
            if (PTLS_UNLIKELY(v > 1073741823)) {
                assert(v <= 4611686018427387903);
                *p++ = 0xc0 | (uint8_t)(v >> 56);
                sb = 6 * 8;
            } else {
                *p++ = 0x80 | (uint8_t)(v >> 24);
                sb = 2 * 8;
            }
            do {
                *p++ = (uint8_t)(v >> sb);
            } while ((sb -= 8) != 0);
        } else {
            *p++ = 0x40 | (uint8_t)((uint16_t)v >> 8);
        }
    }
    *p++ = (uint8_t)v;
    return p;
}

inline void ptls_cipher_init(ptls_cipher_context_t *ctx, const void *iv)
{
    ctx->do_init(ctx, iv);
}

inline void ptls_cipher_encrypt(ptls_cipher_context_t *ctx, void *output, const void *input, size_t len)
{
    ctx->do_transform(ctx, output, input, len);
}

inline void ptls_aead_get_iv(ptls_aead_context_t *ctx, void *iv)
{
    ctx->do_get_iv(ctx, iv);
}

inline void ptls_aead_set_iv(ptls_aead_context_t *ctx, const void *iv)
{
    ctx->do_set_iv(ctx, iv);
}

inline size_t ptls_aead_encrypt(ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen, uint64_t seq,
                                const void *aad, size_t aadlen)
{
    ctx->do_encrypt(ctx, output, input, inlen, seq, aad, aadlen, NULL);
    return inlen + ctx->algo->tag_size;
}

inline void ptls_aead_encrypt_s(ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen, uint64_t seq,
                                const void *aad, size_t aadlen, ptls_aead_supplementary_encryption_t *supp)
{
    ctx->do_encrypt(ctx, output, input, inlen, seq, aad, aadlen, supp);
}

inline void ptls_aead_encrypt_v(ptls_aead_context_t *ctx, void *output, ptls_iovec_t *input, size_t incnt, uint64_t seq,
                                const void *aad, size_t aadlen)
{
    ctx->do_encrypt_v(ctx, output, input, incnt, seq, aad, aadlen);
}

inline void ptls_aead_encrypt_init(ptls_aead_context_t *ctx, uint64_t seq, const void *aad, size_t aadlen)
{
    ctx->do_encrypt_init(ctx, seq, aad, aadlen);
}

inline size_t ptls_aead_encrypt_update(ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen)
{
    return ctx->do_encrypt_update(ctx, output, input, inlen);
}

inline size_t ptls_aead_encrypt_final(ptls_aead_context_t *ctx, void *output)
{
    return ctx->do_encrypt_final(ctx, output);
}

inline void ptls_aead__do_encrypt(ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen, uint64_t seq,
                                  const void *aad, size_t aadlen, ptls_aead_supplementary_encryption_t *supp)
{
    ptls_iovec_t invec = ptls_iovec_init(input, inlen);
    ctx->do_encrypt_v(ctx, output, &invec, 1, seq, aad, aadlen);

    if (supp != NULL) {
        ptls_cipher_init(supp->ctx, supp->input);
        memset(supp->output, 0, sizeof(supp->output));
        ptls_cipher_encrypt(supp->ctx, supp->output, supp->output, sizeof(supp->output));
    }
}

inline void ptls_aead__do_encrypt_v(ptls_aead_context_t *ctx, void *_output, ptls_iovec_t *input, size_t incnt, uint64_t seq,
                                    const void *aad, size_t aadlen)
{
    uint8_t *output = (uint8_t *)_output;

    ctx->do_encrypt_init(ctx, seq, aad, aadlen);
    for (size_t i = 0; i < incnt; ++i)
        output += ctx->do_encrypt_update(ctx, output, input[i].base, input[i].len);
    ctx->do_encrypt_final(ctx, output);
}

inline size_t ptls_aead_decrypt(ptls_aead_context_t *ctx, void *output, const void *input, size_t inlen, uint64_t seq,
                                const void *aad, size_t aadlen)
{
    return ctx->do_decrypt(ctx, output, input, inlen, seq, aad, aadlen);
}

inline void ptls_hash_clone_memcpy(void *dst, const void *src, size_t size)
{
    memcpy(dst, src, size);
}

#define ptls_define_hash(name, ctx_type, init_func, update_func, final_func)                                                       \
    ptls_define_hash6(name, ctx_type, init_func, update_func, final_func, ptls_hash_clone_memcpy)
#define ptls_define_hash6(name, ctx_type, init_func, update_func, final_func, clone_func)                                          \
                                                                                                                                   \
    struct name##_context_t {                                                                                                      \
        ptls_hash_context_t super;                                                                                                 \
        ctx_type ctx;                                                                                                              \
    };                                                                                                                             \
                                                                                                                                   \
    static void name##_update(ptls_hash_context_t *_ctx, const void *src, size_t len)                                              \
    {                                                                                                                              \
        struct name##_context_t *ctx = (struct name##_context_t *)_ctx;                                                            \
        update_func(&ctx->ctx, src, len);                                                                                          \
    }                                                                                                                              \
                                                                                                                                   \
    static void name##_final(ptls_hash_context_t *_ctx, void *md, ptls_hash_final_mode_t mode)                                     \
    {                                                                                                                              \
        struct name##_context_t *ctx = (struct name##_context_t *)_ctx;                                                            \
        if (mode == PTLS_HASH_FINAL_MODE_SNAPSHOT) {                                                                               \
            ctx_type copy = ctx->ctx;                                                                                              \
            final_func(&copy, md);                                                                                                 \
            ptls_clear_memory(&copy, sizeof(copy));                                                                                \
            return;                                                                                                                \
        }                                                                                                                          \
        if (md != NULL)                                                                                                            \
            final_func(&ctx->ctx, md);                                                                                             \
        switch (mode) {                                                                                                            \
        case PTLS_HASH_FINAL_MODE_FREE:                                                                                            \
            ptls_clear_memory(&ctx->ctx, sizeof(ctx->ctx));                                                                        \
            free(ctx);                                                                                                             \
            break;                                                                                                                 \
        case PTLS_HASH_FINAL_MODE_RESET:                                                                                           \
            init_func(&ctx->ctx);                                                                                                  \
            break;                                                                                                                 \
        default:                                                                                                                   \
            assert(!"FIXME");                                                                                                      \
            break;                                                                                                                 \
        }                                                                                                                          \
    }                                                                                                                              \
                                                                                                                                   \
    static ptls_hash_context_t *name##_clone(ptls_hash_context_t *_src)                                                            \
    {                                                                                                                              \
        struct name##_context_t *dst, *src = (struct name##_context_t *)_src;                                                      \
        if ((dst = malloc(sizeof(*dst))) == NULL)                                                                                  \
            return NULL;                                                                                                           \
        dst->super = src->super;                                                                                                   \
        clone_func(&dst->ctx, &src->ctx, sizeof(dst->ctx));                                                                        \
        return &dst->super;                                                                                                        \
    }                                                                                                                              \
                                                                                                                                   \
    static ptls_hash_context_t *name##_create(void)                                                                                \
    {                                                                                                                              \
        struct name##_context_t *ctx;                                                                                              \
        if ((ctx = malloc(sizeof(*ctx))) == NULL)                                                                                  \
            return NULL;                                                                                                           \
        ctx->super = (ptls_hash_context_t){name##_update, name##_final, name##_clone};                                             \
        init_func(&ctx->ctx);                                                                                                      \
        return &ctx->super;                                                                                                        \
    }

#ifdef __cplusplus
}
#endif

#endif
