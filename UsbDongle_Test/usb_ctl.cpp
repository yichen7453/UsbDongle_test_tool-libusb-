#include "stdafx.h"

#include "usb_ctl.h"

uint8_t		buf_finger[FRAME_SIZE];

uint8_t		frame_toggle	= 0x02;
uint16_t	pixel_cnt		= 0;
int			transfer_cnt	= 0;;

struct libusb_device_handle *dev = NULL;

int usb_open_device(uint16_t vid, uint16_t pid)
{
	int res = -1;

	const struct libusb_version* version;
	version = libusb_get_version();
	printf("Using libusb v%d.%d.%d.%d\n\n", version->major, version->minor, version->micro, version->nano);

	if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
		printf("Hotplug capabilites are not supported on this platform\n");
		libusb_exit(NULL);
	}
	else {
		printf("Hotplug capabilites have supported on this platform\n");
	}

	res = libusb_init(NULL);
	if (res < 0) {
		printf("usb_ctl -> usb_init fali. (%d)\n", res);
	}
	else {
		printf("usb_ctl -> usb_init success. (%d)\n", res);

		dev = libusb_open_device_with_vid_pid(NULL, vid, pid);

		if (dev != NULL) {
			res = libusb_claim_interface(dev, 0);
			if (res < 0) {
				printf("usb_ctl -> clain_interface fail. (%d)\n", res);
			}
			else {
				printf("usb_ctl -> claim_interface success. (%d)\n", res);
			}
		} else {
			res = -1;
		} 
	}

	return res;
}

int usb_start_stream(void) {

	int Rst = -1;
	uint16_t frameSize;

	if (dev != NULL) {
		frameSize = 0;
		Rst = libusb_control_transfer(dev, CTRL_OUT, FPD_CMD_START_STREAM, 0, 0, (uint8_t *)&frameSize, 2, 0);
		if (Rst < 0) {
			printf("usb_ctl -> usb_sensor_start_stream fail. (%d)\n", Rst);
		}
		else {
			printf("usb_ctl -> usb_sensor_start_stream. (%d)\n", Rst);
		}
	}

	return Rst;
}

int usb_stop_stream(void) {

	int Rst = -1;
	uint16_t frameSize;

	if (dev != NULL) {
		frameSize = 0;
		Rst = libusb_control_transfer(dev, CTRL_OUT, FPD_CMD_STOP_STREAM, 0, 0, (uint8_t *)&frameSize, 2, 0);
		if (Rst < 0) {
			printf("usb_ctl -> usb_sensor_stop_stream fail. (%d)\n", Rst);
		}
		else {
			printf("usb_ctl -> usb_sensor_stop_stream. (%d)\n", Rst);
		}
	}

	return Rst;
}

int usb_write_register(unsigned char *data, uint16_t dataLength)
{
	int Rst = -1;

	if (dev != NULL) {
		Rst = libusb_control_transfer(dev, CTRL_OUT, FPD_CMD_WRITE_REG, 0, 0, data, dataLength, 0);
		if (Rst < 0) {
			printf("usb_ctl -> usb_sensor_write_register error. (%d)\n", Rst);
		}
		else {
			printf("usb_ctl -> usb_sensor_write_register success. (%d)\n", Rst);
		}
	}

	return Rst;
}

int usb_read_register(uint16_t index, uint8_t* data)
{
	int Rst = -1;
	uint8_t RegData;

	Rst = libusb_control_transfer(dev, CTRL_IN, FPD_CMD_READ_REG, 0, index, &RegData, 1, 0);
	*data = RegData;
	if (Rst < 0) {
		printf("control transfer IN error(%d)\n", Rst);
	}
	else {
		printf("control transfer IN ok, cmd(0x%.02X)\n", FPD_CMD_READ_REG);
		//printf("RegData(0x%.02X)\n", RegData);
		//printf("RegData(0x%.02X)\n", *data);
	}
	
	return Rst;
}

int usb_get_image_data(struct libusb_device_handle *dev, uint8_t** img_data)
{
	int Rst = -1;
	int hdr_idx = 0;
	int dat_idx = 1;
	int dat_len;

	int hdr_cnt = 0;

	uint8_t buf_trans[TRANSFER_SIZE];
	uint8_t buf_del_hdr[TRANSFER_SIZE];
	int len;

	Rst = libusb_bulk_transfer(dev, 0x81, buf_trans, TRANSFER_SIZE, &len, 0);
	if (Rst < 0) {
		printf("libusb_bulk_transfer error. (%d)\n", Rst);
	}
	else {
		printf("Packet length ----------------- packet index(%d) len(%d)\n", transfer_cnt++, len);
		
		while (hdr_idx < len) {
			if ((buf_trans[hdr_idx] & 0x02) != 0x02) {
				if ((buf_trans[hdr_idx] & 0x01) != frame_toggle) {
					printf("Total transfer ----------------- pixel(%d) total pocket(%d)\n", pixel_cnt, transfer_cnt);
					
					*img_data = buf_finger;					

					Rst = 1;
					pixel_cnt = 0;
					transfer_cnt = 0;

					frame_toggle = (buf_trans[hdr_idx] & 0x01);					
				}

				dat_len = len - dat_idx;
				dat_len = (dat_len > PAYLOAD_SIZE ? PAYLOAD_SIZE : dat_len);

				if ((pixel_cnt + dat_len) <= FRAME_SIZE) {
					memcpy(&buf_finger[pixel_cnt], &buf_trans[dat_idx], dat_len);
					pixel_cnt += dat_len;
				}

				hdr_idx += 64;
				dat_idx += 64;
			}
			else {
				++hdr_idx;
				++dat_len;
			}
		}
		
	}

	return Rst;
}

struct libusb_device_handle *get_dev_handle()
{
	return dev;
}