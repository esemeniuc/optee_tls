//
// Created by eric on 09/05/19.
//

#include <mbedtls/entropy.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <tee_api.h>
#include <utee_syscalls.h>
#include <stdio.h>

size_t ceil4(size_t x){
    return ((x+3) / 4) * 4;
}

void *TEE_Calloc(size_t size) {
    size = ceil4(size);
    printf("CALLOC Called %lu\n", size);
    return TEE_Malloc(size, TEE_MALLOC_FILL_ZERO);
}

void TEE_Free2(void *ptr) {
    printf("FREE Called\n");
    TEE_Free(ptr);
}

/**
 * \brief           Entropy poll callback pointer
 *
 * \param data      Callback-specific data pointer (not used)
 * \param output    Data to fill
 * \param len       Maximum size to provide
 * \param olen      The actual amount of bytes put into the buffer (Can be 0)
 *
 * \return          0 if no critical failures occurred,
 *                  MBEDTLS_ERR_ENTROPY_SOURCE_FAILED otherwise
 */
int mbed_entropy_get_bytes(void *data, unsigned char *output,
                           size_t len, size_t *olen) {
    if (utee_cryp_random_number_generate(output, len) == TEE_SUCCESS) {
        *olen = len;
        return 0;
    }
    *olen = 0;
    return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
}

//from https://github.com/ARMmbed/mbedtls/blob/75d9a333cef532d55c01773add6dee1106da95c3/library/net_sockets.c#L139
void mbedtls_net_init(mbedtls_net_context *ctx) {
    ctx->fd = -1;
}

/*
 * Read at most 'len' characters, needed for mbedtls_ssl_set_bio()
 */
int mbedtls_net_recv(void *ctx, unsigned char *buf, size_t len) {
    int fd = ((mbedtls_net_context *) ctx)->fd;
    if (fd < 0)
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

    int bytes;
    if (TEE_SimpleRecvConnection(fd, buf, len, &bytes) != TEE_SUCCESS) {
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }
    return bytes;
}

/*
 * Write at most 'len' characters
 */
int mbedtls_net_send(void *ctx, const unsigned char *buf, size_t len) {
    int fd = ((mbedtls_net_context *) ctx)->fd;
    if (fd < 0)
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

    int bytes;
    if (TEE_SimpleSendConnection(fd, buf, len, &bytes) != TEE_SUCCESS) {
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }
    return bytes;
}