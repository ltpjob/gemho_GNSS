#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "spi.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "loopback.h"
#include "gemho_GNSS.h"
#include "net_service.h"
#include "SEE_i2c.h"
#include <string.h>
#include "gnss_config.h"
#include "net_service.h"


uint8_t RXBuffer0[RXBUFFERSIZE];
uint8_t RXBuffer1[RXBUFFERSIZE];

//uint8_t GPSInitCmd[] = {0xB5, 0x62, 0x06, 0x3E, 0x0C, 0x00, 0x00, 0x20, 0x20, 0x01, 0x00, 0x0A, 0x20, 0x00, 0x01, 0x00, 0x00, 0x00, 0xBC, 0x5D, 
//                        0xB5, 0x62, 0x06, 0x3E, 0x0C, 0x00, 0x00, 0x20, 0x20, 0x01, 0x06, 0x08, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0xB0, 0x1F, 
//                        0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x00, 0x21, 0x91, 0xB5, 0x62, 0x06, 0x11, 0x00, 0x00, 0x17, 0x4B,
//                        0xB5, 0x62, 0x06, 0x3E, 0x34, 0x00, 0x00, 0x00, 0x20, 0x06, 0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x10, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, 0x05, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, 0x06, 0x10, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x27, 0x11, 0xB5, 0x62, 0x06, 0x3E, 0x00, 0x00, 0x44, 0xD2,
//                        0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x03, 0x0F, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x23, 0x2C, 
//                        0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x03, 0x10, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x24, 0x33, 
//                        0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x20, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x32, 0x93};

