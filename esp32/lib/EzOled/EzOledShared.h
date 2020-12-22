#ifndef EZ_OLED_SHARED_H
#define EZ_OLED_SHARED_H

enum testerba {
  one,
  two
};

#include <stdint.h>
#include <Arduino.h>
#include <EzDebug.h>
#include <Wire.h>
#include <SSD1306Wire.h>

#define OLED_SDA      21
#define OLED_SCL      22
#define OLED_ADDRESS  0x3c

// Can't define this here statically otherwise it creates degraded screen drawing.
extern SSD1306Wire __tft;
// static SSD1306Wire __tft(OLED_ADDRESS, OLED_SDA, OLED_SCL);

// These values greatly affect the dynamic memory.
#define EZTF_MAX_COMPONENTS 50 // Maximum components
#define EZTF_MAX_CHILDREN   30 // Maximum children per component

// @todo Need to namespace these
// Good site for picking colors http://www.barth-dev.de/online/rgb565-color-picker/
// Good site for converting back and forth between RGB565 and RGB888 https://trolsoft.ru/en/articles/rgb565-color-picker
// #define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
// #define WHITE       0xFFFF
#define GREY        0x6B6D
#define SILVER	    0xD6BA
#define NICKLE      0x9492
#define LIGHT_PINK  0xF81F
#define LIGHT_GREY  0xB5B6
#define ORANGE      0xFC60
// #define ALTBG       0x5ACB //0x2965 //0x18E3

#define bitmapdatatype unsigned short*

/** @todo There may be a better, native swap function in the TFT_eSPI library to use **/
#define swapVals(type, i, j) {type t = i; i = j; j = t;}

// For 'rotation'
#define PORTRAIT  0
#define LANDSCAPE 1

// Dimensions of buttons on the top and bottom bar on main page.
// @todo Look into moving these into an existing class. Or organize better.
#define DISP_BTN_WIDTH                  30
#define DISP_BTN_HEIGHT                 16
// Icons
// @todo icon height and width are also declared somewhere else.
#define DISP_ICON_HEIGHT                15
#define DISP_ICON_WIDTH                 15
// Dimensions of standard buttons.
#define DISP_BTN_CLOSE_WIDTH            20
#define DISP_BTN_CLOSE_HEIGHT           DISP_BTN_HEIGHT
#define DISP_BTN_CANCEL_WIDTH           20
#define DISP_BTN_CANCEL_HEIGHT          (DISP_BTN_HEIGHT - 5)
// Window Handling
#define DISP_WND_HDR_HEIGHT             DISP_BTN_HEIGHT + 1
#define DISP_WND_HDR_CLOSE_BTN_SPACING  5
#define DISP_WND_HDR_TEXT_LEN           40
#define DISP_MAX_WINDOWS                3
// Graphs
#define EZGRAPH_BAR_WIDTH               4
#define EZGRAPH_SPACING                 10

enum class TOUCH_STATE:uint8_t {
  none,           // There is nothing touching the component.
  justPressed,    // User just touched the component
  holding,        // User is holding the component down
  unhold,        // User is holding the component down
  releasedSoft,   // User released while not touching the component that was pressed (slid finger)
  releasedHard,    // User released while touching the component that was pressed.
};

enum class BTN_ACTION:uint8_t {
  idle,
  click,
  long_click,
  holding,
};

// Important to have the actions numbered as they are since they're used with bit masking, it
// would also seem that "enum class" and bit masking don't get along.
enum COMP_ACT:uint32_t {
  none					= 1,      // There is nothing touching the component.
  justPressed		= 2,      // User just touched the component
  holding				= 4,      // User is holding the component down
  releasedSoft	= 8,      // User released while not touching the component that was pressed (slid finger)
  releasedHard	= 16,     // User released while touching the component that was pressed.
  enabled				= 32,     // Enabled the control
  disabled			= 64,     // Disable the control
  touchable			= 128,    // Act on touches
  untouchable		= 256,    // Do not act on touches
  visible				= 512,    // Show the component
  invisible			= 1024,   // Hide the component
  initialize		= 2048,   // Perform the first draw if neccessary.
  refresh				= 4096,   // Perform the first draw if neccessary.
  close_wnd		  = 8192,   // Only used with windows.
  reset					= 16384,  // Fires when reseting the component to put back in the component pool.
  cancel				= 32768,  // Cancels a window
  unhold				= 65536,  // User released the "holding" action.
};


enum class ON_STATE:uint8_t {
  ON    = 1,
  OFF   = 2,
  NONE  = 3,
};

