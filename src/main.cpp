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
	lcd.LCD_write_string(MENU_X, MENU_Y, "CAM TESTING", LCD_HIGHLIGHT);
}

void setup() {
	lcd.LCD_init();
	lcd.LCD_clear();

	//menu initialization
	init_MENU();
}
Camera_LS_Y201 cam1(UART2);

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
	CString str;
	setup();

	sleep(1000);
	lcd.LCD_write_string(MENU_X, MENU_Y + 1, "Camera module");

//	CSerial serial(UART2,4096);
//	serial.settings(38400);
//	//int counter = 0;
//	while(!serial.writeable())
//	{
//		sleep(10);
//		lcd.LCD_write_string(MENU_X, MENU_Y + 3, "wait ready");
//	}
//
//	char  send[4] = {
//	        0x56,
//	        0x00,
//	        0x26,
//	        0x00
//	    };
//	char  recv[4];
//
//    if (!serial.write(send,4)) {
//    	lcd.LCD_write_string(MENU_X, MENU_Y + 3, "send fail");
//    }else{
//    	lcd.LCD_write_string(MENU_X, MENU_Y + 3, "send OK");
//    }
//    if (!serial.read(recv,4)) {
//    	lcd.LCD_write_string(MENU_X, MENU_Y + 4, "recv fail");
//    }else{
//    	lcd.LCD_write_string(MENU_X, MENU_Y + 4, "recv OK");
//    	lcd.LCD_write_string(MENU_X, MENU_Y + 5, recv);
//    	if ((recv[0] == 0x76)
//    	            && (recv[1] == 0x00)
//    	            && (recv[2] == 0x26)
//    	            && (recv[3] == 0x00)) {
//    		lcd.LCD_write_string(MENU_X, MENU_Y + 4, "init OK wait..");
//    	}
//    }

	Camera_LS_Y201::ErrorCode resetCode = cam1.reset();
    if (resetCode == Camera_LS_Y201::NoError) {
    	lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Reset OK");
	} else if (resetCode == Camera_LS_Y201::SendError) {
		lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Reset f:SendError");
	} else if (resetCode == Camera_LS_Y201::RecvError) {
		lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Reset f:RecvError");
	} else if (resetCode == Camera_LS_Y201::UnexpectedReply) {
		lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Reset f:UnexpectedReply");
	}

    if (cam1.setImageSize(Camera_LS_Y201::ImageSize640_480) == Camera_LS_Y201::NoError) {
    	lcd.LCD_write_string(MENU_X, MENU_Y + 3, "ImageSize OK");
	} else {
		lcd.LCD_write_string(MENU_X, MENU_Y + 3, "ImageSize fail.");
	}

	// Enter an endless loop
	while (1) {
		led = !led;
		sleep(200);

	}
	return 0;
}
