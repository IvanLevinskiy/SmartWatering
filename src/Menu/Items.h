#include "Menu/LCD_Display.h"

#ifndef BASE_ITEM_H
#define BASE_ITEM_H

//Базовый (абстрактный класс)
class BaseItem
{
    protected:

        //Заголовк
        char* Header;

        //Указатель на элемент
        void* Pointer;

        //Указатель на локальный таймер
        //для анимации
        Timer_SD* Timer_2Hz;

    public:

        //Метод для отображения связанной переменной
        virtual void Display(LCD_Display* LCD, MenuMode mode, Timer_SD* Timer_2Hz) = 0;

        //Добавляет к значению 1
        virtual void UpValue() = 0;

        //Вычитает из значения 1
        virtual void DownValue() = 0;

        //Добавление значение к связанной величине
        virtual void AddValue(int num) = 0;

        //Вычитание значения из связаной величины
        virtual void SubValue(int num) = 0;
};

#endif

#ifndef TIME_ITEM_H
#define TIME_ITEM_H

//Класс для редактирования времени 
//время задается в минутах
class TimeItem : public BaseItem
{

    private:


    public:

        /**
         *Конструктор
         *header - заголовок экрана
         *object - объект, связанный с текущим экраном
        **/
        TimeItem(char* header, void * object)
        {
            //Передаем в экземпляр класса заголовок, 
            //который отображаться вверху дисплея
            Header = header;

            //Передаем указатель на элемент массива
            //в свойства класса
            Pointer = object;
        }

        //Метод для отображения
        void Display(LCD_Display* LCD, MenuMode mode, Timer_SD* Timer_2Hz) override
        {
            //Обновляем состояние таймера
            Timer_2Hz->Update();

            //Получаем указатель на переменную
            int* pointer = (int*)Pointer;
            int value = *pointer;

            //Печатаем в дисплей то значение, которое планируем изменить
            LCD->printIntAsTime(value);

            if(mode == ReadMode)
            {
                LCD->printHeader(Header);
                
                //Затираем карандаш
                LCD->AnimationStop();
                return;
            }

            if(Timer_2Hz->IsTick())
            {    
                LCD->AnimationEdit();
                return;
            }
        }

        //Добавляет к значению 1
        void UpValue() override
        {
             //Получаем указатель на переменную
            int* pointer = (int*)Pointer;
            int value = *pointer;

            value = value + 1;

            if(value > 1439)
            {
                value = 1439;
            }
            
            *pointer = value;
        }

        //Вычитает из значения 1
        void DownValue() override
        {
            //Получаем указатель на переменную
            int* pointer = (int*)Pointer;
            int value = *pointer;

            value = value -1;

            if(value < 0)
            {
                value = 0;
            }
            
            *pointer = value;
        }

        void AddValue(int num)  override
        {
            //Получаем указатель на переменную
            int* pointer = (int*)Pointer;
            int value = *pointer;

            value = value + num;

            if(value > 1439)
            {
                value = 1439;
            }
            
            *pointer = value;
        }

        void SubValue(int num)  override
        {
            //Получаем указатель на переменную
            int* pointer = (int*)Pointer;
            int value = *pointer;

            value = value - num;

            if(value < 0)
            {
                value = 0;
            }
            
            *pointer = value;
        }
};

#endif

#ifndef BOOL_ITEM_H
#define BOOL_ITEM_H

//Класс для редактирования булевых переменных
class BoolItem : public BaseItem
{

    private:

    public:

        /**
         *Конструктор
         *header - заголовок экрана
         *object - объект, связанный с текущим экраном
        **/
        BoolItem(char* header, void * object)
        {
            //Передаем в экземпляр класса заголовок, 
            //который отображаться вверху дисплея
            Header = header;

            //Передаем указатель на элемент массива
            //в свойства класса
            Pointer = object;
        }

        String BoolToStr(bool val)
        {
            if(val == true) return "  ON";

            return " OFF";
            
        }

        int fl;
        //Метод для отображения
        void Display(LCD_Display* LCD, MenuMode mode, Timer_SD* Timer_2Hz) override
        {
            
            //Обновляем состояние таймера
            Timer_2Hz->Update();

            //Получаем указатель на переменную
            bool* pointer = (bool*)Pointer;
            bool value = *pointer;

            //Печатаем в дисплей то значение, которое планируем изменить
            LCD->setCursor(12, 1);
            LCD->print(BoolToStr(value));

            if(mode == ReadMode)
            {
                LCD->printHeader(Header);

                //Затираем карандаш
                LCD->AnimationStop();
                return;
            }

            //Мигаем
            if(Timer_2Hz->IsTick())
            {    
                LCD->AnimationEdit();
                return;
            }
	            
        }

        //Добавляет к значению 1
        void UpValue() override
        {
            //Получаем указатель на переменную
            bool* pnt = (bool*)Pointer;
            
            //получаем значение из указателя
            bool value = *pnt;

            //Записываем новое значение в память
            *pnt = !value;
        }

        //Вычитает из значения 1
        void DownValue() override
        {
            //Получаем указатель на переменную
            bool* pnt = (bool*)Pointer;
            
            //получаем значение из указателя
            bool value = *pnt;

            //Записываем новое значение в память
            *pnt = !value;
        }

        void AddValue(int num)  override
        {

        }

        void SubValue(int num)  override
        {

        }
       
};

#endif

#ifndef PREVIEW_ITEM_H
#define PREVIEW_ITEM_H

//Класс стартового экрана
class StartScreenItem : public BaseItem
{
    public:

        char* Row_2;

        StartScreenItem(char* header, char* row_2)
        {
            //Передаем в экземпляр класса заголовок, 
            //который отображаться вверху дисплея
            Header = header;
            Row_2 = row_2;
        }

        //Метод для отображения
        void Display(LCD_Display* LCD, MenuMode mode, Timer_SD* Timer_2Hz) override
        {
            LCD->printHeader(Header);
            LCD->setCursor(0,1);
            LCD->print(Row_2);
        }

        //Добавляет к значению 1
        void UpValue() override
        {

        }

        //Вычитает из значения 1
        void DownValue() override
        {

        }

        void AddValue(int num)  override
        {

        }

        void SubValue(int num)  override
        {

        }
};

#endif