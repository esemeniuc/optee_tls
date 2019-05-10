/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <hello_world_ta.h>
//#include <teec_trace.h>

#ifndef TRACE_ERROR
#define TRACE_ERROR  1
#endif

#ifndef TRACE_INFO
#define TRACE_INFO   2
#endif
#ifndef TRACE_DEBUG
#define TRACE_DEBUG  3
#endif

#ifndef TRACE_FLOW
#define TRACE_FLOW   4
#endif

#ifndef IMSG
#define IMSG(fmt, ...)   dprintf(TRACE_INFO, fmt "\n", ##__VA_ARGS__)
#endif
#ifndef DMSG
#define DMSG(fmt, ...)   dprintf(TRACE_DEBUG, fmt "\n", ##__VA_ARGS__)
#endif

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void) {
    DMSG("has been called");

    return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void) {
    DMSG("has been called");
}

#define EXIT_FAILURE 1

#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
//#include "mbedtls/platform_util.h"
#include "tee_tcpsocket.h"
#include <string.h>

#define stdout 0

static void my_debug(void *ctx, int level,
                     const char *file, int line,
                     const char *str) {
    ((void) level);
    ((void) ctx);

    printf("%s:%04d: %s", file, line, str);
}

void cleanup(mbedtls_net_context *server_fd, mbedtls_entropy_context *entropy, mbedtls_ctr_drbg_context *ctr_drbg,
             mbedtls_ssl_context *ssl, mbedtls_ssl_config *conf, mbedtls_x509_crt *cacert) {
    TEE_SimpleCloseConnection((*server_fd).fd);
    mbedtls_x509_crt_free(cacert);
//    mbedtls_ssl_free(ssl);
//    mbedtls_ssl_config_free(conf);
    mbedtls_ctr_drbg_free(ctr_drbg);
    mbedtls_entropy_free(entropy);
}

