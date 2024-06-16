#ifndef __CRYPTO_AES__
#define __CRYPTO_AES__

#include <stddef.h>
#include <stdint.h>

#define AES_NUM_BITS_PER_BYTE   8

#define AES_NUM_BYTES_PER_WORD  4

#define AES_NUM_BITS_PER_WORD   \
    (AES_NUM_BITS_PER_BYTE *    \
     AES_NUM_BYTES_PER_WORD)

#define AES_128_KEY_NUM_WORDS   4
#define AES_192_KEY_NUM_WORDS   6
#define AES_256_KEY_NUM_WORDS   8

#define AES_128_NUM_ROUNDS      10
#define AES_192_NUM_ROUNDS      12
#define AES_256_NUM_ROUNDS      14

#define AES_128_KEY_EXP_NUM_WORDS   \
    (4 * (AES_128_NUM_ROUNDS + 1))
#define AES_192_KEY_EXP_NUM_WORDS   \
    (4 * (AES_192_NUM_ROUNDS + 1))
#define AES_256_KEY_EXP_NUM_WORDS   \
    (4 * (AES_256_NUM_ROUNDS + 1))

#define AES_128_NUM_BITS        \
    (AES_NUM_BYTES_PER_WORD *   \
     AES_128_KEY_NUM_WORDS)
#define AES_192_NUM_BITS        \
    (AES_NUM_BYTES_PER_WORD *   \
     AES_192_KEY_NUM_WORDS)
#define AES_256_NUM_BITS        \
    (AES_NUM_BYTES_PER_WORD *   \
     AES_256_KEY_NUM_WORDS)

#define AES_128 128
#define AES_192 192
#define AES_256 256

typedef uint32_t aes_word;

typedef uint8_t aes_state[4][4];

typedef struct _aes_cbc_ctx {
    size_t num_words;
    size_t num_rounds;
    uint8_t init_vec[16];
    uint8_t tmp_vec[16];
    aes_word expanded_keys[AES_256_KEY_EXP_NUM_WORDS];
} aes_cbc_ctx;

typedef enum _aes_res {
    AES_OK = 0,

    AES_ERR = -128,
    AES_BAD_KEY_BITS,
    AES_BAD_DATA_LEN,
} aes_res;

aes_res
aes_cbc_ctx_init(
    aes_cbc_ctx * ctx,
    const uint8_t * init_vec,
    const uint8_t * key_buf,
    size_t key_bits 
);

aes_res
aes_cbc_encrypt(
    aes_cbc_ctx * ctx,
    void * out,
    const void * in,
    size_t len
);

void
aes_cbc_decrypt(
    aes_cbc_ctx * ctx,
    void * out,
    const void * in,
    size_t len
);

void
aes_cbc_ctx_free(
    aes_cbc_ctx * ctx
);

#endif
