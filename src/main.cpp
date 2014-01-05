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
#include "class/file.h"
#include "debug.h"

CDebug dbg(DBG_USB);

/* ==============================================
 user adjustable pool memory
 ============================================== */
static uint8_t pool[DEFAULT_POOL_SIZE];

CFile file;

#include "class/serial.h"
#include "class/string.h"
#include "lcd/lcd4884.h"


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

#include "lib/Camera_LS_Y201.h"
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

/**
 * Callback function for readJpegFileContent.
 *
 * @param buf A pointer to a buffer.
 * @param siz A size of the buffer.
 */
void callback_func(int done, int total, uint8_t *buf, size_t siz) {
    file.write(buf,siz);
    static int n = 0;
    int tmp = done * 100 / total;
    if (n != tmp) {
        n = tmp;
        //DEBMSG("Writing...: %3d%%", n);
        CString str;
        str.printf("Writing: %3d%%", n);
        lcd.LCD_write_string(MENU_X, MENU_Y + 5, str);
    }
}

int capture( char *filename) {
    /*
     * Take a picture.
     */
	Camera_LS_Y201::ErrorCode takeCode = cam1.takePicture();
    if (takeCode == Camera_LS_Y201::NoError) {
    	lcd.LCD_write_string(MENU_X, MENU_Y + 2, "takePicture OK");
    } else if (takeCode == Camera_LS_Y201::SendError) {
		lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Take f:SendError");
		return -1;
	} else if (takeCode == Camera_LS_Y201::RecvError) {
		lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Take f:RecvError");
		return -1;
	} else if (takeCode == Camera_LS_Y201::UnexpectedReply) {
		lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Take f:UnexpectedReply");
		return -1;
	}
    lcd.LCD_write_string(MENU_X, MENU_Y + 2, "Captured.");

    file.path("/webcam1768");
    if ( file.open(filename,CFile::WRITE) ) {
    	lcd.LCD_write_string(MENU_X, MENU_Y + 3, "Write Image...");
    	if (cam1.readJpegFileContent(callback_func) != 0) {
    		lcd.LCD_write_string(MENU_X, MENU_Y + 4, "readJpegFileContent fail");
    		file.close();
    		return -3;
    	}
    	file.close();
    }
    else
    {
    	return -2;
    }

    cam1.stopTakingPictures();

    return 0;
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

	bool init_Cam = init_CAM();
	if (init_Cam)
	{
		int r = capture("test.jpg");
	}
	// Enter an endless loop
	while (1) {
		led = !led;
		sleep(200);

	}
	return 0;
}