enum class BTN_STYLE:uint8_t {
  regular       = 0,  // Standard button. Centered text in rectangle.
  auto_size     = 1,  // Auto size button based on text.
  close_window  = 2,  // Button used to close a window/form.
  indicator     = 3,   // Device indicators. Icon on top with indicator text below. 
};

enum class COMP_TYPE:uint8_t {
  panel               = 0,
  button              = 1,
  window              = 2,
  window_panel        = 3,
  window_back_btn     = 4,
  window_cancel_btn   = 5,
  window_header       = 6,
  window_header_text  = 7,
};

enum class COMP_POS:uint8_t {
  center  = 0,
  left    = 1,
  right   = 2,
  top     = 3,
  bottom  = 4,
};

typedef struct btn_colors_t {
  const uint16_t fg			= WHITE;
  const uint16_t bg			= BLACK;
  const uint16_t accent	= ORANGE;
} btn_colors_t;

typedef struct device_colors_t {
  const uint16_t heater				= RED;
  const uint16_t cooler				= YELLOW;
  const uint16_t humidifier		= CYAN;
  const uint16_t dehumidifier = LIGHT_PINK;
  const uint16_t purge				= GREEN;
} device_colors_t;

typedef struct theme_colors_t {
  const uint16_t				fg 			= WHITE;
  const uint16_t				fg_dim  = GREY;
  const uint16_t				bg 			= BLACK;
  const uint16_t				accent 	= ORANGE;
  const btn_colors_t		btn;
  const device_colors_t device;
} theme_colors_t;

typedef struct theme_btn_t {
  uint16_t       width;
  uint16_t       height;
  const uint8_t  border = 1;
  const uint8_t  offset = 3;
} theme_btn_t;

typedef struct theme_t {
  const theme_colors_t color;
  const uint16_t       windowBorderPx = 4;
  theme_btn_t          btn;
} theme_t;

typedef struct xy_pos_t {
  int16_t x = -1;
  int16_t y = -1;
} xy_pos_t;

class TSPoint {
  public:
    TSPoint(void);
    TSPoint(int16_t x, int16_t y, int16_t z);
    bool operator==(TSPoint);
    bool operator!=(TSPoint);
    uint16_t x, y, z;
    // int16_t x, y, z;
};

#if(EZ_DEBUG_ON)
// This enum, const chars and 
enum class compActTxtTablePos_en {
  none          = 0,
  justPressed   = 1,
  holding       = 2,
  releasedSoft  = 3,
  releasedHard  = 4,
  enabled       = 5,
  disabled      = 6,
  touchable     = 7,
  untouchable   = 8,
  visible       = 9,
  invisible     = 10,
  initialize    = 11,
  refresh       = 12,
  close         = 13,
  reset         = 14,
  cancel        = 15
};
const char compAct_txt_none[]						PROGMEM = "none";
const char compAct_txt_justPressed[]		PROGMEM = "justPressed";
const char compAct_txt_holding[]				PROGMEM = "holding";
const char compAct_txt_releasedSoft[]		PROGMEM = "releasedSoft";
const char compAct_txt_releasedHard[]		PROGMEM = "releasedHard";
const char compAct_txt_enabled[]				PROGMEM = "enabled";
const char compAct_txt_disabled[]				PROGMEM = "disabled";
const char compAct_txt_touchable[]			PROGMEM = "touchable";
const char compAct_txt_untouchable[]		PROGMEM = "untouchable";
const char compAct_txt_visible[]				PROGMEM = "visible";
const char compAct_txt_invisible[]			PROGMEM = "invisible";
const char compAct_txt_initialize[]			PROGMEM = "initialize";
const char compAct_txt_refresh[]				PROGMEM = "refresh";
const char compAct_txt_close[]					PROGMEM = "close";
const char compAct_txt_reset[]					PROGMEM = "reset";
const char compAct_txt_cancel[]					PROGMEM = "cancel";

const char *const _compActTxtTable[] PROGMEM = {
  compAct_txt_none,
  compAct_txt_justPressed,
  compAct_txt_holding,
  compAct_txt_releasedSoft,
  compAct_txt_releasedHard,
  compAct_txt_enabled,
  compAct_txt_disabled,
  compAct_txt_touchable,
  compAct_txt_untouchable,
  compAct_txt_visible,
  compAct_txt_invisible,
  compAct_txt_initialize,
  compAct_txt_refresh,
  compAct_txt_close,
  compAct_txt_reset,
  compAct_txt_cancel
};
#endif

// Used by most of the visual 'Ez' classes
static theme_t __thm;

#endif