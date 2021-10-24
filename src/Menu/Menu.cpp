#include "Menu/Menu.h"

/**
 * Перегруженный конструктор класса
 *
 * @param LCD_ADDR	I2C сетевой адрес устройства
 * @param pinUp 	Пин, к которму подключена кнопка вверх
 * @param pinDown	Пин, к которму подключена кнопка вниз
 * @param pinSelect	Пин, к которму подключена кнопка выбора
 */
 Menu::Menu(uint8_t LCD_ADDR)
{
    //Инициализируем дисплей
    LCD = new LiquidCrystal_I2C(LCD_ADDR, 16, 2);
}

Menu::Menu(LiquidCrystal_I2C* lcd)
{
   //Инициализируем дисплей
   LCD = lcd;
}

/**
 * Метод для настройки меню для работы
 * (должен вызываться в setup)
 */
void Menu::Begin()
{
    //Инициализируем дисплей
    LCD->init();

    //Инициализируем дисплей
    LCD->backlight();

    //Запускаем вспомогательный таймер, который
    //впоследствии передаем в каждый Item для анимации
    Timer_2Hz.Start();

    //Запускаем таймер, отслеживающий бездействие
    Timer.Start();
}

void Menu::createSymbol(int pos, byte symbol [])
{
    LCD->createChar(pos, symbol);
}

//Привязка часов реального времени к меню
void Menu::BindingRTC(RTC_DS3231* rtc)
{
    RTC = rtc;
}

/**
 * Метод для добавления нового экрана
 */
void Menu::AddScreen(ContentItem *screen)
{
    //Если превышено максимальное количество
    //пунктов меню, тогда пропускаем дальнейшие шаги
    if(ScreenCount == MAX_SCEEN)
    {
        return;
    }

    //Передаем указатель на таймер
    //для эффектов
    screen->Timer_2Hz = &Timer_2Hz;

    //Экземпляр дисплея
    screen->BindingLCD(LCD);

    //Присваиваем индикатор строки в дисплее
    screen->Row = ScreenCount;

    //Добавляем указатель на экран
    Array[ScreenCount] = screen;

    //Инкрементируем счетчик экранов
    ScreenCount++;
}

/*
    Метод для отмены сна (сброса таймера сна)
*/
void Menu::SleepCancel()
{
        //Сбрасываем таймер, который отслеживает
        //бездействие
        Timer.Reset();

        //Включаем подсветку
        LCD->backlight();
}

//Метод, который генерируется когда
//нажимается кнопка вверх
void Menu::UpCMD()
{
    //Сбрасываем таймер сна
    SleepCancel();

    //Если экран находится в режиме просмотра
    //содержимого передаем команду
    //активному экрану
    if(Array[ActiveScreen]->ContentView == true)
    {
        Array[ActiveScreen]->BtnUpPressed();
        return;
    }

    //Очищаем дисплей
    LCD->clear();


    if(ActiveScreen < ScreenCount - 1)
    {
        ActiveScreen ++;

        //Если индикатор текущей строки больше 3
        //тогда каждому индикатору строки присваиваем - 1
        if(Array[ActiveScreen]->Row > 1)
        {
            for(int i = 0; i < ScreenCount; i ++)
            {
                Array[i]->Row --;
            }
        }
    }

}

//Метод, который генерируется когда
//нажимается кнопка вниз
void Menu::DownCMD()
{
    //Сбрасываем таймер сна
    SleepCancel();

    //Если экран находится в режиме просмотра
    //содержимого передаем команду приемущественно
    if(Array[ActiveScreen]->ContentView == true)
    {
        Array[ActiveScreen]->BtnDownPressed();
        return;
    }

    //Иначе переключаем выбраный объект
    if(ActiveScreen > 0)
    {
        ActiveScreen --;
    }

    //Очищаем дисплей
    LCD->clear();

    //Если индикатор текущей строки больше 3
    //тогда каждому индикатору строки присваиваем + 1
    if(Array[ActiveScreen]->Row < 0)
    {
        for(int i = 0; i < ScreenCount; i ++)
        {
            Array[i]->Row ++;
        }
    }
}

//Метод, который генерируется когда
//нажимается кнопка выбора
void Menu::SelectCMD()
{
    //Сбрасываем таймер сна
    SleepCancel();

    //Передаем команду содержимому экрана
    if(Array[ActiveScreen] != 0)
    {
        Array[ActiveScreen]->BtnSelectPressed();
    }

}

void Menu::Update()
{
  //Если сработал таймер, тогда выключаем подсветку
  Timer.Update();

  if(Timer.IsTick() == true)
  {
      //Выключаем подсветку
      LCD->noBacklight();

      //Если текущий экран это стартровый экран,
      //тогда выходим из функции
      //if( ActiveScreen == 0)
      //{
          return;
      //}

      //Возвращаемся в режим редактирования
      //Mode = ReadMode;

      //Очищаем дисплей
      //LCD->clear();

      //Устанавливаем в качестве начального
      //экрана экран с индексом 0
      //ActiveScreen = 0;
  }
    //Serial.println(ActiveScreen);

    //Если находимся в режиме просмотра содежимого
    if(Array[ActiveScreen]->ContentView == true)
    {
        Array[ActiveScreen]->DisplayContent();
        return;
    }



    //Проходим по всем пунктам
    //и выводим только те,которые
    //со строковым номером 0 и 1
    for(int i = 0; i < ScreenCount; i ++)
    {
      if(Array[i]->Row == 0 || Array[i]->Row == 1)
      {
          Array[i]->Display();
      }
    }

    //Костыль для отображения стрелки
    //LCD->setCursor(0, Array[ActiveScreen]->Row - 1);
    //LCD->print(" ");

    LCD->setCursor(0, Array[ActiveScreen]->Row);
    LCD->print("\1");
}
