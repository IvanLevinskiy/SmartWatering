#include "Menu/LCD_Display.h"
#include <inttypes.h>
#include <ESP8266WiFi.h>
//#include <Arduino.h>
#include <Wire.h>

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LCD_Display::LCD_Display(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize)
{
	_addr = lcd_addr;
	_cols = lcd_cols;
	_rows = lcd_rows;
	_charsize = charsize;
	_backlightval = LCD_BACKLIGHT;
}


//Метод для запуска дисплея
void LCD_Display::begin() 
{
	Wire.begin();
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (_rows > 1) {
		_displayfunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((_charsize != 0) && (_rows == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50); 

	// Now we pull both RS and R/W low to begin commands
	expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	delay(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// second try
	write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// third go!
	write4bits(0x03 << 4); 
	delayMicroseconds(150);

	// finally, set to 4-bit interface
	write4bits(0x02 << 4); 

	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();
	
	// clear it off
	clear();
	
	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
	
	home();
}

//Метод для лчистки дисплея
void LCD_Display::clear()
{
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

//Метод для инициализации дисплея
void LCD_Display::init()
{
	begin();// 
  // this command takes a long time!
} 


//Метод, осуществляющий установку курсора в положение 0,0 (Занимает много времени!).
void LCD_Display::home()
{
	command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}


//Метод, осуществляющий установку курсора в позицию указанную номером колонки и строки.
void LCD_Display::setCursor(uint8_t col, uint8_t row)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };

	if (row > _rows) 
	{
		row = _rows-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}


//Метод, осуществляющий быстрое выключение дисплея (без изменения данных в ОЗУ).
void LCD_Display::noDisplay() 
{
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}


//Метод, осуществляющий быстрое включение дисплея (без изменения данных в ОЗУ).
void LCD_Display::display() 
{
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

//Метод для создания пользовательского символа
void LCD_Display::createChar(uint8_t location, uint8_t charmap[]) 
{
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));

	for (int i=0; i<8; i++) 
	{
		write(charmap[i]);
	}
}

//Выключение подсветки
void LCD_Display::noBacklight(void) 
{
	_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(0);
}

//Включение подсветки
void LCD_Display::backlight(void) 
{
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);
}

/*********** mid level commands, for sending data/cmds */

inline void LCD_Display::command(uint8_t value) 
{
	send(value, 0);
}

inline size_t LCD_Display::write(uint8_t value) 
{
	send(value, Rs);
	return 1;
}


/************ low level data pushing commands **********/

// write either command or data
void LCD_Display::send(uint8_t value, uint8_t mode) 
{
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
	write4bits((highnib)|mode);
	write4bits((lownib)|mode); 
}

void LCD_Display::write4bits(uint8_t value) 
{
	expanderWrite(value);
	pulseEnable(value);
}

void LCD_Display::expanderWrite(uint8_t _data)
{                                        
	Wire.beginTransmission(_addr);
	Wire.write((int)(_data) | _backlightval);
	Wire.endTransmission();   
}


void LCD_Display::pulseEnable(uint8_t _data)
{
	expanderWrite(_data | En);	// En high
	delayMicroseconds(1);		// enable pulse must be >450ns
	
	expanderWrite(_data & ~En);	// En low
	delayMicroseconds(50);		// commands need > 37us to settle
}

//Загрузка пользовательского символа
void LCD_Display::load_custom_character(uint8_t char_num, uint8_t *rows)
{
	createChar(char_num, rows);
}

//Управление подсветкой
void LCD_Display::setBacklight(uint8_t new_val)
{
	if (new_val) 
	{
		backlight();		// turn backlight on
	} 
	else 
	{
		noBacklight();		// turn backlight off
	}
}


void LCD_Display::printHeader(char *header)
{
	//Осуществляем печать заголовка только в том случае, если
	//он отличается от того, что в уже напечатан
	//if(StringCompare(Header, header) == false)
	//{
		//Header = header;
		home();
		print(header);
	//}

}


void LCD_Display::AnimationEdit()
{
	 if(AnimationStep == 0)
    {
        setCursor(0,1);
        print(char(0));
    }

    if(AnimationStep == 1)
    {
        setCursor(0,1);
        print("_");
        print(char(0));
    }

    if(AnimationStep == 2)
    {
        setCursor(0,1);
        print("__");
        print(char(0));
    }

    if(AnimationStep == 3)
    {
        setCursor(0,1);
        print("      ");
    }

    AnimationStep ++;
    if(AnimationStep > 3)
    {
        AnimationStep = 0;
    }    
}

//Печать числа как времени
void LCD_Display::printIntAsTime(int time)
{
	int hour = time / 60;
	int minute = time - (hour * 60);

	// Форматирование и отображение времени
    setCursor(11,1);
    if (hour < 10) print("0");
    print(hour); print(":"); 
    if (minute < 10) print("0");
    print(minute); 
}

//стоп анимации
void LCD_Display::AnimationStop()
{
	 if(AnimationStep != 0)
    {
		AnimationStep = 0;
        setCursor(0,1);
        print("     ");
    }
}