//uint8_t GPSInitCmd[] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x30, 0x01, 0x46, 0xC1, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x50, 0x01, 0x66, 0x01, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x32, 0x01, 0x48, 0xC5, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x33, 0x01, 0x49, 0xC7, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x10, 0x01, 0x26, 0x81, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x31, 0x01, 0x47, 0xC3, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x02, 0x01, 0x18, 0x65, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x01, 0x01, 0x17, 0x63, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x00, 0x01, 0x16, 0x61, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x13, 0x01, 0x24, 0x78, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x09, 0x01, 0x1A, 0x64, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x06, 0x01, 0x17, 0x5E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x61, 0x01, 0x72, 0x14, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x85, 0x01, 0x96, 0x5C, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x12, 0x01, 0x23, 0x76, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x29, 0x01, 0x3A, 0xA4, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x60, 0x01, 0x71, 0x12, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x84, 0x01, 0x95, 0x5A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x0E, 0x01, 0x1F, 0x6E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x69, 0x01, 0x7A, 0x24, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x3E, 0x01, 0x4F, 0xCE, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x02, 0x01, 0x13, 0x56, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x39, 0x01, 0x4A, 0xC4, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x47, 0x01, 0x58, 0xE0, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x01, 0x01, 0x12, 0x54, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x24, 0x01, 0x35, 0x9A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x23, 0x01, 0x34, 0x98, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x17, 0x01, 0x28, 0x80, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x1E, 0x01, 0x2F, 0x8E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x32, 0x01, 0x43, 0xB6, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x3B, 0x01, 0x4C, 0xC8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x86, 0x01, 0x97, 0x5E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x11, 0x52, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x57, 0x01, 0x68, 0x00, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x08, 0x01, 0x19, 0x62, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x34, 0x01, 0x45, 0xBA, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x04, 0x01, 0x15, 0x5A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x11, 0x01, 0x22, 0x74, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x16, 0x01, 0x27, 0x7E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x62, 0x01, 0x73, 0x16, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x1D, 0x01, 0x2E, 0x8C, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x3D, 0x01, 0x4E, 0xCC, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x07, 0x01, 0x18, 0x60, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x31, 0x01, 0x42, 0xB4, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x53, 0x01, 0x64, 0xF8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x1B, 0x01, 0x2C, 0x88, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x10, 0x02, 0x01, 0x1D, 0x74, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x10, 0x02, 0x01, 0x1D, 0x74, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x10, 0x02, 0x01, 0x1D, 0x74, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x10, 0x10, 0x01, 0x2B, 0x90, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x04, 0xFF, 0x01, 0x0E, 0x4A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0xFF, 0x01, 0x2B, 0xA1, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x07, 0x01, 0x33, 0xB1, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x03, 0x01, 0x2F, 0xA9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x0E, 0x01, 0x3A, 0xBF, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x08, 0x01, 0x34, 0xB3, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x09, 0x01, 0x35, 0xB5, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x0B, 0x01, 0x37, 0xB9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x0F, 0x01, 0x3B, 0xC1, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x0D, 0x01, 0x39, 0xBD, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x04, 0x01, 0x30, 0xAB, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0xFF, 0x01, 0x1D, 0x77, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x60, 0x01, 0x7E, 0x39, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x20, 0x01, 0x3E, 0xB9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x21, 0x01, 0x3F, 0xBB, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x06, 0x01, 0x24, 0x85, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x06, 0x01, 0x24, 0x85, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x06, 0x01, 0x24, 0x85, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x06, 0x01, 0x24, 0x85, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x05, 0x01, 0x23, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x05, 0x01, 0x23, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x05, 0x01, 0x23, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x05, 0x01, 0x23, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x05, 0x01, 0x1A, 0x68, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x28, 0x01, 0x3D, 0xAE, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x09, 0x01, 0x1E, 0x70, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x0B, 0x01, 0x20, 0x74, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x02, 0x01, 0x17, 0x62, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x06, 0x01, 0x1B, 0x6A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x27, 0x01, 0x3C, 0xAC, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x07, 0x01, 0x1C, 0x6C, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x21, 0x01, 0x36, 0xA0, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x2E, 0x01, 0x43, 0xBA, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x08, 0x01, 0x1D, 0x6E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x04, 0x01, 0x19, 0x66, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0xFF, 0x01, 0x0B, 0x41, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x60, 0x01, 0x6C, 0x03, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x22, 0x01, 0x2E, 0x87, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x31, 0x01, 0x3D, 0xA5, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x04, 0x01, 0x10, 0x4B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x40, 0x01, 0x4C, 0xC3, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x61, 0x01, 0x6D, 0x05, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x39, 0x01, 0x45, 0xB5, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x09, 0x01, 0x15, 0x55, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x34, 0x01, 0x40, 0xAB, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x01, 0x01, 0x0D, 0x45, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x02, 0x01, 0x0E, 0x47, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x07, 0x01, 0x13, 0x51, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x10, 0x01, 0x1C, 0x63, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x35, 0x01, 0x41, 0xAD, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x32, 0x01, 0x3E, 0xA7, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x06, 0x01, 0x12, 0x4F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x03, 0x01, 0x0F, 0x49, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x30, 0x01, 0x3C, 0xA3, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x24, 0x01, 0x30, 0x8B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x25, 0x01, 0x31, 0x8D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x23, 0x01, 0x2F, 0x89, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x20, 0x01, 0x2C, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x26, 0x01, 0x32, 0x8F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x21, 0x01, 0x2D, 0x85, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x11, 0x01, 0x1D, 0x65, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x12, 0x01, 0x1E, 0x67, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x30, 0x01, 0x3D, 0xA6, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x31, 0x01, 0x3E, 0xA8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x61, 0x01, 0x6E, 0x08, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x41, 0x01, 0x4E, 0xC8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x10, 0x01, 0x1D, 0x66, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x15, 0x01, 0x22, 0x70, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x59, 0x01, 0x66, 0xF8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x11, 0x01, 0x1E, 0x68, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x13, 0x01, 0x20, 0x6C, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x20, 0x01, 0x2D, 0x86, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x27, 0x01, 0x01, 0x33, 0xB7, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x27, 0x03, 0x01, 0x35, 0xBB, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x11, 0x01, 0x29, 0x89, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x16, 0x01, 0x2E, 0x93, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x17, 0x01, 0x2F, 0x95, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x13, 0x01, 0x2B, 0x8D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x04, 0x01, 0x1C, 0x6F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x03, 0x01, 0x1B, 0x6D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x12, 0x01, 0x2A, 0x8B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x01, 0x01, 0x19, 0x69, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x15, 0x01, 0x2D, 0x91, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x06, 0x01, 0x1E, 0x73, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x09, 0xFF, 0x01, 0x13, 0x59, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x09, 0x14, 0x01, 0x28, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xFF, 0xFF, 0x01, 0x09, 0x3B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xFF, 0xFF, 0x01, 0x09, 0x3B};


