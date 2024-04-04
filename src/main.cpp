// // Подключаем библиотеки
// #include <WiFi.h>
// #include <WebServer.h>
// #include <ESPmDNS.h>
// #include <SPIFFS.h>

// // Определяем интервал записи в файл
// const unsigned long INTERVAL = 10000;
// unsigned long write_millis = 0;

// // Определяем название и пароль точки доступа
// const char* ssid = "esp32"; // Это поле нужно заполнить необходимыми данными
// const char* password = "12345687"; // Это поле нужно заполнить необходимыми данными

// // Определяем имя сервера в сети
// const char* host = "esp32Serv";

// // Создаём объект Веб-сервера
// WebServer server(80);
// WiFiServer

// // Функция вывода списка файлов в HTML страницу
// void listRootToHtml()
// {
//     // Открываем корневой файл
//     File root = SPIFFS.open("/");

//     // Если не получилось открыть
//     if (!root) {
//         // Выводим ошибку
//         Serial.println("error");
//         // Возвращаемся из функции
//         return;
//     }

//     // Создаём строку для конкатенации
//     String html = "<html><meta charset=\"UTF-8\"><body>";

//     // Если корневой файл - каталог
//     if (root.isDirectory()) {

//         // Открываем следующий файл
//         File file = root.openNextFile();

//         // Входим в цикл, если файл существует
//         /* После прохода данного цикла должна
//          * получиться страница вида:
//          * <html><meta charset="UTF-8"><body>
//          * <p><a href="имя_первого_файла download="имя_первого_файла">
//          * имя_первого_файла</a></p>
//          * <p><a href="имя_второго_файла download="имя_второго_файла">
//          * имя_второго_файла</a></p>
//          * .....
//          * <p><a href="имя_N-фала download="имя_N-файла">
//          * имя_N-файла</a></p>
//          * </body></html>
//          */
//         while (file) {

//             // Записываем имя файла
//             String name = file.name();

//             // Удаляем первый символ ("/");
//             name.remove(0, 1);

//             // Конкатенируем строку
//             html += "<p><a href=\"";
//             html += name;
//             html += (String)"\" download=\"";
//             html += name +"\">";
//             html += name;
//             html += "</a></p>";

//             // Открываем следующий файл
//             file = root.openNextFile();
//         }
//         html += "</body></html>";
//     }

//     // Отравляем страницу клиенту
//     server.send(200, "text/html", html);
// }

// // Функция обработки клиента
// void handleClient()
// {
//     // Выводим содержимое корневого каталога как Веб-страницу
//     listRootToHtml();
// }

// // Функция чтения файла
// bool handleFileRead(String path)
// {
//     // Открываем файл для чтения по указанному пути
//     File file = SPIFFS.open(path, "r");

//     // Если файл не удалось открыть
//     if (!file) {
//         Serial.println("Ошибка. Файл не существует.");

//         // Отвечаем клиенту ошибкой
//         server.send(404, "text/plain", "FileNotFound");

//         // Возвращаемся из функции
//         return false;
//     }

//     // Отдаём клиенту содержимое файла
//     server.streamFile(file, "text/html");
//     // Закрываем файл
//     file.close();
//     // Возвращаемся из функции
//     return true;
// }

// void setup() {

//     // Форматируем внутренную память (нужно выполнить только один раз)
//     // SPIFFS.format();

//     // Инициируем объект файловой системы
//     SPIFFS.begin();

//     Serial.begin(115200);
//     Serial.println();
//     Serial.println("Подключаемся к WiFi");

//     // Инициируем точку доступа WiFi
//     WiFi.softAP(ssid, password);
//     // while (WiFi.status() != WL_CONNECTED) {
//     //     Serial.print('.');
//     //     delay(100);
//     // }
//     // Записываем IP-адрес
//     IPAddress myIP = WiFi.localIP();

//     // Инициируем Multicast DNS
//     MDNS.begin(host);

//     // Выводим IP-адрес Веб-сервера
//     Serial.print("IP-адрес: ");
//     Serial.println(myIP);

//     // Функция, выполняемая при подключении к клиента корневому каталогу
//     server.on("/",  HTTP_GET, handleClient);
//     // Функция, которая будет выполнена, если файл не найден
//     server.onNotFound([]() {
//         // Если по указанному пути файл не найден...
//         if (!handleFileRead(server.uri())) {
//             // Отвечаем клиенту ошибкой 404
//             server.send(404, "text/plain", "FILE NOT FOUND");
//         }
//     });

