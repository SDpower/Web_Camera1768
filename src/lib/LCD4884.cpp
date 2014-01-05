/*
 ===============================================================================
 Name        : lcd48x84 @ extensions
 Modify      : Jason
 Date		 : 2012/06/09
 Version     : 1.0.0
 Copyright   : Copyright (C) 2012 Embeda Technology Inc.
 License	 : CC BY-SA
 Description : LCD 48x84, modify for UNO.Net
 History	 :
 ===============================================================================
 */

/*
 Modified by Lauren
 version 0.3

 Any suggestions are welcome.
 E-mail: Lauran.pan@gmail.com

 Editor     : Lauren from DFRobot
 Date       : 06.01.2012

 =��=

 * Have the back light under control.
 * Update the library and sketch to compatible with IDE V1.0 and earlier

 */

#include "lcd/LCD4884.h"
#include "lcd/font_6x8.h"
#include "lcd/font_big.h"
#include "lcd/digiNum16x24.h"

/* 2012/06/09, mask by Jason
 #if defined(ARDUINO) && ARDUINO >= 100
 #include "Arduino.h"
 #else
 #include "WProgram.h"
 #include "WConstants.h"
 #endif


 extern "C"
 {
 #include <avr/pgmspace.h>
 #include <avr/io.h>
 }
 */
#include "class/timeout.h"

LCD4884::LCD4884() :
	lcd(UNO_2, UNO_3, UNO_4, UNO_5, UNO_6, UNO_7, END) {

}

//LCD4884 lcd = LCD4884();

void LCD4884::backlight(byte dat) {
	if (dat == 1)
		lcd[LCD_BL] = HIGH;
	else
		lcd[LCD_BL] = LOW;
}

void LCD4884::LCD_init(void) {
	CTick to;

	for (int i = 0; i < 6; i++) {
		lcd[i].output(NOT_OPEN); // set lcd I/F as output pins
		if ( i==SPI_SCK ) {
			lcd[i] = HIGH;
		} else {
			lcd[i] = LOW;
		}
	}

	lcd[LCD_RST] = LOW;
	to.delay(2);
	lcd[LCD_RST] = HIGH;

	lcd[SPI_CS] = LOW;
	to.delay(2);
	lcd[SPI_CS] = HIGH;
	to.delay(2);
	lcd[LCD_BL] = HIGH;

	LCD_write_byte(0x21, 0);
	LCD_write_byte(0xc0, 0);
	LCD_write_byte(0x06, 0);
	LCD_write_byte(0x13, 0);
	LCD_write_byte(0x20, 0);
	LCD_clear();
	LCD_write_byte(0x0c, 0);

//	lcd[SPI_CS] = HIGH;
}

void LCD4884::LCD_write_byte(byte dat, byte dat_type) {
	unsigned int i;
	lcd[SPI_CS] = LOW;

	if (dat_type == 0) {
		lcd[LCD_DC] = LOW;
	} else {
		lcd[LCD_DC] = HIGH;
	}

	for (i = 0; i < 8; i++) {
		if (dat & 0x80) {
			lcd[SPI_MOSI] = HIGH;
		} else {
			lcd[SPI_MOSI] = LOW;
		}
		lcd[SPI_SCK] = LOW;
		dat = dat << 1;
		lcd[SPI_SCK] = HIGH;
	}
	lcd[SPI_CS] = HIGH;
}

void LCD4884::LCD_draw_bmp_pixel(byte X, byte Y,
		const byte *map, byte Pix_x, byte Pix_y) {
	unsigned int i, n;
	byte row;

	if (Pix_y % 8 == 0)
		row = Pix_y / 8;
	else
		row = Pix_y / 8 + 1;

	for (n = 0; n < row; n++) {
		LCD_set_XY(X, Y);
		for (i = 0; i < Pix_x; i++) {
			LCD_write_byte(map[i + n * Pix_x], 1);
		}
		Y++;
	}
}

void LCD4884::LCD_write_string(byte X, byte Y, const char *s,
		char mode) {
	LCD_set_XY(X, Y);
	while (*s) {
		LCD_write_char(*s, mode);
		s++;
	}
}

void LCD4884::LCD_write_chinese(byte X, byte Y,
		const byte *c, byte ch_with, byte num,
		byte line, byte row) {
	byte i, n;
	LCD_set_XY(X, Y);
	for (i = 0; i < num;) {
		for (n = 0; n < ch_with * 2; n++) {
			if (n == ch_with) {
				if (i == 0)
					LCD_set_XY(X, Y + 1);
				else {
					LCD_set_XY((X + (ch_with + row) * i), Y + 1);
				}
			}
			LCD_write_byte(c[(i * ch_with * 2) + n], 1);
		}
		i++;
		LCD_set_XY((X + (ch_with + row) * i), Y);
	}
}

void LCD4884::LCD_write_string_big(byte X, byte Y,
		const char *string, char mode) {
	while (*string) {

		LCD_write_char_big(X, Y, *string, mode);

		if (*string++ == '.')
			X += 5;
		else
			X += 12;
	}
}

void LCD4884::LCD_write_digital_num(byte x, byte y, const char *num, char mode) {
	while(*num) {
		LCD_write_digital_chr(x, y, *num++, mode);
		x += DSP_DIGIT_SIZE_WIDTH;
	}
}

void LCD4884::LCD_write_digital_chr(byte x, byte y, byte ch, char mode) {
	int i, j;
	int ch_dat;

	if ( ch==':' ) {
		ch = 10;
	} else if ( ch=='.' ) {
		ch = 11;
	} else if ( ch==' ' ) {
		ch = 12;
	} else if ( ch>='0' && ch<='9' ) {
		ch -= '0';
	} else {
		return; // error character
	}

	for (i=0; i<3; i++) { // width = 24 = 3byte
		LCD_set_XY(x, y + i);
		for (j = 0; j < 16; j++) {
			ch_dat = digatalFont[ch][i*16+j];
			LCD_write_byte((mode == LCD_NORMAL) ? ch_dat : (ch_dat ^ 0xff), 1);
		}
	}
}


/* write char in big font */
void LCD4884::LCD_write_char_big(byte X, byte Y, byte ch, char mode) {
	int i, j;
	int ch_dat;


	if (ch == '.')
		ch = 10;
	else if (ch == '+')
		ch = 11;
	else if (ch == '-')
		ch = 12;
	else
		ch = ch & 0x0f;

	for (i = 0; i < 3; i++) {
		LCD_set_XY(X, Y + i);

		for (j = 0; j < 16; j++) {
			//ch_dat =  pgm_read_byte(pFont+ch*48 + i*16 +j);
			ch_dat = big_number[ch][i][j];
			LCD_write_byte((mode == LCD_NORMAL) ? ch_dat : (ch_dat ^ 0xff), 1);
		}
	}
}

void LCD4884::LCD_write_char(byte c, char mode) {
	byte line;
	byte ch;

	c -= 0x20;

	for (line = 0; line < 6; line++) {
		//ch = pgm_read_byte(pFont+c*6+line);
		ch = font6_8[c][line];
		LCD_write_byte((mode == LCD_NORMAL) ? ch : (ch ^ 0xff), 1);
	}
}

void LCD4884::LCD_set_XY(byte X, byte Y) {
	LCD_write_byte(0x40 | Y, 0); // column
	LCD_write_byte(0x80 | X, 0); // row
}

void LCD4884::LCD_clear(void) {
	unsigned int i;

	LCD_write_byte(0x0c, 0);
	LCD_write_byte(0x80, 0);

	for (i = 0; i < 504; i++)
		LCD_write_byte(0, 1);
}