//uint8_t GPSInitCmd[] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x05, 0x01, 0x1A, 0x68, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x28, 0x01, 0x3D, 0xAE, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x09, 0x01, 0x1E, 0x70, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x0B, 0x01, 0x20, 0x74, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x02, 0x01, 0x17, 0x62, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x06, 0x01, 0x1B, 0x6A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x27, 0x01, 0x3C, 0xAC, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x07, 0x01, 0x1C, 0x6C, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x21, 0x01, 0x36, 0xA0, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x2E, 0x01, 0x43, 0xBA, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x08, 0x01, 0x1D, 0x6E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x04, 0x01, 0x19, 0x66, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0xFF, 0x01, 0x0B, 0x41, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x60, 0x01, 0x6C, 0x03, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x22, 0x01, 0x2E, 0x87, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x31, 0x01, 0x3D, 0xA5, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x04, 0x01, 0x10, 0x4B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x40, 0x01, 0x4C, 0xC3, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x61, 0x01, 0x6D, 0x05, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x39, 0x01, 0x45, 0xB5, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x09, 0x01, 0x15, 0x55, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x34, 0x01, 0x40, 0xAB, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x01, 0x01, 0x0D, 0x45, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x02, 0x01, 0x0E, 0x47, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x07, 0x01, 0x13, 0x51, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x10, 0x01, 0x1C, 0x63, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x35, 0x01, 0x41, 0xAD, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x32, 0x01, 0x3E, 0xA7, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x06, 0x01, 0x12, 0x4F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x03, 0x01, 0x0F, 0x49, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x30, 0x01, 0x3C, 0xA3, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x24, 0x01, 0x30, 0x8B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x25, 0x01, 0x31, 0x8D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x23, 0x01, 0x2F, 0x89, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x20, 0x01, 0x2C, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x26, 0x01, 0x32, 0x8F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x21, 0x01, 0x2D, 0x85, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x11, 0x01, 0x1D, 0x65, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x12, 0x01, 0x1E, 0x67, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x15, 0x01, 0x22, 0x70, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x04, 0x01, 0x1C, 0x6F, 
//0xB5, 0x62, 0x06, 0x3E, 0x34, 0x00, 0x00, 0x00, 0x20, 0x06, 0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x10, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, 0x05, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, 0x06, 0x10, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x27, 0x11, 0xB5, 0x62, 0x06, 0x3E, 0x00, 0x00, 0x44, 0xD2, 0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x7E, 0xB5, 0x62, 0x06, 0x00, 0x01, 0x00, 0x01, 0x08, 0x22};


