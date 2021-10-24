#include "MENU/SystemTimeItem.h"

//Конструктор класса
SystemTimeItem::SystemTimeItem(char* header, RTC_DS3231* rtc)
{
    //Передаем в экземпляр класса заголовок,
    //который отображаться вверху дисплея
    Header = header;

    //Передача указателя на RTC
    RTC = rtc;

    //Сохраняем дисплей
    this->LCD = LCD;
}

//Метод для отображения
void SystemTimeItem::Display()
{
    //Обновление таймера
    Timer_2Hz->Update();

    printHeader(Header);
    LCD->setCursor(8,1);

    if(mode == ReadMode)
    {
      DisplayOfReadMode(Timer_2Hz);
      return;
    }

    if(mode == WriteMode)
    {
      DisplayOfWriteMode(Timer_2Hz);
      return;
    }
}

//Метод для отображения системного времени в режиме чтения
void SystemTimeItem::DisplayOfReadMode(Timer_SD* Timer_2Hz)
{
    //22.02.20 15:00:01
    DateTime now = RTC->now();

    if(Timer_2Hz->GetBlinkerState())
    {
        printNum(now.hour());
        LCD->print(':');
        printNum(now.minute());
        LCD->print(':');
        printNum(now.second());
        return;
    }

    printNum(now.hour());
    LCD->print(' ');
    printNum(now.minute());
    LCD->print(' ');
    printNum(now.second());
}

void SystemTimeItem::DisplayOfWriteMode(Timer_SD* Timer_2Hz)
{
  //Если редактируются секунды
  if(StepEditor == 0)
  {
    printNum(hours);
    LCD->print(':');
    printNum(minutetes);
    LCD->print(':');

    if(Timer_2Hz->GetBlinkerState())
    {
        printNum(seconds);
        return;
    }
    else
    {
      LCD->print("  ");
    }

    return;
  }

  if(StepEditor == 1)
  {
    printNum(hours);
    LCD->print(':');

    if(Timer_2Hz->GetBlinkerState())
    {
        printNum(minutetes);
        return;
    }
    else
    {
      LCD->print("  ");
    }

    LCD->print(':');
    printNum(seconds);

    return;
  }

  if(StepEditor == 2)
  {
    if(Timer_2Hz->GetBlinkerState())
    {
        printNum(hours);
        return;
    }
    else
    {
      LCD->print("  ");
    }

    LCD->print(':');

    printNum(minutetes);

    LCD->print(':');
    printNum(seconds);

    return;
  }

}

//Добавляет к значению 1
void SystemTimeItem::UpValue()
{
    if(StepEditor == 0)
    {
        if(seconds < 59)
        {
          seconds ++;
        }

        return;
    }

    if(StepEditor == 1)
    {
        if(minutetes < 59)
        {
          minutetes ++;
        }

        return;
    }

    if(StepEditor == 2)
    {
        if(hours < 23)
        {
          hours ++;
        }

        return;
    }
}

//Вычитает из значения 1
void SystemTimeItem::DownValue()
{
  if(StepEditor == 0)
  {
      if(seconds > 0)
      {
        seconds --;
      }

      return;
  }

  if(StepEditor == 1)
  {
      if(minutetes > 0)
      {
        minutetes --;
      }

      return;
  }

  if(StepEditor == 2)
  {
      if(hours > 0)
      {
        hours --;
      }

      return;
  }
}

//Начало редактирования
void SystemTimeItem::StartEdit()
{
        DateTime CurrentDateTime = RTC->now();
        hours = CurrentDateTime.hour();
        minutetes = CurrentDateTime.minute();
        seconds = CurrentDateTime.second();
}

//Конец редактирования
void SystemTimeItem::EndEdit()
{
    SetTime();
}

//Переключение шага для редактирования
//(меняем разряды)
bool SystemTimeItem::IsEdition()
{
  //Редактирование секунд
  if(StepEditor == 0)
  {
      //Сообщаем, что редактирование не завершено
      StepEditor = 1;
      return true;
  }

  //Редактирование минут
  if(StepEditor == 1)
  {
      //Сообщаем, что редактирование не завершено
      StepEditor = 2;
      return true;
  }

  //Редактирование часов
  if(StepEditor == 2)
  {
      //Сообщаем, что редактирование не завершено
      StepEditor = 0;
      return false;
  }
}


char* SystemTimeItem::printNum(int num)
{
    int reg_1 = num / 10;
    int reg_2 = num % 10;

    LCD->print(reg_1, DEC);
    LCD->print(reg_2, DEC);
}

//Метод для установки времени
void SystemTimeItem::SetTime()
{
    //Получаем текущее время
    DateTime CurrentDateTime = RTC->now();
    DateTime NewDateTime =  DateTime( CurrentDateTime.year(), CurrentDateTime.month(), CurrentDateTime.day(),
                                      hours, minutetes, seconds);

  RTC->adjust(NewDateTime);
}

void SystemTimeItem::BtnUpPressed()
{
    if(mode == WriteMode)
    {
        UpValue();
    }
}

void SystemTimeItem::BtnDownPressed()
{
    if(mode == WriteMode)
    {
        DownValue();
    }
}

void SystemTimeItem::BtnSelectPressed()
{
      if(mode == ReadMode)
      {
          //Подготовка к редактированию
          //параметров
          StartEdit();

          //Сохранение режима меню
          mode = WriteMode;
          return;
      }

      if(IsEdition() == false)
      {
          //Процедура завершения
          //редактирования переменной
          EndEdit();

          //Сохранение режима меню
          mode = ReadMode;
      }
}
