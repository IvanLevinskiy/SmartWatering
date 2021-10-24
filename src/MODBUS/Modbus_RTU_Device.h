#ifndef MODBUS_RTU_DEVICE_H
#define MODBUS_RTU_DEVICE_H

#include<Arduino.h>
#include"Timers/Timer_SD.h"

class MODBUS_RTU_DEVICE
{
    public:

      //Конструктор класса
      MODBUS_RTU_DEVICE(int id, Stream* serialPort, int de_re);

      //Метод для чтения холдинг регистра
      uint16_t ReadHoldingRegistr(uint16_t Registr);

      //Метод для записи значения в холдинг регистр
      void WriteHoldingRegistr(int Registr, uint16_t Value);

      //Прототип функции для обновления
      void Update();

      //Переменные для управления реле
      bool RELAY_0 = false;
      bool RELAY_1 = false;
      bool RELAY_2 = false;
      bool RELAY_3 = false;

      //Слово управления
      uint16_t Value = 0;

      uint16_t Temperature = 0;

      int Task = 0;


    private:

      //Адрес устройства
      int ID;

      //Буфер байтов
      byte ByteArray[32];

      //Указатель на порт
      Stream* SerialPort;

      //Последнее время сессии
      unsigned long LastTime;

      //Переменная для временного хранения
      //контрольной суммы
      uint16_t crc;

      //DE_RE pin
      int DE_RE;

      //Таймер для сброса пина DIR
      Timer_SD timer_reset_dir_pin = Timer_SD(50);

      //Длина принятых данных
      int RECIVED_BYTES_LENGTCH = 0;

      int couter = 0;

      //Программный таймер
      bool _isTimer(unsigned long period);

      unsigned long currentTime = 0;

      unsigned long T1_5 = 16500000/9600;

      unsigned long frameDelay = 0;

      //Метод для вычисления контрольной суммы
      uint16_t CRC(unsigned char* data, unsigned char length);

      //Метод для отправки сообщения
      void Send(unsigned char* data, unsigned char length);

      void sendPacket(unsigned char* data, unsigned char length);

      int counter = 0;
};

#endif