//uint8_t GPSInitCmd[] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x30, 0x01, 0x46, 0xC1, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x50, 0x01, 0x66, 0x01, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x32, 0x01, 0x48, 0xC5, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x33, 0x01, 0x49, 0xC7, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x10, 0x01, 0x26, 0x81, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x31, 0x01, 0x47, 0xC3, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x02, 0x01, 0x18, 0x65, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x01, 0x01, 0x17, 0x63, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x00, 0x01, 0x16, 0x61, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x13, 0x01, 0x24, 0x78, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x09, 0x01, 0x1A, 0x64, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x06, 0x01, 0x17, 0x5E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x61, 0x01, 0x72, 0x14, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x85, 0x01, 0x96, 0x5C, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x12, 0x01, 0x23, 0x76, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x29, 0x01, 0x3A, 0xA4, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x60, 0x01, 0x71, 0x12, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x84, 0x01, 0x95, 0x5A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x0E, 0x01, 0x1F, 0x6E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x69, 0x01, 0x7A, 0x24, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x3E, 0x01, 0x4F, 0xCE, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x02, 0x01, 0x13, 0x56, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x39, 0x01, 0x4A, 0xC4, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x47, 0x01, 0x58, 0xE0, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x01, 0x01, 0x12, 0x54, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x24, 0x01, 0x35, 0x9A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x23, 0x01, 0x34, 0x98, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x17, 0x01, 0x28, 0x80, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x1E, 0x01, 0x2F, 0x8E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x32, 0x01, 0x43, 0xB6, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x3B, 0x01, 0x4C, 0xC8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x86, 0x01, 0x97, 0x5E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x11, 0x52, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x57, 0x01, 0x68, 0x00, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x08, 0x01, 0x19, 0x62, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x34, 0x01, 0x45, 0xBA, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x04, 0x01, 0x15, 0x5A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x11, 0x01, 0x22, 0x74, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x16, 0x01, 0x27, 0x7E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x62, 0x01, 0x73, 0x16, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x1D, 0x01, 0x2E, 0x8C, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x3D, 0x01, 0x4E, 0xCC, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x07, 0x01, 0x18, 0x60, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x31, 0x01, 0x42, 0xB4, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x53, 0x01, 0x64, 0xF8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x06, 0x1B, 0x01, 0x2C, 0x88, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x10, 0x02, 0x01, 0x1D, 0x74, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x10, 0x02, 0x01, 0x1D, 0x74, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x10, 0x02, 0x01, 0x1D, 0x74, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x10, 0x10, 0x01, 0x2B, 0x90, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x04, 0xFF, 0x01, 0x0E, 0x4A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0xFF, 0x01, 0x2B, 0xA1, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x07, 0x01, 0x33, 0xB1, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x03, 0x01, 0x2F, 0xA9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x0E, 0x01, 0x3A, 0xBF, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x08, 0x01, 0x34, 0xB3, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x09, 0x01, 0x35, 0xB5, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x0B, 0x01, 0x37, 0xB9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x0F, 0x01, 0x3B, 0xC1, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x0D, 0x01, 0x39, 0xBD, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x04, 0x01, 0x30, 0xAB, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0xFF, 0x01, 0x1D, 0x77, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x60, 0x01, 0x7E, 0x39, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x20, 0x01, 0x3E, 0xB9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x03, 0x01, 0x21, 0x7F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x80, 0x01, 0x9E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x21, 0x01, 0x3F, 0xBB, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x02, 0x01, 0x20, 0x7D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x06, 0x01, 0x24, 0x85, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x06, 0x01, 0x24, 0x85, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x06, 0x01, 0x24, 0x85, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x06, 0x01, 0x24, 0x85, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x1E, 0x79, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x40, 0x01, 0x5E, 0xF9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x05, 0x01, 0x23, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x05, 0x01, 0x23, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x05, 0x01, 0x23, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x13, 0x05, 0x01, 0x23, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x05, 0x01, 0x1A, 0x68, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x28, 0x01, 0x3D, 0xAE, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x09, 0x01, 0x1E, 0x70, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x0B, 0x01, 0x20, 0x74, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x02, 0x01, 0x17, 0x62, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x06, 0x01, 0x1B, 0x6A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x27, 0x01, 0x3C, 0xAC, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x07, 0x01, 0x1C, 0x6C, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x21, 0x01, 0x36, 0xA0, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x2E, 0x01, 0x43, 0xBA, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x08, 0x01, 0x1D, 0x6E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x04, 0x01, 0x19, 0x66, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0xFF, 0x01, 0x0B, 0x41, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x60, 0x01, 0x6C, 0x03, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x22, 0x01, 0x2E, 0x87, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x31, 0x01, 0x3D, 0xA5, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x04, 0x01, 0x10, 0x4B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x40, 0x01, 0x4C, 0xC3, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x61, 0x01, 0x6D, 0x05, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x39, 0x01, 0x45, 0xB5, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x09, 0x01, 0x15, 0x55, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x34, 0x01, 0x40, 0xAB, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x01, 0x01, 0x0D, 0x45, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x02, 0x01, 0x0E, 0x47, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x07, 0x01, 0x13, 0x51, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x10, 0x01, 0x1C, 0x63, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x35, 0x01, 0x41, 0xAD, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x32, 0x01, 0x3E, 0xA7, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x06, 0x01, 0x12, 0x4F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x03, 0x01, 0x0F, 0x49, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x30, 0x01, 0x3C, 0xA3, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x24, 0x01, 0x30, 0x8B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x25, 0x01, 0x31, 0x8D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x23, 0x01, 0x2F, 0x89, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x20, 0x01, 0x2C, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x26, 0x01, 0x32, 0x8F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x21, 0x01, 0x2D, 0x85, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x11, 0x01, 0x1D, 0x65, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x12, 0x01, 0x1E, 0x67, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x30, 0x01, 0x3D, 0xA6, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x31, 0x01, 0x3E, 0xA8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x61, 0x01, 0x6E, 0x08, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x41, 0x01, 0x4E, 0xC8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x10, 0x01, 0x1D, 0x66, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x15, 0x01, 0x22, 0x70, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x59, 0x01, 0x66, 0xF8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x11, 0x01, 0x1E, 0x68, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x13, 0x01, 0x20, 0x6C, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x20, 0x01, 0x2D, 0x86, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x27, 0x01, 0x01, 0x33, 0xB7, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x27, 0x03, 0x01, 0x35, 0xBB, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x11, 0x01, 0x29, 0x89, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x16, 0x01, 0x2E, 0x93, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x17, 0x01, 0x2F, 0x95, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x13, 0x01, 0x2B, 0x8D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x04, 0x01, 0x1C, 0x6F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x03, 0x01, 0x1B, 0x6D, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x12, 0x01, 0x2A, 0x8B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x01, 0x01, 0x19, 0x69, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x15, 0x01, 0x2D, 0x91, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x06, 0x01, 0x1E, 0x73, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x09, 0xFF, 0x01, 0x13, 0x59, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x09, 0x14, 0x01, 0x28, 0x83, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xFF, 0xFF, 0x01, 0x09, 0x3B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xFF, 0xFF, 0x01, 0x09, 0x3B, 0xB5, 0x62, 0x06, 0x3E, 0x34, 0x00, 0x00, 0x00, 0x20, 0x06, 0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x10, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, 0x05, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, 0x06, 0x10, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x27, 0x11, 0xB5, 0x62, 0x06, 0x3E, 0x00, 0x00, 0x44, 0xD2, 0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x7E, 0xB5, 0x62, 0x06, 0x00, 0x01, 0x00, 0x01, 0x08, 0x22};