//     // Инициируем сервер
//     server.begin();
//     Serial.println("Сервер запущен.");
// }

// void loop() {
//     // Библиотечная функция обработки клиента
//     server.handleClient();

//     // Если прошёл интервал ожидания
//     if (millis() - write_millis > INTERVAL) {
//         // Записываем текущие millis
//         write_millis = millis();
//         // Создаём файл с абсолютным путём
//         File file = SPIFFS.open("/logger.txt", "w");

//         // Если файл не получилось создать
//         if (!file)
//             // Выводим сообщение
//             Serial.println("error creating file");
//         // Иначе
//         else
//             // Выводим текущие millis в файл
//             file.println(write_millis);
//             file.flush();
//             file.close();
//     }
//     // Даём процессору переключиться на другие задачи
//     delay(20);
// }

#include <WiFi.h>
#include <WebServer.h>
#include "SpiffsMinimal.h"

enum DT_AppStatus : int8_t
{
	GOOD_CODE,
	BAD_CODE,
	DROP_TEST_END,
};

enum DT_AppTask : int8_t
{
	WAIT,
	SITE,
	CALIBRATION,
	DROP_TASK,
};

struct DT_LedColor
{
private:
	int8_t
		pinR = -1,
		pinG = -1,
		pinB = -1;
	uint8_t
		R = 0,
		G = 0,
		B = 0;

public:
	void begin(int8_t pinR, int8_t pinG, int8_t pinB)
	{
		this->pinR = pinR;
		this->pinG = pinG;
		this->pinB = pinB;
		pinMode(pinR, OUTPUT);
		pinMode(pinG, OUTPUT);
		pinMode(pinB, OUTPUT);
	}

	void SetColorByCode(DT_AppStatus &code)
	{
		Serial.print(pinR);
		Serial.print(pinG);
		Serial.print(pinB);
		Serial.println();
		switch (code)
		{
		case DT_AppStatus::GOOD_CODE:
			G = 255;
			B = 0;
			R = 0;
			break;
		case DT_AppStatus::BAD_CODE:
			G = 0;
			B = 0;
			R = 255;
			break;
		case DT_AppStatus::DROP_TEST_END:
			G = 0;
			B = 255;
			R = 0;
			break;
		default:
			G = 0;
			B = 0;
			R = 255;
			break;
		}
		this->Write();
		if (code == DT_AppStatus::GOOD_CODE)
		{
			delay(3000);
			this->Clear();
			/* code */
		}
		
	}

	void Write()
	{
		analogWrite(this->pinR, R);
		analogWrite(this->pinG, G);
		analogWrite(this->pinB, B);
	}

	void Clear()
	{
		R = 0;
		G = 0;
		B = 0;
		this->Write();
	}
};

///================== Global section =====================

#pragma region WIFI
const char *ssid = "ESP32";
const char *password = "12345678";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);
#pragma endregion
#pragma region MPU
#pragma endregion
#pragma region PINS
static constexpr int8_t LEDR = 2;
static constexpr int8_t LEDG = 4;
static constexpr int8_t LEDB = 16;
#pragma endregion
#pragma region err
DT_AppStatus stts = DT_AppStatus::GOOD_CODE;
DT_LedColor ledCtrl;
#pragma endregion
DT_AppTask apTask = DT_AppTask::SITE;

bool isCalibration = false;
bool isTest = false;
static volatile int8_t testNo = 0;
///================== Global section =====================

