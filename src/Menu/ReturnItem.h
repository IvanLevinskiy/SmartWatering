#ifndef RETURN_ITEM_H
#define RETURN_ITEM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <LiquidCrystal_I2C.h>
#include "Timers/Timer_SD.h"
#include "Menu/BaseItem.h"
#include "Menu/ContentItem.h"

//Класс для редактирования булевых переменных
class ReturnItem : public BaseItem
{

    private:


    public:

        /**
         *Конструктор
         *header - заголовок экрана
         *object - объект, связанный с текущим экраном
        **/
        ReturnItem(void * object);

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
