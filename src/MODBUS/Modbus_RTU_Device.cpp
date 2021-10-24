#include"Modbus_RTU_Device.h"

MODBUS_RTU_DEVICE::MODBUS_RTU_DEVICE(int id, Stream* serialPort, int de_re)
{
  ID = id;
  DE_RE = de_re;
  SerialPort = serialPort;
  pinMode(DE_RE, OUTPUT);

  timer_reset_dir_pin.Start();
}

//Метод для отправки сообщения
void MODBUS_RTU_DEVICE::Send(unsigned char* data, unsigned char length)
{
    digitalWrite(DE_RE, HIGH);
    SerialPort->write(data, length);

    //Сброс таймера для установки пина на прием
    timer_reset_dir_pin.Reset();
}

void MODBUS_RTU_DEVICE::sendPacket(unsigned char* data, unsigned char length)
{
	   digitalWrite(DE_RE, HIGH);

     SerialPort->write(data, length);

     //for(int i = 0; i < length; i++)
     //{
     //    delayMicroseconds(1041);
     //}

	   SerialPort->flush();

	   digitalWrite(DE_RE, LOW);

	   //delayStart = millis(); // start the timeout delay
}

//Метод для чтения холдинг регистра
uint16_t MODBUS_RTU_DEVICE::ReadHoldingRegistr(uint16_t Registr)
{
  ByteArray[0] = ID;    //Адрес устройства
  ByteArray[1] = 0x03;  //Код функции
  ByteArray[2] = highByte(Registr);
  ByteArray[3] = lowByte(Registr);
  ByteArray[4] = 0x00;  //Количество регистров
  ByteArray[5] = 0x01;  //Количество регистров

  crc = CRC(ByteArray, 6);

  ByteArray[6] = highByte(crc);
  ByteArray[7] = lowByte(crc);

  //Отправка сообщения
  //Send(ByteArray, 8);
  sendPacket(ByteArray, 8);


  //delay(10);
  for(int i = 0; i < 100; i ++)
  {
      if(SerialPort->available())
      {
        RECIVED_BYTES_LENGTCH = 0;

        while (SerialPort->available())
        {
            ByteArray[RECIVED_BYTES_LENGTCH] = SerialPort->read();
            RECIVED_BYTES_LENGTCH ++;
        }

        return (ByteArray[3] << 8) + ByteArray[4];
      }

      delay(1);
  }

  return 999;
}

//Метод для записи значения в холдинг регистр
void MODBUS_RTU_DEVICE::WriteHoldingRegistr(int Registr, uint16_t value)
{
  ByteArray[0] = ID;    //Адрес устройства
  ByteArray[1] = 0x10;  //Код функции
  ByteArray[2] = highByte(Registr); //Адрес регистра highByte
  ByteArray[3] = lowByte( Registr);  //Адрес регистра lowByte
  ByteArray[4] = 0x00; //Количество регистров highByt
  ByteArray[5] = 0x01; //Количество регистров lowByte
  ByteArray[6] = 0x02; //Количество байт далее
  ByteArray[7] = highByte(value); //Значение highByt
  ByteArray[8] = lowByte(value);  //Значение lowByte

  crc = CRC(ByteArray, 9);

  ByteArray[9] =  highByte(crc);
  ByteArray[10] = lowByte(crc);

  //Отправка сообщения
  //Отправка сообщения
  //Send(ByteArray, 8);
  sendPacket(ByteArray, 11);


  //delay(10);
  for(int i = 0; i < 100; i ++)
  {
    if(SerialPort->available())
    {
        return;
    }

    delay(1);
  }
}

//Вычисление контрольной суммы
uint16_t MODBUS_RTU_DEVICE::CRC(unsigned char* data, unsigned char length)
{
    unsigned int temp, temp2, flag;
    temp = 0xFFFF;
    for (unsigned char i = 0; i < length; i++)
    {
        temp = temp ^ *data++;
        for (unsigned char j = 1; j <= 8; j++)
        {
            flag = temp & 0x0001;
            temp >>= 1;
        if (flag)   temp ^= 0xA001;
    }
  }
  temp2 = temp >> 8;
  temp = (temp << 8) | temp2;
  temp &= 0xFFFF;
  return temp;
}

//Программный таймер
bool  MODBUS_RTU_DEVICE::_isTimer(unsigned long period)
{
    currentTime  = millis();

    if (currentTime >= LastTime)
    {

        return (currentTime >= (LastTime + period));
    }
    else
    {
        return(currentTime >=(4294967295-LastTime + period));
    }
  }

void MODBUS_RTU_DEVICE::Update()
{
      //Обновление таймера установки пина на пердачу RS485
      timer_reset_dir_pin.Update();


      if(_isTimer(500) == true)
      {

        if(Task == 0)
        {
            //Сбрасываем состояние реле
            Value = 0;

            //Составляем слово управления
            Value |= (uint16_t)((int)RELAY_0 << 0);
            Value |= (uint16_t)((int)RELAY_1 << 1);
            Value |= (uint16_t)((int)RELAY_2 << 2);
            Value |= (uint16_t)((int)RELAY_3 << 3);

            WriteHoldingRegistr(0, Value);
        }

        if(Task == 1)
        {
            Temperature = ReadHoldingRegistr(0);
        }

        LastTime = millis();

        Task ++;

        if(Task == 2)
        {
            Task = 0;
        }
      }

      //Устанавливаем пин направления према-передачи RS485
      digitalWrite(DE_RE, !timer_reset_dir_pin.GetOutputState());
}
