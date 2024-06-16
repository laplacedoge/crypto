#include <string.h>

#include "aes.h"

static
uint8_t
x_times(
    uint8_t data
) {
    uint8_t result = data << 1;

    if (data >> 7 != 0) {
        result ^= 0x1B;
    }

    return result;
}

static
uint8_t
multiply(
    uint8_t a,
    uint8_t b
) {
    uint8_t tmp;
    uint8_t result = 0;

    for (uint8_t i = 0; i < 8; i++) {
        if (i == 0) {
            tmp = a;
        } else {
            tmp = x_times(tmp);
        }

        if ((b >> i & 0x01) != 0) {
            result ^= tmp;
        }
    }

    return result;
}

uint8_t
s_box(
    uint8_t data
) {
    static uint8_t table[] = {
        0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
        0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
        0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
        0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
        0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
        0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
        0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
        0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
        0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
        0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
        0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
        0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
        0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
        0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
        0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
        0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16,
    };

    return table[data];
}

void
sub_bytes(
    aes_state * state
) {
    for (uint8_t i = 0; i < 16; i++) {
        uint8_t * byte = (uint8_t *)state + i;

        *byte = s_box(*byte);
    }
}

static
void
shift_rows(
    aes_state * state
) {
    uint8_t tmp;

    tmp = (*state)[1][0];
    (*state)[1][0] = (*state)[1][1];
    (*state)[1][1] = (*state)[1][2];
    (*state)[1][2] = (*state)[1][3];
    (*state)[1][3] = tmp;

    tmp = (*state)[2][0];
    (*state)[2][0] = (*state)[2][2];
    (*state)[2][2] = tmp;
    tmp = (*state)[2][1];
    (*state)[2][1] = (*state)[2][3];
    (*state)[2][3] = tmp;

    tmp = (*state)[3][3];
    (*state)[3][3] = (*state)[3][2];
    (*state)[3][2] = (*state)[3][1];
    (*state)[3][1] = (*state)[3][0];
    (*state)[3][0] = tmp;
}

static
void
mix_columns(
    aes_state * state
) {
    for (uint8_t c = 0; c < 4; c++) {
        uint8_t column[4];

        for (uint8_t r = 0; r < 4; r++) {
            column[r] = (*state)[r][c];
        }

        (*state)[0][c] = multiply(0x02, column[0]) ^
                         multiply(0x03, column[1]) ^
                         column[2] ^
                         column[3];

        (*state)[1][c] = column[0] ^
                         multiply(0x02, column[1]) ^
                         multiply(0x03, column[2]) ^
                         column[3];

        (*state)[2][c] = column[0] ^
                         column[1] ^
                         multiply(0x02, column[2]) ^
                         multiply(0x03, column[3]);

        (*state)[3][c] = multiply(0x03, column[0]) ^
                         column[1] ^
                         column[2] ^
                         multiply(0x02, column[3]);

    }
}

static
void
add_round_key(
    aes_state * state,
    aes_word * round_keys
) {
    for (uint8_t c = 0; c < 4; c++) {
        for (uint8_t r = 0; r < 4; r++) {
            (*state)[r][c] ^= ((uint8_t *)(round_keys + c))[r];
        }
    }
}

static
aes_word
rot_word(
    aes_word word
) {
    uint8_t * input = (uint8_t *)&word;
    uint8_t output[4];

    output[0] = input[1];
    output[1] = input[2];
    output[2] = input[3];
    output[3] = input[0];
    
    return *(aes_word *)output;
}

static
aes_word
sub_word(
    aes_word word
) {
    uint8_t * input = (uint8_t *)&word;
    uint8_t output[4];

    output[0] = s_box(input[0]);
    output[1] = s_box(input[1]);
    output[2] = s_box(input[2]);
    output[3] = s_box(input[3]);
    
    return *(aes_word *)output;
}

static
void
fill_state(
    aes_state * state,
    const void * data
) {
    for (uint8_t c = 0; c < 4; c++) {
        for (uint8_t r = 0; r < 4; r++) {
            (*state)[r][c] = *((uint8_t *)data + c * 4 + r);
        }
    }
}

static
void
extract_state(
    void * buf,
    const aes_state * state
) {
    for (uint8_t c = 0; c < 4; c++) {
        for (uint8_t r = 0; r < 4; r++) {
            *((uint8_t *)buf + c * 4 + r) = (*state)[r][c];
        }
    }
}