void demo(void) {

    int ret = 1, len;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    mbedtls_net_context server_fd;
    uint32_t flags;
    unsigned char buf[1024];
    const char *pers = "ssl_client1";
    char SERVER_NAME[] = "172.217.3.206";
    int SERVER_PORT = 80;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
    mbedtls_entropy_add_source(); //FIXME
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     (const unsigned char *) pers,
                                     strlen(pers))) != 0) {
        printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        cleanup(&server_fd, &entropy, &ctr_drbg, &ssl, &conf, &cacert);
        return;
    }
    ret = mbedtls_x509_crt_parse(&cacert, (const unsigned char *) mbedtls_test_cas_pem,
                                 mbedtls_test_cas_pem_len);
    if (ret < 0) {
        printf(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        cleanup(&server_fd, &entropy, &ctr_drbg, &ssl, &conf, &cacert);
        return;
    }

    //gets a fd
    if (TEE_SimpleOpenConnection(SERVER_NAME, SERVER_PORT, &(server_fd.fd)) != TEE_SUCCESS) {
        printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
        cleanup(&server_fd, &entropy, &ctr_drbg, &ssl, &conf, &cacert);
        return;
    }

    if ((ret = mbedtls_ssl_config_defaults(&conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        cleanup(&server_fd, &entropy, &ctr_drbg, &ssl, &conf, &cacert);
        return;
    }

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

    mbedtls_platform_set_calloc_free( TEE_Calloc, TEE_Free);
    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
        printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        cleanup(&server_fd, &entropy, &ctr_drbg, &ssl, &conf, &cacert);
        return;
    }
//
//    if ((ret = mbedtls_ssl_set_hostname(&ssl, SERVER_NAME)) != 0) {
//        printf(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
//        cleanup(&server_fd, &entropy, &ctr_drbg, &ssl, &conf, &cacert);
//        return;
//    }
//
//    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
//    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
//        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
//            printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
//            cleanup(&server_fd, &entropy, &ctr_drbg, &ssl, &conf, &cacert);
//            return;
//        }
//    }
//
//    /* In real life, we probably want to bail out when ret != 0 */
//    if ((flags = mbedtls_ssl_get_verify_result(&ssl)) != 0) {
//        char vrfy_buf[512];
//        mbedtls_printf(" failed\n");
//        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
//        mbedtls_printf("%s\n", vrfy_buf);
//    } else
//        mbedtls_printf(" ok\n");
//
//    /*
//     * 3. Write the GET request
//     */
//    char getReq[] = "GET / HTTP/1.0\r\n\r\n";
//    len = sizeof(getReq);
//
//    while ((ret = mbedtls_ssl_write(&ssl, (unsigned char *) getReq, len)) <= 0) {
//        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
//            mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
//            cleanup(&server_fd, &entropy, &ctr_drbg, &ssl, &conf, &cacert);
//            return;
//        }
//    }
//
//
//    do {
//        len = sizeof(buf) - 1;
//        memset(buf, 0, sizeof(buf));
//        ret = mbedtls_ssl_read(&ssl, buf, len);
//
//        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
//            continue;
//        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
//            break;
//        if (ret < 0) {
//            mbedtls_printf("failed\n  ! mbedtls_ssl_read returned %d\n\n", ret);
//            break;
//        }
//        if (ret == 0) {
//            mbedtls_printf("\n\nEOF\n\n");
//            break;
//        }
//        len = ret;
//        mbedtls_printf(" %d bytes read\n\n%s", len, (char *) buf);
//    } while (1);
//    int fd = 0;
//    DMSG("RUNNING");
//    TEE_Result res = TEE_SimpleOpenConnection(SERVER_NAME, SERVER_PORT, &fd);
//    if (res != TEE_SUCCESS) {
//        DMSG("FAIL OPEN");
//    }
//    int bytesSent, bytesRecv;
//
//    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
//
//    res = TEE_SimpleSendConnection(fd, getReq, sizeof(getReq), &bytesSent);
//    if (res != TEE_SUCCESS) {
//        DMSG("FAIL SEND");
//    }
//    char recvbuf[2000] = {};
//    res = TEE_SimpleRecvConnection(fd, recvbuf, sizeof(recvbuf), &bytesRecv);
//    if (res != TEE_SUCCESS) {
//        DMSG("FAIL RECV");
//    }
//    printf("got %d bytes\n", bytesRecv);
//    for (int i = 0; i < 2000; i++)
//        printf("%c", recvbuf[i]);
//    DMSG("%s\n\n", recvbuf);



//    TEE_tcpSocket_Setup tcpSetup = (TEE_tcpSocket_Setup) {
//            .ipVersion=TEE_IP_VERSION_4,
//            .server_addr="127.0.0.1",
//            .server_port=80,};
//    TEE_Result res;
//    TEE_iSocketHandle tcpCtx;
//    uint32_t protocolError;
//    DMSG("NOT OPENED");
//    res = TEE_tcpSocket->open(&tcpCtx, &tcpSetup, &protocolError);
//    if (res != TEE_SUCCESS) {
//        DMSG("FAIL OPEN %ud", res);
//    }
//    DMSG("OPENED");


}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, TEE_Param params[4], void **sess_ctx) {
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE);

    DMSG("has been called");

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

/* Unused parameters */
    (void) &params;
    (void) &sess_ctx;

/*
 * The DMSG() macro is non-standard, TEE Internal API doesn't
 * specify any means to logging from a TA.
 */
    IMSG("Hello World!\n");

/* If return value != TEE_SUCCESS the session will not be created. */
    return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void *sess_ctx) {
    (void) &sess_ctx; /* Unused parameter */
    IMSG("Goodbye!\n");
}


static TEE_Result inc_value(uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
                                               TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE);

    DMSG("has been called");
    DMSG("running demo");
    demo();
    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;


    TEE_iSocket *const TEE_tlsSocket = NULL;
    IMSG("%x", (unsigned int) TEE_tlsSocket);


    IMSG("Got value: %u from NW", params[0].value.a);
    IMSG("Got value: %u from NW", params[0].value.b);
    params[0].value.a++;
    IMSG("Increase value to: %u", params[0].value.a);
    IMSG("Increase value to: %u", params[0].value.b);

    return TEE_SUCCESS;
}

static TEE_Result dec_value(uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
                                               TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE);

    DMSG("has been called");

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    IMSG("Got value: %u from NW", params[0].value.a);
    params[0].value.a--;
    IMSG("Decrease value to: %u", params[0].value.a);

    return TEE_SUCCESS;
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t
                                      param_types,
                                      TEE_Param params[4]) {
    (void) &sess_ctx; /* Unused parameter */

    switch (cmd_id) {
        case TA_HELLO_WORLD_CMD_INC_VALUE:
            return
                    inc_value(param_types, params
                    );
        case TA_HELLO_WORLD_CMD_DEC_VALUE:
            return
                    dec_value(param_types, params
                    );
        default:
            return TEE_ERROR_BAD_PARAMETERS;
    }
}
