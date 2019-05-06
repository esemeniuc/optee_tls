#!/bin/bash

make -C host \
CROSS_COMPILE=~/trustedcapsules/code/toolchains/aarch64/bin/aarch64-linux-gnu- \
TEEC_EXPORT=~/trustedcapsules/code/optee_client/out/export \
--no-builtin-variables

make -C ta \
CROSS_COMPILE=~/trustedcapsules/code/toolchains/aarch64/bin/aarch64-linux-gnu- \
PLATFORM=hikey_debian_stable \
TA_DEV_KIT_DIR=~/trustedcapsules/code/optee_os/out/arm/export-ta_arm64
