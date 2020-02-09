#include "Menu/LCD_Display.h"
#include <inttypes.h>
//#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Wire.h>

#if (defined(__AVR__))
#include <avr\pgmspace.h>
#else
#include <pgmspace.h>
#endif

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
// LCD_Display constructor is called).

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

/**
void LCD_Display::print(const wchar_t *_str)
{

  int current_char  = 0;
  int size = 0;
 
  //Определяем длину строки (количество символов)
  while(_str[size] != 0)
  {
    size++;
  }
  
  while(current_char < size)
  {
    printwc(_str[current_char]);
    current_char++;
    cursor_col++;
  }
}

void LCD_Display::printwc(const wchar_t _chr)
{
  uint8_t rus_[8];
  switch(_chr)
  {
    //Русский алфавит, требующий новых символов
    //Единовременно может быть заменено только 8 символов
    case 1041: //Б
      memcpy_PF(rus_, (const void*)rus_B, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_B);
    break;
    case 1043: //Г
      memcpy_PF(rus_, (const void*)rus_G, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_G);
    break;
    case 1044: //Д
      memcpy_PF(rus_, (const void*)rus_D, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_D);
    break;
    case 1046: //Ж
      memcpy_PF(rus_, (const void*)rus_ZH, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_ZH);
    break;
    case 1047: //З
      memcpy_PF(rus_, (const void*)rus_Z, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_Z);
    break;
    case 1048: //И
      memcpy_PF(rus_, (const void*)rus_I, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_I);
    break;
    case 1049: //Й
      memcpy_PF(rus_, (const void*)rus_II, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_II);
    break;
    case 1051: //Л
      memcpy_PF(rus_, (const void*)rus_L, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_L);
    break;
    case 1055: //П
      memcpy_PF(rus_, (const void*)rus_P, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_P);
    break;
    case 1059: //У
      memcpy_PF(rus_, (const void*)rus_U, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_U);
    break;
    case 1060: //Ф
      memcpy_PF(rus_, (const void*)rus_F, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_F);
    break;
    case 1062: //Ц
      memcpy_PF(rus_, (const void*)rus_TS, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_TS);
    break;
    case 1063: //Ч
      memcpy_PF(rus_, (const void*)rus_CH, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_CH);
    break;
    case 1064: //Ш
      memcpy_PF(rus_, (const void*)rus_SH, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_SH);
    break;
    case 1065: //Щ
      memcpy_PF(rus_, (const void*)rus_SCH, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_SCH);
    break;
    case 1066: //Ъ
      memcpy_PF(rus_, (const void*)rus_tverd, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_tverd);
    break;
    case 1067: //Ы
      memcpy_PF(rus_, (const void*)rus_Y, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_Y);
    break;
    case 1068: //Ь
      memcpy_PF(rus_, (const void*)rus_myagk, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_myagk);
    break;
    case 1069: //Э
      memcpy_PF(rus_, (const void*)rus_EE, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_EE);
    break;
    case 1070: //Ю
      memcpy_PF(rus_, (const void*)rus_YU, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_YU);
    break;
    case 1071: //Я
      memcpy_PF(rus_, (const void*)rus_YA, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_YA);
    break;
    case 1073: //б
      memcpy_PF(rus_, (const void*)rus_b, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_b);
    break;
    case 1074: //в
      memcpy_PF(rus_, (const void*)rus_v, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_v);
    break;
    case 1075: //г
      memcpy_PF(rus_, (const void*)rus_g, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_g);
    break;
    case 1076: //д
      memcpy_PF(rus_, (const void*)rus_d, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_d);
    break;
    case 1105: //ё
      memcpy_PF(rus_, (const void*)rus_yo, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_yo);
    break;
    case 1078: //ж
      memcpy_PF(rus_, (const void*)rus_zh, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_zh);
    break;
    case 1079: //з
      memcpy_PF(rus_, (const void*)rus_z, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_z);
    break;
    case 1080: //и
      memcpy_PF(rus_, (const void*)rus_i, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_i);
    break;
    case 1081: //й
      memcpy_PF(rus_, (const void*)rus_ii, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_ii);
    break;
    case 1082: //к
      memcpy_PF(rus_, (const void*)rus_k, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_k);
    break;
    case 1083: //л
      memcpy_PF(rus_, (const void*)rus_l, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_l);
    break;
    case 1084: //м
      memcpy_PF(rus_, (const void*)rus_m, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_m);
    break;
    case 1085: //н
      memcpy_PF(rus_, (const void*)rus_n, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_n);
    break;
    case 1087: //п
      memcpy_PF(rus_, (const void*)rus_p, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_p);
    break;
    case 1090: //т
      memcpy_PF(rus_, (const void*)rus_t, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_t);
    break;
    case 1092: //ф
      memcpy_PF(rus_, (const void*)rus_f, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_f);
    break;
    case 1094: //ц
      memcpy_PF(rus_, (const void*)rus_ts, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_ts);
    break;
    case 1095: //ч
      memcpy_PF(rus_, (const void*)rus_ch, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_ch);
    break;
    case 1096: //ш
      memcpy_PF(rus_, (const void*)rus_sh, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_sh);
    break;
    case 1097: //щ
      memcpy_PF(rus_, (const void*)rus_sch, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_sch);
    break;
    case 1098: //ъ
      memcpy_PF(rus_, (const void*)rus_tverd_mal, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_tverd_mal);
    break;
    case 1099: //ы
      memcpy_PF(rus_, (const void*)rus_y, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_y);
    break;
    case 1100: //ь
      memcpy_PF(rus_, (const void*)rus_myagk_mal, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_myagk_mal);
    break;
    case 1101: //э
      memcpy_PF(rus_, (const void*)rus_ee, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_ee);
    break;
    case 1102: //ю
      memcpy_PF(rus_, (const void*)rus_yu, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_yu);
    break;
    case 1103: //я
      memcpy_PF(rus_, (const void*)rus_ya, 8);
      CharSetToLCD((uint8_t *)rus_, &index_rus_ya);
    break;
    //Русский алфавит, использующий одинаковые с английским алфавитом символы
    case 1040: //А
      LCD_Display::print("A");
    break;
    case 1042: //В
      LCD_Display::print("B");
    break;
    case 1045: //Е
      LCD_Display::print("E");
    break;
    case 1025: //Ё
      LCD_Display::print("E");
    break;
    case 1050: //К
      LCD_Display::print("K");
    break;
    case 1052: //M
      LCD_Display::print("M");
    break;
    case 1053: //H
      LCD_Display::print("H");
    break;
    case 1054: //O
      LCD_Display::print("O");
    break;
    case 1056: //P
      LCD_Display::print("P");
    break;
    case 1057: //C
      LCD_Display::print("C");
    break;
    case 1058: //T
      LCD_Display::print("T");
    break;
    case 1061: //X
      LCD_Display::print("X");
    break;
    case 1072: //а
      LCD_Display::print("a");
    break;
    case 1077: //е
      LCD_Display::print("e");
    break;
    case 1086: //o
      LCD_Display::print("o");
    break;
    case 1088: //p
      LCD_Display::print("p");
    break;
    case 1089: //c
      LCD_Display::print("c");
    break;
    case 1091: //y
      LCD_Display::print("y");
    break;
    case 1093: //x
      LCD_Display::print("x");
    break;
    case 0x00B0: //Знак градуса
      LCD_Display::write(223);
    break;
    //Английский алфавит без изменения
    default:
      LCD_Display::print((char)_chr);
    break;
  }
}


//Перевод символа из кодировки ASCII в UTF-8 (для печати расширенных русских символов на LCD)
wchar_t *LCD_Display::asciiutf8(unsigned char ascii)
{
  if (ascii==168) *char_utf8 = 0x401;//код ASCII буквы Ё
  else if (ascii==184) *char_utf8 = 0x451;//код ASCII буквы ё
  else if ((ascii>=192)&&(ascii<=255))//остальные буквы русского алфавита
  {
    *char_utf8 = ascii+848;
  }
  else *char_utf8 = ascii;

  return char_utf8;
}

**/

