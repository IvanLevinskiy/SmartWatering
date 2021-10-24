#include "MENU/BaseItem.h"

//Метод для печати заголовка
void BaseItem::printHeader(char * header)
{
  //Осуществляем печать заголовка только в том случае, если
  //он отличается от того, что в уже напечатан
    LCD->home();
    LCD->print(header);
}

//Метод для печати заголовка
void BaseItem::printIntAsTime(int time)
{
    //Затираем лишнюю информацию
    LCD->setCursor(0,1);
    LCD->print("         ");


    int hour = time / 60;
    int minute = time - (hour * 60);

    // Форматирование и отображение времени
    LCD->setCursor(11,1);
    if (hour < 10) LCD->print("0");
    LCD->print(hour); LCD->print(":");
    if (minute < 10) LCD->print("0");
    LCD->print(minute);
}

//Печать времени
void BaseItem::printIntAsTime(int value, Timer_SD* Timer_2Hz)
{
      //Затираем лишнюю информацию
      LCD->setCursor(0,1);
      LCD->print("         ");

      Timer_2Hz->Update();

      int hour = value / 60;
      int minute = value-(hour * 60);

      //Форматирование и отображение времени
        LCD->setCursor(11,1);

        //Моргаем минутами
         if(StepEditor == 0)
        {
             if (hour < 10)
                {
                    LCD->print("0");
                }

                LCD->print(hour);
                LCD->print(":");

            if(Timer_2Hz->GetBlinkerState() )
            {
                 if (minute < 10)
                {
                    LCD->print("0");
                }
                LCD->print(minute);
            }
            else
            {
                LCD->print("  ");
            }

            return;
        }

        //Моргаем часами
        if(StepEditor == 1)
        {
            if(Timer_2Hz->GetBlinkerState() )
            {
                if (hour < 10)
                {
                    LCD->print("0");
                }

                LCD->print(hour);
                LCD->print(":");
            }
            else
            {
                LCD->print("  :");
            }

            if (minute < 10)
            {
                LCD->print("0");
            }
            LCD->print(minute);

            return;
        }
}

/*
    МЕТОД ДЛЯ ПРИВЯЗКИ ДИСПЛЕЯ
*/
void BaseItem::BindingLCD(LiquidCrystal_I2C* lcd)
{
    this->LCD = lcd;
}

/*
    МЕТОД ДЛЯ ПРИВЯЗКИ ТАЙМЕРА
*/
void BaseItem::BindingTimer(Timer_SD* timer)
{
    this->Timer_2Hz = timer;
}

//Анимация карандаша
void BaseItem::AnimationEdit()
{
  switch (AnimationStep)
  {
      case 0: LCD->setCursor(0,1);
              LCD->print(char(0));
              break;

      case 1: LCD->setCursor(0,1);
              LCD->print("_");
              LCD->print(char(0));
              break;

      case 2: LCD->setCursor(0,1);
              LCD->print("__");
              LCD->print(char(0));
              break;

      case 3: LCD->setCursor(0,1);
              LCD->print("      ");
              AnimationStep = 0;
              break;
  }

    AnimationStep ++;
}

//стоп анимации
void BaseItem::AnimationStop()
{
   if(AnimationStep != 0)
    {
        AnimationStep = 0;
        LCD->setCursor(0,1);
        LCD->print("     ");
    }
}

//Добавление нового символа
//к списку пользовательских символов
void BaseItem::AddSymbol(byte* symbol)
{
    SymbolsList[SymbolCounter] = symbol;
    SymbolCounter++;
}

//Загрузка пользовательских символов
//в память дисплея
void BaseItem::LoadSymbols(int offset = 3)
{
    //Если символов в памяти нет,
    //погда пропускаем дальнейший когд
    if(SymbolCounter == 0)
    {
        return;
    }

    //Загружаем символы в дисплей
    for(int i = 0; i < SymbolCounter; i ++)
    {
        LCD->createChar(i + offset, SymbolsList[i]);
    }
}
