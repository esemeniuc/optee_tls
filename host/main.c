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

#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <hello_world_ta.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Error: got too few params\n"
               "Usage: ./tool [REGISTER|VERIFY|DECRYPT] params\n"
               "./tool REGISTER EMAIL@EMAIL.com\n"
               "./tool VERIFY EMAIL@EMAIL.com NONCE\n"
               "./tool DECRYPT FILE.capsule\n");
        return 1;
    }

    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = TA_HELLO_WORLD_UUID;
    uint32_t err_origin;

    /* Initialize a context connecting us to the TEE */
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

    /*
     * Open a session to the TA
     */
    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res, err_origin);

    /*
     * Execute a function in the TA by invoking it
     *
     * The value of command ID part and how the parameters are
     * interpreted is part of the interface provided by the TA.
     */

    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));

    /*
     * Prepare the argument. Pass a value in the first parameter,
     * the remaining three parameters are unused.
     */


    if (strcmp(argv[1], "REGISTER") == 0) {
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE,
                                         TEEC_NONE, TEEC_NONE);
        op.params[0].tmpref.buffer = argv[2];
        op.params[0].tmpref.size = strlen(argv[2]) + 1;
        printf("Registering the email: %s\n", argv[2]);
        res = TEEC_InvokeCommand(&sess, TA_APP_REGISTER, &op, &err_origin);
        if (res != TEEC_SUCCESS)
            errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
        printf("Registered the email: %s\n", argv[2]);
    } else if (strcmp(argv[1], "VERIFY") == 0) {
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
                                         TEEC_NONE, TEEC_NONE);
        op.params[0].tmpref.buffer = argv[2];
        op.params[0].tmpref.size = strlen(argv[2]) + 1;
        op.params[1].tmpref.buffer = argv[3];
        op.params[1].tmpref.size = strlen(argv[3]) + 1;
        printf("Verifying the email: %s\n", argv[2]);
        res = TEEC_InvokeCommand(&sess, TA_APP_VERIFY, &op, &err_origin);
        if (res != TEEC_SUCCESS)
            errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
        printf("Verifying the email: %s\n", argv[2]);
    } else if (strcmp(argv[1], "DECRYPT") == 0) {
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE,
                                         TEEC_NONE, TEEC_NONE);
        op.params[0].tmpref.buffer = argv[2];
        op.params[0].tmpref.size = strlen(argv[2]) + 1;
        printf("Decrypting the email: %s\n", argv[2]);
        res = TEEC_InvokeCommand(&sess, TA_APP_DECRYPT, &op, &err_origin);
        if (res != TEEC_SUCCESS)
            errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
        printf("Decrypted the email: %s\n", argv[2]);
    } else {
        printf("Got wrong param 1: %s\n", argv[1]);
    }


    /*
     * We're done with the TA, close the session and
     * destroy the context.
     *
     * The TA will print "Goodbye!" in the log when the
     * session is closed.
     */

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    return 0;
}
