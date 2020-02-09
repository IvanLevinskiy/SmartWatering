#include <Modbus\Modbus.h>

#pragma region  ПРИВАТНЫЕ МЕТОДЫ

bool MODBUS_TCP_SLAVE::_checkMacAddres(byte array[])
{
    bool result = 0;
    for (byte i = 0; i < 6; i++)
    {
        if (array[i] == 255) 
        {
            return 0;
        }

        if (array[i] > 0 ) 
        {
            result = 1;
        }
    }

        return result;
}

void MODBUS_TCP_SLAVE::WIFI_AP_INIT()
{   
    WiFi.mode(WIFI_AP);
    //WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.softAP("Watering");
    
    //if ( ! (_checkMacAddres(MAC))) 
    //{
    //    WiFi.softAPmacAddress(MAC);
    //}
    
    delay(3000);
    MBServer.begin();
}

void MODBUS_TCP_SLAVE::WIFI_STA_INIT()
{    
    //Подключение к сети
    WiFi.begin("MyAsus", "Ae3182126");
      
    //Конфигурация параметров сети модуля
    WiFi.config(ip, gateway, subnet);

    //WiFi.setAutoConnect(true);
    //WiFi.setAutoReconnect(true);
    //WiFi.persistent(false);

    //Ждем 10 секунд, если соединение не произошло
    //выходим из метода
    while(WiFi.status() != WL_CONNECTED) 
    {        
        delay(500);
        Serial.print(".");
    }

    MBServer.begin();
}    

//Управление WiFi адаптером
void MODBUS_TCP_SLAVE::WiFi_CONTROL()
{
    //Если состояние WiFi не изменилось
    //тогда выходим из функции
    if(WiFi_State == WiFi_Old_State)
    {
        return;
    }

    //Если флаг, разрешающий разворачивание WiFi сети
    //изменился на TRUE, тогда запускаем WiFi точку доступа
    if(WiFi_State == true && WiFi_Old_State == false)
    {
      WIFI_AP_INIT();
      WiFi_Old_State = WiFi_State;
      Serial.println("WiFi_State == true");
      return;
    }

    //Если флаг, разрешающий разворачивание WiFi сети
    //изменился на FALSE, тогда закрываем WiFi точку доступа
    if(WiFi_State == false && WiFi_Old_State == true)
    {
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      WiFi_Old_State = WiFi_State;
      Serial.println("WiFi_State == false");
      return;
    }
}

//Функция, котороя обрабатывает запрос от сервера
//на чтение HOLDING регистров
void MODBUS_TCP_SLAVE::READ_HOLDING_REGISTERS()
{
  //************ЗАПРОС
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х03)
  //ByteArray[8] -  адрес первого регистра (старший байт)
  //ByteArray[9] -  адрес первого регистра (младший байт)
  //ByteArray[10] - количество регистров (старший байт)
  //ByteArray[11] - количество регистров (младший байт)

  //Определяем адрес певого регистра, который надо прочиталь
  word FIRST_REGISTER_ADDRESS = word(ByteArray[8],ByteArray[9]);

  //Определяем количество регистров, которые будем читать
  word QUANTITY_REGISTERS = word(ByteArray[10],ByteArray[11]);

  //************ОТВЕТ
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х03)
  //ByteArray[8] -  Количество байт далее
  //ByteArray[9] -  Значение регистра 1 (старший байт)
  //ByteArray[10] - Значение регистра 1 (младший байт)
  //ByteArray[11] - Значение регистра 2 (старший байт)
  //ByteArray[12] - Значение регистра 2 (младший байт)
  //и так далее

  byte BYTE_COUNT = 0;      //Счетчик байтов
  word MESSAGE_LENGTCH = 3; //Длина сообщения

  //Наполняем телеграмму значениями регистров, которые надо прочиталь
  for(int i = 0; i < QUANTITY_REGISTERS; i++) 
  {
     ByteArray[9  + i * 2] = highByte(MB_HOLDING_REGISTERS[FIRST_REGISTER_ADDRESS + i]);
     ByteArray[10 + i * 2] = lowByte(MB_HOLDING_REGISTERS [FIRST_REGISTER_ADDRESS + i]);
     BYTE_COUNT += 2;
  }

   //Считаем, какая длина сообщения получилась
   MESSAGE_LENGTCH += word(0, BYTE_COUNT);

   //Переносим оставшиеся данные в телеграмму
   //Длина телеграммы
   ByteArray[4] = highByte(MESSAGE_LENGTCH);
   ByteArray[5] = lowByte(MESSAGE_LENGTCH);

   //Количество байт за заголовком
   ByteArray[8] = BYTE_COUNT;
    
   client.write((const uint8_t *)ByteArray, 6 + MESSAGE_LENGTCH);
   MB_FUNCTION = MB_FC_NONE;
   
   //Serial.print("SEND: ");

  //Serial.println("");
}

