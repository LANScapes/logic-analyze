/* Actual CoreFoundation decoding/type checks; registry/USB/library calls are
 * mocked by the same CLI harness. No IOKit/libusb link or registry access. */
#define DSLCAP_TEST_REAL_CF 1
#include "test_spool.c"
