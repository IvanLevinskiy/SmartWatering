#ifndef PREVIEW_ITEM_H
#define PREVIEW_ITEM_H

#include <LiquidCrystal_I2C.h>
#include "Timers/Timer_SD.h"
#include "Menu/BaseItem.h"

//Класс стартового экрана
class StartScreenItem : public BaseItem
{
    public:

        char* Row_2;

        StartScreenItem(char* header, char* row_2);

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


};

#endif
