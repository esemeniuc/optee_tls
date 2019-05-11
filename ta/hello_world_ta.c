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

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void) {
    return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void) {
}

#include "tee_tcpsocket.h"
#include <string.h>
#include <pubpriv.h>

TEE_Result
registerReq(int fd, const char *SERVER_NAME, int SERVER_PORT, const char *email, uint8_t *resp, size_t resp_size) {
    char regReq[2048], json_payload[2048];
    snprintf(json_payload, sizeof(json_payload), "{\"email\": \"%s\", \"pubkey\": \"%s\"}", email, pubkey);
    snprintf(regReq, sizeof(regReq), "POST /register HTTP/1.1\r\n"
                                     "Host: %s:%d\r\n"
                                     "User-Agent: TrustedCapsule/0.0.1\r\n"
                                     "Accept: */*\r\n"
                                     "Content-Type: application/json\r\n"
                                     "Content-Length: %lu\r\n"
                                     "\r\n"
                                     "%s\r\n", SERVER_NAME, SERVER_PORT, strlen(json_payload), json_payload);

    int bytesSent = 0, bytesRecv = 0;
    if (TEE_SimpleSendConnection(fd, regReq, sizeof(regReq), &bytesSent) != TEE_SUCCESS) {
        EMSG("Failed to send register request");
        return TEE_ERROR_COMMUNICATION;
    }
    do {
        resp += bytesRecv;
        resp_size -= bytesRecv;
        if (TEE_SimpleRecvConnection(fd, resp, resp_size, &bytesRecv) != TEE_SUCCESS) {
            EMSG("Failed to receive register response");
            return TEE_ERROR_COMMUNICATION;
        }
        printf("got %d bytes\n", bytesRecv);
        for (int i = 0; i < bytesRecv; i++)
            printf("%c", resp[i]);
    } while (bytesRecv > 0);

    return TEE_SUCCESS;
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

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

/* Unused parameters */
    (void) &params;
    (void) &sess_ctx;

/* If return value != TEE_SUCCESS the session will not be created. */
    return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void *sess_ctx) {
    (void) &sess_ctx; /* Unused parameter */
}


static TEE_Result ta_register(uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                               TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE);


    if (param_types != exp_param_types) {
        EMSG("Expected: 0x%x, got: 0x%x", exp_param_types, param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }

    char *email = params[0].memref.buffer;
    char SERVER_NAME[] = "198.162.52.232";
    int SERVER_PORT = 5000;

    int fd;
    if (TEE_SimpleOpenConnection(SERVER_NAME, SERVER_PORT, &fd) != TEE_SUCCESS) {
        EMSG("Failed to open socket");
        return TEE_ERROR_COMMUNICATION;
    }
    unsigned char resp[4096];
    return registerReq(fd, SERVER_NAME, SERVER_PORT, email, resp, sizeof(resp));
}


TEE_Result
verifyReq(int fd,
          const char *SERVER_NAME,
          int SERVER_PORT,
          const char *email,
          const char *nonce,
          uint8_t *resp,
          size_t resp_size) {
    char verifyReq[2048], json_payload[2048];
    snprintf(json_payload, sizeof(json_payload),
             "{\"email\": \"%s\", \"pubkey\": \"%s\", \"nonce\": \"%s\"}",
             email, pubkey, nonce);
    snprintf(verifyReq, sizeof(verifyReq), "POST /verify HTTP/1.1\r\n"
                                           "Host: %s:%d\r\n"
                                           "User-Agent: TrustedCapsule/0.0.1\r\n"
                                           "Accept: */*\r\n"
                                           "Content-Type: application/json\r\n"
                                           "Content-Length: %lu\r\n"
                                           "\r\n"
                                           "%s\r\n", SERVER_NAME, SERVER_PORT, strlen(json_payload), json_payload);

    int bytesSent = 0, bytesRecv = 0;
    if (TEE_SimpleSendConnection(fd, verifyReq, sizeof(verifyReq), &bytesSent) != TEE_SUCCESS) {
        EMSG("Failed to send verify request");
        return TEE_ERROR_COMMUNICATION;
    }
    do {
        resp += bytesRecv;
        resp_size -= bytesRecv;
        if (TEE_SimpleRecvConnection(fd, resp, resp_size, &bytesRecv) != TEE_SUCCESS) {
            EMSG("Failed to receive verify response");
            return TEE_ERROR_COMMUNICATION;
        }
        printf("got %d bytes\n", bytesRecv);
        for (int i = 0; i < bytesRecv; i++)
            printf("%c", resp[i]);
    } while (bytesRecv > 0);

    return TEE_SUCCESS;
}

static TEE_Result ta_verify(uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                               TEE_PARAM_TYPE_MEMREF_INPUT,
                                               TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE);


    if (param_types != exp_param_types) {
        EMSG("Expected: 0x%x, got: 0x%x", exp_param_types, param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }

    char *email = params[0].memref.buffer;
    char *nonce = params[1].memref.buffer;
    char SERVER_NAME[] = "198.162.52.232";
    int SERVER_PORT = 5000;

    int fd;
    if (TEE_SimpleOpenConnection(SERVER_NAME, SERVER_PORT, &fd) != TEE_SUCCESS) {
        DMSG("FAIL OPEN");
    }
    unsigned char resp[4096];

    //TODO decrypt nonce

    return verifyReq(fd, SERVER_NAME, SERVER_PORT, email, nonce, resp, sizeof(resp));
}

static TEE_Result ta_decrypt(uint32_t param_types, TEE_Param params[4]) {
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
        case TA_APP_REGISTER:
            return ta_register(param_types, params);
        case TA_APP_VERIFY:
            return ta_verify(param_types, params);
        case TA_APP_DECRYPT:
            return ta_verify(param_types, params);
        default:
            return TEE_ERROR_BAD_PARAMETERS;
    }
}