static
void
cipher(
    void * cipher_text,
    const void * plain_text,
    size_t num_rounds,
    aes_word * expanded_keys
) {
    aes_state state;

    fill_state(&state, plain_text);

    add_round_key(&state, expanded_keys);

    for (uint8_t r = 1; r < num_rounds; r++) {
        sub_bytes(&state);
        shift_rows(&state);
        mix_columns(&state);
        add_round_key(&state, expanded_keys + r * 4);
    }

    sub_bytes(&state);
    shift_rows(&state);
    add_round_key(&state, expanded_keys + num_rounds * 4);

    extract_state(cipher_text, &state);
}

static
void
expand_key(
    void * output,
    const void * input,
    size_t num_words,
    size_t num_rounds
) {
    const uint8_t round_consts[] = {
        0x01, 0x02, 0x04, 0x08, 0x10,
        0x20, 0x40, 0x80, 0x1B, 0x36,
    };

    memcpy(output, input, AES_NUM_BYTES_PER_WORD * num_words);

    uint8_t i = num_words;

    while (i <= 4 * num_rounds + 3) {
        uint32_t tmp = ((uint32_t *)output)[i - 1];

        if (i % num_words == 0) {
            tmp = sub_word(rot_word(tmp)) ^
                  round_consts[i / num_words - 1];
        } else if (num_words == AES_256_KEY_NUM_WORDS &&
                   i % num_words == 4) {
            tmp = sub_word(tmp);
        }

        ((uint32_t *)output)[i] =
            ((uint32_t *)output)[i - num_words] ^ tmp;

        i++;
    }
}

aes_res
aes_cbc_ctx_init(
    aes_cbc_ctx * ctx,
    const uint8_t * init_vec,
    const uint8_t * key_buf,
    size_t key_bits
) {
    if (key_bits != AES_128 &&
        key_bits != AES_192 &&
        key_bits != AES_256) {
        return AES_BAD_KEY_BITS;
    }

    /* Determine the number of words of the key. */
    size_t num_words;

    switch (key_bits) {
    case AES_128: num_words = AES_128_KEY_NUM_WORDS; break;
    case AES_192: num_words = AES_192_KEY_NUM_WORDS; break;
    case AES_256: num_words = AES_256_KEY_NUM_WORDS; break;
    }

    ctx->num_words = num_words;

    /* Determine the number of rounds. */
    size_t num_rounds;

    switch (key_bits) {
    case AES_128: num_rounds = AES_128_NUM_ROUNDS; break;
    case AES_192: num_rounds = AES_192_NUM_ROUNDS; break;
    case AES_256: num_rounds = AES_256_NUM_ROUNDS; break;
    }

    ctx->num_rounds = num_rounds;

    /* Copy initial vector. */
    memcpy(ctx->init_vec, init_vec, 16);

    /* Perform key expansion. */
    expand_key(ctx->expanded_keys, key_buf, num_words, num_rounds);

    return AES_OK;
}

static
void
cbc_xor_vec(
    void * src,
    const void * dest
) {
    for (uint8_t i = 0; i < 16; i++) {
        *((uint8_t *)src + i) ^= *((uint8_t *)dest + i);
    }
}

aes_res
aes_cbc_encrypt(
    aes_cbc_ctx * ctx,
    void * out,
    const void * in,
    size_t len
) {
    if (len % 16 != 0) {
        return AES_BAD_DATA_LEN;
    }

    /* Copy the initial vector to the temporary vector. */
    memcpy(ctx->tmp_vec, ctx->init_vec, 16);

    uint8_t * off_out = (uint8_t *)out;
    uint8_t * off_in = (uint8_t *)in;
    uint16_t tmp_buf[16];

    while (len != 0) {
        memcpy(tmp_buf, off_in, 16);

        cbc_xor_vec(tmp_buf, ctx->tmp_vec);

        cipher(off_out, tmp_buf, ctx->num_rounds, ctx->expanded_keys);

        memcpy(ctx->tmp_vec, off_out, 16);

        off_out += 16;
        off_in += 16;
        len -= 16;
    }

    return AES_OK;
}

void
aes_cbc_decrypt(
    aes_cbc_ctx * ctx,
    void * out,
    const void * in,
    size_t len
) {

}

void
aes_cbc_ctx_free(
    aes_cbc_ctx * ctx
) {

}
