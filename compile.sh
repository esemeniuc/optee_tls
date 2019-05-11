#!/bin/bash
set -e #fail if a single thing fails

CC=~/trustedcapsules/code/toolchains/aarch64/bin/aarch64-linux-gnu-
DIR=~/trustedcapsules/code
#DIR=~/dev/optee_clean

#make -C host V=1 ${1} \
#CROSS_COMPILE=${CC} \
#TEEC_EXPORT=~/dev/optee_clean/optee_client/out/export/usr \
#--no-builtin-variables

make -C host V=1 ${1} \
CROSS_COMPILE=${CC} \
TEEC_EXPORT=~/trustedcapsules/code/optee_client/out/export \
--no-builtin-variables

#mkdir ta/mbedcopy
#cp ~/dev/optee_clean/optee_os/lib/libmbedtls/mbedtls/library/* ./ta/mbedcopy/

make -C /home/eric/trustedcapsules/code/build optee-os
#rm -f home/eric/trustedcapsules/code/optee_os/out/arm/export-ta_arm64/libmbedtls.a

make -C ta V=1 ${1} \
CROSS_COMPILE=${CC} \
PLATFORM=hikey_debian_stable \
CFG_TEE_TA_LOG_LEVEL=4 \
TA_DEV_KIT_DIR=${DIR}/optee_os/out/arm/export-ta_arm64

cat << EOM
 _____ _   _ _____  _____  _____ _____ _____
/  ___| | | /  __ \\/  __ \\|  ___/  ___/  ___|
\\ \`--.| | | | /  \\/| /  \\/| |__ \\ \`--.\\ \`--.
 \`--. \\ | | | |    | |    |  __| \`--. \\\`--. \\
/\\__/ / |_| | \\__/\\| \\__/\\| |___/\\__/ /\\__/ /
\\____/ \\___/ \\____/ \\____/\\____/\\____/\\____/
EOM