//uint8_t GPSInitCmd[] = {0xB5, 0x62, 0x06, 0x3E, 0x0C, 0x00, 0x00, 0x20, 0x20, 0x01, 0x03, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB4, 0x3B, 
//                        0xB5, 0x62, 0x06, 0x3E, 0x0C, 0x00, 0x00, 0x20, 0x20, 0x01, 0x06, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0xB8, 0x57, 
//                        0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x02, 0x15, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x27, 0x49, 
//                        0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x02, 0x13, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x25, 0x3B};

uint8_t GPSInitCmd_ubxmsg[] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x30, 0x01, 0x3D, 0xA6, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x31, 0x01, 0x3E, 0xA8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x61, 0x01, 0x6E, 0x08, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x14, 0x01, 0x21, 0x6E, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x10, 0x01, 0x1D, 0x66, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x15, 0x01, 0x22, 0x70, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x59, 0x01, 0x66, 0xF8, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x32, 0x01, 0x3F, 0xAA, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x11, 0x01, 0x1E, 0x68, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x13, 0x01, 0x20, 0x6C, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x20, 0x01, 0x2D, 0x86};
uint8_t GPSInitCmd_rtk[] = {0xB5, 0x62, 0x06, 0x3E, 0x0C, 0x00, 0x00, 0x20, 0x20, 0x01, 0x00, 0x0A, 0x20, 0x00, 0x01, 0x00, 0x00, 0x00, 0xBC, 0x5D, 0xB5, 0x62, 0x06, 0x3E, 0x0C, 0x00, 0x00, 0x20, 0x20, 0x01, 0x06, 0x08, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0xB0, 0x1F, 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x03, 0x0F, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x23, 0x2C, 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x03, 0x10, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x24, 0x33, 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x20, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x32, 0x93};
uint8_t GPSInitCmd_baudrate[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x7E, 0xB5, 0x62, 0x06, 0x00, 0x01, 0x00, 0x01, 0x08, 0x22};
uint8_t GPSInitCmd_satellite[] = {0xB5, 0x62, 0x06, 0x3E, 0x34, 0x00, 0x00, 0x00, 0x20, 0x06, 0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, 0x05, 0x00, 0x03, 0x00, 0x01, 0x00, 0x05, 0x01, 0x06, 0x08, 0x0E, 0x00, 0x00, 0x00, 0x01, 0x01, 0x19, 0x15, 0xB5, 0x62, 0x06, 0x3E, 0x00, 0x00, 0x44, 0xD2};

__IO uint8_t g_using_buf0 = 1;
__IO uint8_t g_recv_flag = 0;

wiz_NetInfo WIZNETINFO = {.mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0x66},
                          .ip = {192, 168, 88, 6},
                          .sn = {255,255,255,0},
                          .gw = {192, 168, 88, 1},
                          .dns = {0,0,0,0},
                          .dhcp = NETINFO_STATIC };



static __IO uint32_t TimingDelay;

//int fputc(int ch, FILE *f)
//{
//  return ch;
//}


