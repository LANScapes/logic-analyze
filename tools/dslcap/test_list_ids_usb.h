/* Minimal fake libusb ABI for the hardware-free spool harness. There is no
 * real libusb link: an added non-allowlisted USB call cannot resolve. */
#ifndef DSLCAP_TEST_LIST_IDS_USB_H
#define DSLCAP_TEST_LIST_IDS_USB_H
#include <stdint.h>
#include <sys/types.h>
typedef struct libusb_context { int active; } libusb_context;
typedef struct libusb_device libusb_device;
typedef struct libusb_device_handle { libusb_device *dev; } libusb_device_handle;
struct libusb_device_descriptor {
    uint8_t bLength, bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass, bDeviceSubClass, bDeviceProtocol, bMaxPacketSize0;
    uint16_t idVendor, idProduct, bcdDevice;
    uint8_t iManufacturer, iProduct, iSerialNumber, bNumConfigurations;
};
#define LIBUSB_ENDPOINT_IN 0x80
#define LIBUSB_REQUEST_TYPE_STANDARD 0x00
#define LIBUSB_RECIPIENT_DEVICE 0x00
#define LIBUSB_REQUEST_GET_DESCRIPTOR 0x06
#define LIBUSB_DT_STRING 0x03
static int ids_test_backend_supported;
int libusb_init(libusb_context **ctx);
void libusb_exit(libusb_context *ctx);
ssize_t libusb_get_device_list(libusb_context *ctx, libusb_device ***list);
void libusb_free_device_list(libusb_device **list, int unref);
int libusb_get_device_descriptor(libusb_device *dev, struct libusb_device_descriptor *desc);
uint8_t libusb_get_bus_number(libusb_device *dev);
int libusb_get_port_numbers(libusb_device *dev, uint8_t *ports, int size);
int libusb_open(libusb_device *dev, libusb_device_handle **handle);
void libusb_close(libusb_device_handle *handle);
int libusb_control_transfer(libusb_device_handle *handle, uint8_t type,
    uint8_t request, uint16_t value, uint16_t index, unsigned char *data,
    uint16_t length, unsigned int timeout);
#endif
