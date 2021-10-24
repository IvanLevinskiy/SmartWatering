#include "Menu/NumericViewItem.h"

/**
 *Конструктор
 *header - заголовок экрана
 *object - объект, связанный с текущим экраном
**/
NumericViewItem::NumericViewItem(char* header, void * object)
{
    //Передаем в экземпляр класса заголовок,
    //который отображаться вверху дисплея
    Header = header;

    //Передаем указатель на элемент массива
    //в свойства класса
    Pointer = object;
}

//Метод для отображения
void NumericViewItem::Display()
{
    //Обновляем состояние таймера
    Timer_2Hz->Update();

    //Вывод заголовка
    printHeader(Header);

    //Получаем указатель на переменную
    int* pointer = (int*)Pointer;
    int value = *pointer;

    //Печатаем в дисплей то значение, которое планируем изменить
    if(mode == ReadMode)
    {
        //Преобразование значения
        //к строковому виду
        bottomRow = String(value);

        //Вывод текста в строку
        printBottomRow();

        //Затираем карандаш
        AnimationStop();

        //Выход из функции
        return;
    }

    //В режиме редактирования печатаем
    //в дисплей числовое значение
    if(Timer_2Hz->GetBlinkerState() == true)
    {
        //получение пустой строки
        bottomRow = "        ";

        //Вывод текста в строку
        printBottomRow();
    }
    else
    {

        //Преобразование значения
        //к строковому виду
        bottomRow = String(temp_value);

        //Вывод текста в строку
        printBottomRow();
    }

    //Обновляем анимацию
    if(Timer_2Hz->IsTick())
    {
        AnimationEdit();
        return;
    }
}

//Добавляет к значению 1
void NumericViewItem::UpValue()
{

  if(temp_value < Max)
  {
        temp_value += 1;
  }
}

//Вычитает из значения 1
void NumericViewItem::DownValue()
{
    if(temp_value > Min)
    {
        temp_value -= 1;
    }
}

 //Начало редактирования
void NumericViewItem::StartEdit()
{
    //Получаем указатель на переменную
    int* pointer = (int*)Pointer;

    //Разыменовываем указатель
    temp_value = *pointer;

}

//Конец редактирования
void NumericViewItem::EndEdit()
{
    //Получаем указатель на переменную
    int* pointer = (int*)Pointer;

    //Записываем значение в регистр
    *pointer = temp_value;
}

//Переключение шага для редактирования
//(меняем разряды)
bool NumericViewItem::IsEdition()
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
        return true;
    }
}

void NumericViewItem::BtnUpPressed()
{
    if(mode == WriteMode)
    {
        UpValue();
    }
}

void NumericViewItem::BtnDownPressed()
{
    if(mode == WriteMode)
    {
        DownValue();
    }
}

void NumericViewItem::BtnSelectPressed()
{
      //Если разрешение на редактирование нет
      //тогда пропускаем ход
      if(ReadOnly == true)
      {
          //Признак, что параметр только для чтения
          LCD->setCursor(0, 1);
          LCD->print("R");
          return;
      }

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
