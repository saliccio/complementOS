#pragma once

#include "errCodes.h"

// VGA driver implementation (25x80 mode for now)

#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define COLOR_DEFAULT 0x0F

#define DEFAULT_ATTR_BYTE 0x0 // default color is white

#define SCREEN_CTRL_PORT 0x3D4 // address for the port-mapped control register of the screen device
#define SCREEN_DATA_PORT 0x3D5 // address for the port-mapped data register of the screen device