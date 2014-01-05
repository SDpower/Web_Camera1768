/*
 ===============================================================================
 Name        : main.cpp
 Author      :
 Version     : 1.0.0
 Date		 :
 Copyright   :
 License	 :
 Description : main definition
 ===============================================================================
 	 	 	 	 	 	 	 	 History
 ---------+---------+--------------------------------------------+-------------
 DATE     |	VERSION |	DESCRIPTIONS							 |	By
 ---------+---------+--------------------------------------------+-------------

 ===============================================================================
 */
#include "uCXpresso.h"
#include "arduino.h"
#include "class/pin.h"
#include "probe/probe.h"

#include "debug.h"

CDebug dbg(DBG_USB);

/* ==============================================
 user adjustable pool memory
 ============================================== */
static uint8_t pool[DEFAULT_POOL_SIZE];

#include "class/serial.h"
#include "class/string.h"
#include "lcd/lcd4884.h"
#include "lib/Camera_LS_Y201.h"

LCD4884 lcd;

#define MENU_X	0		// 0-83
#define MENU_Y	0		// 0-5
void init_MENU(void) {
	lcd.LCD_clear();
	lcd.LCD_write_string(MENU_X, MENU_Y, "Web Camera1768", LCD_HIGHLIGHT);
}

void setup_lcd() {
	lcd.LCD_init();
	lcd.LCD_clear();

	//menu initialization
	init_MENU();
}
Camera_LS_Y201 cam1(UART2);

bool init_CAM(void){
	sleep(1000);
	lcd.LCD_write_string(MENU_X, MENU_Y + 1, "Camera init...");

	//init cam1
	bool init_set = false;
	Camera_LS_Y201::ErrorCode resetCode = cam1.reset();
	if (resetCode == Camera_LS_Y201::NoError) {
		lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Reset OK");
		init_set = true;
	} else if (resetCode == Camera_LS_Y201::SendError) {
		lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Reset f:SendError");
	} else if (resetCode == Camera_LS_Y201::RecvError) {
		lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Reset f:RecvError");
	} else if (resetCode == Camera_LS_Y201::UnexpectedReply) {
		lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Reset f:UnexpectedReply");
	}
	//Set cam1 ImageSizeto 640x480
	bool init_ImageSizeto = false;
	if (cam1.setImageSize(Camera_LS_Y201::ImageSize640_480) == Camera_LS_Y201::NoError) {
		lcd.LCD_write_string(MENU_X, MENU_Y + 3, "ImageSize OK");
		init_ImageSizeto = true;
	} else {
		lcd.LCD_write_string(MENU_X, MENU_Y + 3, "ImageSize fail.");
	}

	if (init_ImageSizeto && init_set)
	{
		init_MENU();
		lcd.LCD_write_string(MENU_X, MENU_Y + 1, "Ready to GO.");
		return true;
	}
	else
	{
		lcd.LCD_write_string(MENU_X, MENU_Y + 4, "Please check Camera..");
		return false;
	}

}


/* ==============================================
 main task routine
 ============================================== */
int main(void) {
	pool_memadd((uint32_t) pool, sizeof(pool));

#ifdef DEBUG
	dbg.start();
	probe.start();
#endif

	// Simple demo Code (removable)
	CPin led(LED1);
	setup_lcd();

	bool init_Cam = false;
	init_Cam = init_CAM();
	// Enter an endless loop
	while (1) {
		led = !led;
		sleep(200);

	}
	return 0;
}
