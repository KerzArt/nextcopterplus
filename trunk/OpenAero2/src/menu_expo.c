//***********************************************************
//* menu_expo.c
//***********************************************************

//***********************************************************
//* Includes
//***********************************************************

#include <avr/pgmspace.h> 
#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/delay.h>
#include "..\inc\io_cfg.h"
#include "..\inc\init.h"
#include "..\inc\mugui.h"
#include "..\inc\glcd_menu.h"
#include "..\inc\menu_ext.h"
#include "..\inc\glcd_driver.h"
#include "..\inc\main.h"
#include "..\inc\eeprom.h"

//************************************************************
// Prototypes
//************************************************************

// Menu items
void menu_expo(void);

//************************************************************
// Defines
//************************************************************

#define EXPOITEMS 4 	// Number of menu items
#define EXPOSTART 92 	// Start of Menu text items
#define EXPOTEXT 0	 	// Start of value text items

//************************************************************
// Battery menu items
//************************************************************

const uint8_t ExpoMenuOffsets[EXPOITEMS] PROGMEM = {90, 90, 90, 90};
const uint8_t ExpoMenuText[EXPOITEMS] PROGMEM = {EXPOTEXT, 0, 0, 0};
const menu_range_t expo_menu_ranges[] PROGMEM = 
{
	{0,100,10,0,0}, 	// Min, Max, Increment, Style, Default
	{0,100,10,0,0},
	{0,100,10,0,0},
	{0,100,10,0,0},
};
//************************************************************
// Main menu-specific setup
//************************************************************

void menu_expo(void)
{
	uint8_t cursor = LINE0;
	uint8_t button = 0;
	uint8_t top = EXPOSTART;
	uint8_t temp = 0;
	int16_t values[EXPOITEMS];
	menu_range_t range;
	uint8_t text_link = 0;
	
	while(button != BACK)
	{
		// Clear buffer before each update
		clear_buffer(buffer);	

		// Load values from eeprom
		values[0] = Config.AileronExpo;
		values[1] = Config.ElevatorExpo;
		values[2] = Config.RudderExpo;
		values[3] = Config.Differential;

		// Print menu
		print_menu_items(top, EXPOSTART, &values[0], (prog_uchar*)expo_menu_ranges, (prog_uchar*)ExpoMenuOffsets, (prog_uchar*)ExpoMenuText, cursor);

		// Poll buttons when idle
		button = poll_buttons();
		while (button == NONE)					
		{
			button = poll_buttons();
		}

		// Handle menu changes
		update_menu(EXPOITEMS, EXPOSTART, button, &cursor, &top, &temp);

		range = get_menu_range ((prog_uchar*)expo_menu_ranges, temp - EXPOSTART);
		//range = expo_menu_ranges[temp - EXPOSTART];

		if (button == ENTER)
		{
			text_link = pgm_read_byte(&ExpoMenuText[temp - EXPOSTART]);
			values[temp - EXPOSTART] = do_menu_item(temp, values[temp - EXPOSTART], range ,-35, text_link);
		}

		// Update value in config structure
		Config.AileronExpo = values[0];
		Config.ElevatorExpo = values[1];
		Config.RudderExpo = values[2];
		Config.Differential = values[3];

		if (button == ENTER)
		{
			Save_Config_to_EEPROM(); // Save value and return
		}
	}
	menu_beep();
	_delay_ms(200);
}

