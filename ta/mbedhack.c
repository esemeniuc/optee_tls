//
// Created by eric on 09/05/19.
//

#include <mbedtls/ssl.h>
#include <mbedtls/net_sockets.h>
#include <tee_api.h>

void *TEE_Calloc(size_t size) {
    return TEE_Malloc(size, TEE_MALLOC_FILL_ZERO);
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