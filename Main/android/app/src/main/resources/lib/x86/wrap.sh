#!/system/bin/sh
export LIBC_DEBUG_MALLOC_OPTIONS="rear_guard=64 backtrace=8"
"$@"