#pragma region SITE_FOO
String SendMainHTML(DT_AppStatus &stts)
{
	File root = SPIFFS.open("/");
	if (!root)
	{
		stts = DT_AppStatus::BAD_CODE;
		Serial.println("error");
		// Возвращаемся из функции
		// return;
	}
	String ptr = R"rawliteral(
 <!DOCTYPE html>
<html>
<head>
  <title>Drop Test by OT</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
<style>html {font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;}
.esp_ctrl_wrapper{display: flex;flex-direction: column;align-items: center;justify-content: center;margin: 5px 0;}
.esp_bt{background-color: aqua;border-radius: 10px;width: 80%;padding: 5px 0;border: solid 1px black;text-decoration: none;color: black;margin: 0;display: flex;justify-content: center;align-items: center;}
.esp_state {width: 20px;height: 20px;border-radius: 10px;margin: 5px 10px;}
.es_red {background-color: red;}
.es_green {background-color: green;}
.esp_ctrl{margin: 5px;}
.esp_ctrl_el {display: flex;justify-content: center;align-items: center;}
.file_wrapper_main {flex-direction: column;align-items: center;width: 100%;height: auto;display: flex;justify-content: center;}
.file_list {border-radius: 3px;border-width: 1px;border-color: #f44336;background-color: #86b0fd;display: flex;flex-direction: column;align-items: center;width: 80%;height: auto;min-height: 20%;}
.file_item {display: flex;background-color: aliceblue;flex-wrap: wrap;width: 90%;margin: 10px 0;padding: 4px 2px;justify-content: space-evenly;border-radius: 10px;border: solid;}
.file_text_area {display: flex;margin: 0 10px;size: 15px;color: black;}
.file_wrapper {display: flex;}
.res_wrapper{margin: 5px 0;display: flex;justify-content: center;}
</style>
</head>
<body>
  <h1>Drop Test</h1>
  <!-- ESP CTRL -->
  <div class="esp_ctrl_wrapper">
    <div class="esp_ctrl_el">
      <h1 class="esp_ctrl"> ESP Control</h1>
      <div class="esp_state   )rawliteral";
	if (stts != DT_AppStatus::BAD_CODE)
	{
		ptr += "es_green";
	}
	else
	{
		ptr += "es_red";
	}
	ptr += R"rawliteral( "> </div>
    </div>
    <a class="esp_ctrl esp_bt" href="Stest">Start Test</a>
    <a class="esp_ctrl esp_bt" href="call">Calibration MPU</a>
  </div>
  <!-- FILE LIST -->
  <div class="file_wrapper_main">
    <div class="res_wrapper">
      <h1 class="file_text_area">Results</h1>
      <a href="fileClear" class="esp_bt">Clear</a>
    </div> )rawliteral";

	File file = root.openNextFile();
	/// FILE LIST
	while (file)
	{
		String filename = file.name();
		ptr += R"rawliteral(
    <div class="file_list">
      <div class="file_item">
        <div class="file_wrapper">
          <div class="file_text_area"> FILE: </div>
          <div class="file_text_area">
          )rawliteral";
		ptr += filename;
		ptr += R"rawliteral(        
          </div>
        </div>
        <div class="file_wrapper">
          <a class="file_text_area" 
          href=" )rawliteral";
		ptr += filename;
		ptr += R"rawliteral(" 
          download=")rawliteral";
		ptr += filename;
		ptr += R"rawliteral(">
          Download</a>
        </div>
        <div class="file_wrapper">
          <div class="file_text_area ">Size:</div>
          <div class="file_text_area">)rawliteral";
		ptr += String(file.size());
		ptr += R"rawliteral(        
           </div>
          <div class="file_text_area"> byte</div>
        </div>
      </div>
    </div>
  )rawliteral";
		file = root.openNextFile();
	}
	ptr += R"rawliteral(
  </div>
</body>
</html>
)rawliteral";
	return ptr;
}
void handle_OnConnect()
{
	Serial.println("Connection");
	server.send(200, "text/html", SendMainHTML(stts));
}
void handle_OnTest()
{
	if (!isCalibration && !isTest)
	{
		Serial.println("Testing");
		server.send(200, "text/html", "TEST please wait on this page while test will finish");
		apTask = DT_AppTask::DROP_TASK;
	}
}
void handle_OnCalibration()
{
	if (!isCalibration && !isTest)
	{
		Serial.println("Calibration");
		apTask = DT_AppTask::CALIBRATION;
		server.send(200, "text/html", "CALIBRATION START");
	}
}
void handle_OnClearFileSystem()
{
	Serial.println("ClearFS");
	File root = SPIFFS.open("/");
	File file = root.openNextFile();
	while (file)
	{
		Serial.print("  FILE: ");
		Serial.print(file.name());
		Serial.print("\tSIZE: ");
		Serial.println(file.size());
		String fn = "/"; fn += file.name();
		SPFmin::deleteFile(SPIFFS, fn.c_str());
		file = root.openNextFile();
	}
	server.send(200, "text/html", SendMainHTML(stts));
}
bool handleFileRead(String path)
{
	// Открываем файл для чтения по указанному пути
	File file = SPIFFS.open(path, "r");
	// Если файл не удалось открыть
	if (!file)
	{
		Serial.println("Ошибка. Файл не существует.");
		// Отвечаем клиенту ошибкой
		server.send(404, "text/plain", "FileNotFound");
		// Возвращаемся из функции
		return false;
	}
	// Отдаём клиенту содержимое файла
	server.streamFile(file, "text/html");
	// Закрываем файл
	file.close();
	// Возвращаемся из функции
	return true;
}
#pragma endregion

