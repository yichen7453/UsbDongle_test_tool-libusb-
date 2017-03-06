#include "stdafx.h"

#include "libusb.h"


// ------+---------+---------+---------+---------+---------+---------+---------+
// define
// ------+---------+---------+---------+---------+---------+---------+---------+
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof((a)[0]))
#define CTRL_OUT		(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT)
#define CTRL_IN			(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN)
#define CTRL_OUT		(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT)
#define	EP_DATA			(1 | LIBUSB_ENDPOINT_IN)
#define EP_INTR			(2 | LIBUSB_ENDPOINT_IN)
#define USB_RQ			0x05

#define BUFFER_SIZE             4096
#define PAYLOAD_SIZE            63
#define TRANSFER_SIZE           4162

#define FRAME_WIDTH             128
#define FRAME_HEIGHT            128
#define FRAME_SIZE              (FRAME_WIDTH * FRAME_HEIGHT)

// ------+---------+---------+---------+---------+---------+---------+---------+
// FPD Command
// ------+---------+---------+---------+---------+---------+---------+---------+
#define FPD_CMD_WRITE_REG		0x01
#define FPD_CMD_READ_REG		0x02
#define FPD_CMD_READ_FRAME		0x03
#define FPD_CMD_SET_INIT_TABLE	0x04
#define FPD_CMD_GET_ARRTIB		0x05
#define FPD_CMD_SET_ARRTIB		0x06
#define FPD_CMD_START_STREAM	0x07
#define FPD_CMD_STOP_STREAM		0x08
#define FPD_CMD_SELECT_MODE		0x09



int usb_open_device(uint16_t vid, uint16_t pid);
int usb_get_image_data(struct libusb_device_handle *dev, uint8_t** img_data);
int usb_start_stream(void);
int usb_stop_stream(void);
int usb_write_register(unsigned char *data, uint16_t dataLength);
int usb_read_register(uint16_t index, uint8_t* data);


struct libusb_device_handle *get_dev_handle();