void MODBUS_TCP_SLAVE::WRITE_SINGLE_HOLDING_REGISTER()
{
  //************ЗАПРОС
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х10)
  //ByteArray[8] -  адрес регистра (старший байт)
  //ByteArray[9] -  адрес регистра (младший байт)
  //ByteArray[10] - значение (старший байт)
  //ByteArray[11] - значение (младший байт)

  //Определяем адрес певого регистра, который надо прочиталь
  word REGISTER_ADDRESS = word(ByteArray[8],ByteArray[9]);

  //Записываем принятое значение в регистр
  MB_HOLDING_REGISTERS[REGISTER_ADDRESS] = word(ByteArray[10],ByteArray[11]);

  //************ОТВЕТ
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х10)
  //ByteArray[8] -  Адрес регистра 1 (старший байт)
  //ByteArray[9] -  Адрес регистра 1 (младший байт)
  //ByteArray[10] - Значение (старший байт)
  //ByteArray[11] - Значение (младший байт)
  
  //Отвечаем (телеграмма та же, которую приняли)
  client.write((const uint8_t *)ByteArray, 12);
  MB_FUNCTION = MB_FC_NONE;
}

void MODBUS_TCP_SLAVE::WRITE_MULTI_HOLDING_REGISTER()
{
  //************ЗАПРОС
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х10)
  //ByteArray[8] -  адрес первого регистра (старший байт)
  //ByteArray[9] -  адрес первого регистра (младший байт)
  //ByteArray[10] - количество регистров (старший байт)
  //ByteArray[11] - количество регистров (младший байт)
  //ByteArray[12] - количество байт далее
  //ByteArray[13] - значение 1 (старший байт)
  //ByteArray[14] - значение 1 (младший байт)
  //ByteArray[15] - значение 2 (старший байт)
  //ByteArray[16] - значение 2 (младший байт)

  //Определяем адрес певого регистра, который надо прочиталь
  word FIRST_REGISTER_ADDRESS = word(ByteArray[8],ByteArray[9]);

  //Определяем количество регистров, которые будем читать
  word QUANTITY_REGISTERS = word(ByteArray[10],ByteArray[11]);

  //************ОТВЕТ
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х10)
  //ByteArray[8] -  Адрес регистра 1 (старший байт)
  //ByteArray[9] -  Адрес регистра 1 (младший байт)
  //ByteArray[10] - Количество записаных регистров (старший байт)
  //ByteArray[11] - Количество записаных регистров (младший байт)

  //Устанавливаем длину сообщения, она всегда фиксированая
  ByteArray[4] = 0;
  ByteArray[5] = 6;
  
  //Записываем данные из телеграммы в регистры
  for(int i = 0; i < QUANTITY_REGISTERS; i ++)
  {
      MB_HOLDING_REGISTERS[FIRST_REGISTER_ADDRESS + i] = word(ByteArray[ 13 + i * 2],ByteArray[14 + i * 2]);
  }
  
   client.write((const uint8_t *)ByteArray, 12); 
   MB_FUNCTION = MB_FC_NONE;
}

// Обработка запроса
void MODBUS_TCP_SLAVE::MB_NEW_DATA_PROCESSING()
{
  //Если адрес уствойства не то, тогда 
  //пропускае
  if(ByteArray[MB_TCP_UID] != MB_DEVICE_ID)
  {
     return;
  }
        
    switch(MB_FUNCTION) 
    {
        //Если нет функции
        case MB_FC_NONE:  break;

        //ЧТЕНИЕ НЕСКОЛЬКИХ РЕГИСТРОВ КОД: 0х03
        case MB_FC_READ_REGISTERS: READ_HOLDING_REGISTERS(); break; 

        //ЗАПИСЬ ОДНОГО РЕГИСТРА КОД: 0х06
        case MB_FC_WRITE_REGISTER: WRITE_SINGLE_HOLDING_REGISTER(); break; 
      
        //ЗАПИСЬ НЕСКОЛЬКИХ РЕГИСТРОВ КОД: 0х10
        case MB_FC_WRITE_MULTIPLE_REGISTERS: WRITE_MULTI_HOLDING_REGISTER(); break;
      }
}

//Обновление данных MODBUS
void MODBUS_TCP_SLAVE::MODBUS_UPDATE()
{ 
  //Вызываем метод, осуществляющий 
  //управление подключением по WiFi
  WiFi_CONTROL();
  
  //Если WiFi выключен, выходим из функции
  if(WiFi_State == false)
  {
      return;
  }

  //Если клиента нет, пытаемся его подключить
  if(! client)
  {
      client = MBServer.available();
      delay(100);
      return;   
  }
  
    //Modbus TCP/IP
    //Читаем телеграмму
    if(client.available() > 0) 
    {
       RECIVED_BYTES_LENGTCH = 0;

       while(client.available()) 
       {
         ByteArray[RECIVED_BYTES_LENGTCH] = client.read();
         RECIVED_BYTES_LENGTCH++;
       }
         
        client.flush();

        MB_NEW_DATA_PROCESSING();
    }
}

#pragma endregion

#pragma region  ПУБЛИЧНЫЕ МЕТОДЫ

MODBUS_TCP_SLAVE::MODBUS_TCP_SLAVE()
{

}

#pragma endregion
















