global-incdirs-y += include
srcs-y += hello_world_ta.c

srcs-y += mbed_optee_compat.c

# To remove a certain compiler flag, add a line like this
#cflags-template_ta.c-y += -Wno-strict-prototypes
