#!/usr/bin/env bash

mkdir -p /tmp/optee_armtz
scp eric@198.162.52.232:~/dev/optee_examples/hello_world/host/optee_example_hello_world .
scp eric@198.162.52.232:~/dev/optee_examples/hello_world/ta/8aaaf200-2450-11e4-abe2-0002a5d5c51b.ta /tmp/optee_armtz/
