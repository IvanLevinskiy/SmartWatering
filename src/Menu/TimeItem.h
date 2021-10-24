#ifndef TIME_ITEM_H
#define TIME_ITEM_H

#include <LiquidCrystal_I2C.h>
#include "Timers/Timer_SD.h"
#include "Menu/BaseItem.h"

//Класс для редактирования времени
//время задается в минутах
class TimeItem : public BaseItem
{
    private:
        //Временная переменная для
        //хранения результата
        uint16_t temp_value;

    public:

        /**
         *Конструктор
         *header - заголовок экрана
         *object - объект, связанный с текущим экраном
        **/
        TimeItem(char* header, void * object);

        //Метод для отображения
        void Display() override;

        //Переключение шага для редактирования
        //(меняем разряды)
        bool IsEdition() override;

        //Сообщаем модели, что нажата кнопка
        //вверх
        void BtnUpPressed() override;

        //Сообщаем модели, что нажата кнопка
        //вниз
        void BtnDownPressed() override;

        //Сообщаем модели, что нажата кнопка
        //выбора
        void BtnSelectPressed() override;

        //Добавляет к значению 1
        void UpValue();

        //Вычитает из значения 1
        void DownValue();

         //Начало редактирования
        void StartEdit();

        //Конец редактирования
        void EndEdit();      
};

#endif