void Delay_ms(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void RCC_Configuration(void)
{
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 | RCC_APB1Periph_I2C2, ENABLE);
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //usart1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //spi2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Confugure MISO pin as Input Floating  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_Pin_12);
  
  //i2c2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void NVIC_Configuration(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;   
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void DMA_Configuration(void)
{
  DMA_InitTypeDef DMA_InitStructure;
  
  /* USARTy TX DMA1 Channel (triggered by USARTy Tx event) Config */
  DMA_DeInit(DMA1_Channel5);  
  DMA_Cmd(DMA1_Channel5, DISABLE); 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RXBuffer0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = sizeof(RXBuffer0);
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  
  DMA_ClearFlag(DMA1_FLAG_TC5);                                 
  DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);          
}

void USART_GPS_init(uint32_t baudrate)
{
  USART_InitTypeDef USART_InitStructure;
  
  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  
  USART_Cmd(USART1, DISABLE);
  USART_Init(USART1, &USART_InitStructure);
  
  USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
  DMA_Cmd(DMA1_Channel5, ENABLE);
  USART_Cmd(USART1, ENABLE);
}

void USART_GSP_start()
{
  int i = 0;
  
  while(g_recv_flag == 0);
  for(i=0; i<sizeof(GPSInitCmd_ubxmsg); i++)
  {
    USART_SendData(USART1,GPSInitCmd_ubxmsg[i]);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
  }
  
  for(i=0; i<sizeof(GPSInitCmd_rtk); i++)
  {   
    USART_SendData(USART1,GPSInitCmd_rtk[i]);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
  }
  
  for(i=0; i<sizeof(GPSInitCmd_satellite); i++)
  {
    USART_SendData(USART1,GPSInitCmd_satellite[i]);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
  }
  
  for(i=0; i<sizeof(GPSInitCmd_baudrate); i++)
  {
    USART_SendData(USART1,GPSInitCmd_baudrate[i]);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
  }
  
}

void W5500_config()
{
  SPI_Configuration();
  
  // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP 
  /* Critical section callback */
  reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);   //ע���ٽ�������
  /* Chip selection call back */
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
  reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);//ע��SPIƬѡ�źź���
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
  reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);  // CS must be tried with LOW.
#else
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
#error "Unknown _WIZCHIP_IO_MODE_"
#else
  reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#endif
#endif
  /* SPI Read & Write callback function */
  reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);    //ע���д����
  
  SPI_start();
}

void tick_ms_init()
{
  if (SysTick_Config(SystemCoreClock / 1000))
  {
    /* Capture error */ 
    while (1);
  }
}

int main(void)
{
  SystemInit();
  
  RCC_Configuration();
  NVIC_Configuration();
  GPIO_Configuration();
  DMA_Configuration();
  tick_ms_init();
  
  SEE_i2c_init();
  
//  uint8_t buf[128] = "";
//  uint8_t i = 0;
//  uint32_t ret = 0;
//  
//  for(i=0; i<128; i++)
//  {
//    ret = SEE_i2c_write(i, i);
//    if(ret != 0)
//      printf("err:%d\n", ret);
//  }
//  
//  for(i=0; i<128; i++)
//  {
//    ret = SEE_i2c_read(&buf[i], i);
//    if(ret != 0)
//      printf("err:%d\n", ret);
//    else
//      printf("%d\n", buf[i]);
//  }
  

  
  USART_GPS_init(9600);
  USART_GSP_start();
  USART_GPS_init(115200);
  W5500_config();
  
  gnss_config cfg;
  cfg.netinfo = WIZNETINFO;
  
//  int ret = 0;
//  ret = save_config(&cfg);
//  printf("ret:%d\n", ret);
  
  memset(&cfg, 0, sizeof(cfg));
  if(read_config(&cfg) == 0)
  {
    if(cfg.magicnumber == MAGICNUMBER)
    {
      network_init(&cfg.netinfo);
    }
    else
    {
//      while(1);
      network_init(&WIZNETINFO);
    }
  }
  else
  {
//    while(1);
    network_init(&WIZNETINFO);
  }
  
  while(1)
  {
    net_service();
    
  }
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  network_aliveTick();
  TimingDelay_Decrement();
}

void DMA1_Channel5_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_FLAG_TC5))
  {
    if(g_using_buf0 ==0)
    {
      DMA1_Channel5->CMAR = (uint32_t)RXBuffer0;
      g_using_buf0 = 1;
    }
    else
    {
      DMA1_Channel5->CMAR = (uint32_t)RXBuffer1;
      g_using_buf0 = 0;
    }
    
    g_recv_flag++;
    
    DMA_ClearITPendingBit(DMA1_IT_TC5);
  }
}