#pragma region TASKS

void /*IRAM_ATTR */ Task_Calibration(void *arg)
{
	isCalibration = true;
	Serial.println("Calibration start");

	while (true)
	{
		/* code */
		break;
	}

	delay(5000);

	Serial.println("Calibration end");
	isCalibration = false;
	vTaskDelete(NULL);
}

void /*IRAM_ATTR */ Task_Test(void *arg)
{
	isTest = true;
	Serial.println("TEST start");
	++testNo;
	int8_t i = 100;
	String tn = "/Test";tn += String(testNo); tn += ".txt";

	File file = SPIFFS.open(tn, FILE_WRITE);
	if (!file)
	{
		stts = DT_AppStatus::BAD_CODE;
		isTest = false;
	}
	
	if(isTest){
		while (i>0)
		{
			file.println(i);
			i--;
		}
	}
	file.close();

	Serial.println("TEST end");
	isTest = false;
	stts = DT_AppStatus::DROP_TEST_END;
	delay(1000);
	ledCtrl.SetColorByCode(stts);
	vTaskDelete(NULL);
}

#pragma endregion TASKS

void setup()
{
	Serial.begin(115200);

	ledCtrl.begin(LEDR, LEDG, LEDB);

	if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
	{
		Serial.println("SPIFFS::Mount Failed");
		stts = DT_AppStatus::BAD_CODE;
		ledCtrl.SetColorByCode(stts);
		return;
	}
	delay(100);
	if (!SPFmin::listDir(SPIFFS, "/", 0))
	{
		Serial.println("SPIFFS::DirectoryError");
		stts = DT_AppStatus::BAD_CODE;
		ledCtrl.SetColorByCode(stts);
		return;
	}
	

	if (!WiFi.softAP(ssid, password))
	{
		Serial.println("WiFi::starting error");
		stts = DT_AppStatus::BAD_CODE;
		ledCtrl.SetColorByCode(stts);
		return;
	}

	
	if (!WiFi.softAPConfig(local_ip, gateway, subnet))
	{
		Serial.println("WiFi::config error");
		stts = DT_AppStatus::BAD_CODE;
		ledCtrl.SetColorByCode(stts);
		return;
	}
	delay(100);

	server.on("/", handle_OnConnect);
	server.on("/call", handle_OnCalibration);
	server.on("/Stest", handle_OnTest);
	server.on("/fileClear", handle_OnClearFileSystem);
	server.onNotFound(
		[]()
		{
			// Serial.println(server.uri());
			// if (server.uri() == "/favicon.ico")	 // это не работает !!!
			// {
			// 	return;
			// }
			// "/favicon.ico" - ебаное дерьмецо которое портит жизнь консоли (PC only)
			if (!handleFileRead(server.uri())) 
			{
				server.send(404, "text/plain", "Not found");
			}
			
		});

	server.begin();
	Serial.println("HTTP server started");
	ledCtrl.SetColorByCode(stts);
	Serial.println("OnWork");
	apTask = DT_AppTask::SITE;
}

void loop()
{
	switch (apTask)
	{
	case DT_AppTask::SITE:
		server.handleClient();
		delay(20);
		break;
	case DT_AppTask::CALIBRATION:
		if (!isCalibration && !isTest)
		{
			xTaskCreatePinnedToCore(Task_Calibration, "calb", 2048, NULL, 5, NULL, 0);
			Serial.println("case calibration");
		}
		apTask = DT_AppTask::SITE;
		break;
	case DT_AppTask::DROP_TASK:
		if (!isCalibration && !isTest)
		{
			xTaskCreatePinnedToCore(Task_Test, "dt", 4096, NULL, 1, NULL, 0);
			Serial.println("case TEST");
		}
		apTask = DT_AppTask::SITE;
		break;

		break;
	default:
		delay(200);
		break;
	}
}
