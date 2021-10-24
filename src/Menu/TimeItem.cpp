#include "Menu/TimeItem.h"

/**
 *Конструктор
 header - заголовок экрана
 object - указатель на объект, связанный с текущим экраном
**/
TimeItem::TimeItem(char* header, void * object)
{
    //Передаем в экземпляр класса заголовок,
    //который отображаться вверху дисплея
    Header = header;

    //Передаем указатель на элемент массива
    //в свойства класса
    Pointer = object;
}

//Метод для отображения
void TimeItem::Display()
{
    //Обновляем состояние таймера
    Timer_2Hz->Update();

    //Получаем указатель на переменную
    uint16_t* pointer = (uint16_t*)Pointer;
    uint16_t value = *pointer;

    //Печатаем в дисплей то значение, которое планируем изменить
    if(mode == ReadMode)
    {

        printHeader(Header);


        LCD->print("     ");
        printIntAsTime(value);

        //Затираем карандаш
        AnimationStop();
        return;
    }

    //В режиме редактирования печатаем
    //в дисплей числовое значение
    printIntAsTime(temp_value, Timer_2Hz);

    //Обновляем анимацию
    if(Timer_2Hz->IsTick())
    {
        AnimationEdit();
        return;
    }
}

//Добавляет к значению 1
void TimeItem::UpValue()
{
  int hour = temp_value / 60;
  int minutetes = temp_value-(hour * 60);

    //Если редактируем разряд 0
    if(StepEditor == 0)
    {
      minutetes += 1;

      if(minutetes < 60)
      {
          temp_value = hour * 60 + minutetes;
      }

      return;
    }

    //Если редактируем разряд 1
    if(StepEditor == 1)
    {
      hour += 1;

      if(hour < 24)
      {
          temp_value = hour * 60 + minutetes;
      }

      return;
    }
}

//Вычитает из значения 1
void TimeItem::DownValue()
{
  int hour = temp_value / 60;
  int minutetes = temp_value-(hour * 60);

    //Если редактируем разряд 0
    if(StepEditor == 0)
    {
      minutetes -= 1;

      if(minutetes >= 0)
      {
          temp_value = hour * 60 + minutetes;
      }

      //Переход по указателю для проверки
      //валидности значения времени
      goto M1;

    }

    //Если редактируем разряд 1
    if(StepEditor == 1)
    {
      hour -= 1;

      if(hour >= 0)
      {
          temp_value = hour * 60 + minutetes;
      }

      M1: if(temp_value < 0)
      {
          temp_value = 0;
      }

    }

    return;
}

 //Начало редактирования
void TimeItem::StartEdit()
{
    //Получаем указатель на переменную
    uint16_t* pointer = (uint16_t*)Pointer;

    //Разыменовываем указатель
    temp_value = *pointer;

}

//Конец редактирования
void TimeItem::EndEdit()
{
    //Получаем указатель на переменную
    uint16_t* pointer = (uint16_t*)Pointer;

    //Записываем значение в регистр
    *pointer = temp_value;
}

//Переключение шага для редактирования
//(меняем разряды)
bool TimeItem::IsEdition()
{
    //Инкрементируем шаг
    if(StepEditor == 0)
    {
        //Сообщаем, что редактирование не завершено
        StepEditor ++;
        return true;
    }

    if(StepEditor == 1)
    {
        //Сообщаем, что редактирование не завершено
        StepEditor ++;
        StepEditor = 0;
        return false;
    }

    StepEditor = 0;
    return false;
}

void TimeItem::BtnUpPressed()
{
    if(mode == WriteMode)
    {
        UpValue();
    }
}

void TimeItem::BtnDownPressed()
{
    if(mode == WriteMode)
    {
        DownValue();
    }
}

void TimeItem::BtnSelectPressed()
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
