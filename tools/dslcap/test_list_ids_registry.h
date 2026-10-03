/* Only read-only registry calls are declared. The harness does not link IOKit
 * or libusb: adding a device/user-client operation fails compilation/linking. */
#ifndef DSLCAP_TEST_REGISTRY_H
#define DSLCAP_TEST_REGISTRY_H
#include <stdbool.h>
#include <stdint.h>
#ifdef DSLCAP_TEST_REAL_CF
#include <CoreFoundation/CoreFoundation.h>
#else
#include <stddef.h>
typedef struct ids_cf *CFTypeRef;
typedef CFTypeRef CFNumberRef;
typedef CFTypeRef CFStringRef;
typedef CFTypeRef CFMutableDictionaryRef;
typedef CFTypeRef CFDictionaryRef;
typedef const void *CFAllocatorRef;
typedef unsigned long CFTypeID;
typedef long CFIndex;
typedef int CFNumberType;
typedef unsigned int CFStringEncoding;
typedef unsigned char UInt8;
typedef unsigned char Boolean;
typedef struct { CFIndex location, length; } CFRange;
#define kCFAllocatorDefault NULL
#define kCFNumberSInt64Type 4
#define kCFStringEncodingUTF8 0x08000100U
/* Property keys are static strings, not copied property values. */
#define CFSTR(s) ((CFStringRef)(s))
static inline CFRange CFRangeMake(CFIndex start, CFIndex length) { return (CFRange){start, length}; }
CFTypeID CFGetTypeID(CFTypeRef value);
CFTypeID CFNumberGetTypeID(void);
CFTypeID CFStringGetTypeID(void);
Boolean CFNumberIsFloatType(CFNumberRef value);
Boolean CFNumberGetValue(CFNumberRef value, CFNumberType type, void *out);
CFIndex CFStringGetLength(CFStringRef value);
CFIndex CFStringGetBytes(CFStringRef value, CFRange range, CFStringEncoding encoding,
    UInt8 loss, Boolean external, UInt8 *buffer, CFIndex size, CFIndex *used);
CFTypeRef CFRetain(CFTypeRef value);
void CFRelease(CFTypeRef value);
#endif

typedef uint32_t io_object_t;
typedef io_object_t io_registry_entry_t;
typedef io_object_t io_iterator_t;
typedef uint32_t mach_port_t;
typedef int kern_return_t;
typedef unsigned int IOOptionBits;
typedef int boolean_t;
#define kIOMainPortDefault 0
#define KERN_SUCCESS 0
CFMutableDictionaryRef IOServiceMatching(const char *name);
kern_return_t IOServiceGetMatchingServices(mach_port_t port, CFDictionaryRef matching,
                                           io_iterator_t *iterator);
io_object_t IOIteratorNext(io_iterator_t iterator);
boolean_t IOIteratorIsValid(io_iterator_t iterator);
kern_return_t IOObjectRelease(io_object_t object);
kern_return_t IORegistryEntryGetRegistryEntryID(io_registry_entry_t entry, uint64_t *entry_id);
CFTypeRef IORegistryEntryCreateCFProperty(io_registry_entry_t entry, CFStringRef key,
                                         CFAllocatorRef allocator, IOOptionBits options);
static int ids_test_use_registry = 1;
#endif
