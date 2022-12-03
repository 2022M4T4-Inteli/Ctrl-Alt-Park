// Including librarys
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <MFRC522.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h> 
#include <string.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

// Defining the variables that represent the ports
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define RFID_SS_SDA 21
#define RFID_RST 14
#define BUZZER 18
#define LED1 36
#define LED2 39
#define LED3 45
#define LED4 37
#define PRISMA_NUMBER 256

// Adafruit_MMA8451 mma = Adafruit_MMA8451();

// Instantiating objects
WiFiMulti wifiMulti;
MFRC522 rfidBase = MFRC522(RFID_SS_SDA, RFID_RST);
String jsonVars;
xSemaphoreHandle ftmSemaphore;
JSONVar getJson;

// Instantiating variables
String serverName = "http://10.128.64.231:2707/";
float distance;
int iniciar = 0, ledState = LOW;
int count = 0;
unsigned long previousMillis = 0;
bool count_c = false, ftmSuccess = true;


// Defining constants
const char* ssid_ap = "Grupo5";
const char* password_ap = "Grupo5-123";
const char *AP_SSID = "Inteli-COLLEGE";
const char *AP_PWD = "QazWsx@123";

const float velocity = 1;

const int IdValletParking = 1;
const int I2C_SDA = 2;
const int I2C_SCL = 1;

const uint8_t FTM_FRAME_COUNT = 16;
const uint16_t FTM_BURST_PERIOD = 2;

// int scanI2C(){
//   byte error, address;
//   int nDevices;

//   Serial.println("Scanning...");

//   nDevices = 0;
//   for(address = 1; address< 127; address++ ) 
//   {
//     // The i2c_scanner uses the return value of
//     // the Write.endTransmisstion to see if
//     // a device did acknowledge to the address.
//     Wire.beginTransmission(address);
    

//     if (error == 0)
//     {
//       Serial.print("I2C device found at address 0x");
//       if (address<16) 
//         Serial.print("0");
//       Serial.println(address,HEX);
//       return (address,HEX);
//       Serial.println("  !");

//       nDevices++;
//     }
//     else if (error==4) 
//     {
//       Serial.print("Unknow error at address 0x");
//       if (address<16) 
//         Serial.print("0");
//       Serial.println(address,HEX);
//     }    
//   }
//   if (nDevices == 0)
//     Serial.println("No I2C devices found\n");
//   else
//     Serial.println("done\n");

//   delay(5000);  
// }

// void acelerometerInit(){
//   if (! mma.begin(scanI2C())) {
//     Serial.println("Couldnt start");
//     while (1);
//   }
//   Serial.println("MMA8451 found!");
  
//   mma.setRange(MMA8451_RANGE_2_G);
  
//   Serial.print("Range = "); Serial.print(2 << mma.getRange());  
//   Serial.println("G");
// }

// void readAcelerometer(){
//   mma.read();
//   Serial.print("X:\t"); Serial.print(mma.x); 
//   Serial.print("\tY:\t"); Serial.print(mma.y); 
//   Serial.print("\tZ:\t"); Serial.print(mma.z); 
//   Serial.println();

//   /* Get a new sensor event */ 
//   sensors_event_t event; 
//   mma.getEvent(&event);

//   /* Display the results (acceleration is measured in m/s^2) */
//   Serial.print("X: \t"); Serial.print(event.acceleration.x); Serial.print("\t");
//   Serial.print("Y: \t"); Serial.print(event.acceleration.y); Serial.print("\t");
//   Serial.print("Z: \t"); Serial.print(event.acceleration.z); Serial.print("\t");
//   Serial.println("m/s^2 ");
// }

// Function for validating the status of the FTM
void onFtmReport(arduino_event_t *event) {
  // Variable to store return types
  const char * status_str[5] = {"SUCCESS", "UNSUPPORTED", "CONF_REJECTED", "NO_RESPONSE", "FAIL"};

  // Starting FTM
  wifi_event_ftm_report_t * report = &event->event_info.wifi_ftm_report;
  
  // Getting FTM status
  ftmSuccess = report->status == FTM_STATUS_SUCCESS;

  // Verifying status value, if is success return the distance, else return error message
  if (ftmSuccess) {

    //Assigning new distance value through FTM
    distance =  (float)report->dist_est / 100.0 - 41;    
    
    // Showing display infos
    lcd.setCursor(6, 1);
    lcd.print(String((float)report->dist_est / 100.0 - 41));
    free(report->ftm_report_data);
  } else {
    Serial.print("FTM Error: ");
    Serial.println(status_str[report->status]);
  }

  xSemaphoreGive(ftmSemaphore);
}

// Start FTM session and verifying report
bool getFtmReport(){
  if(!WiFi.initiateFTM(FTM_FRAME_COUNT, FTM_BURST_PERIOD)){
    Serial.println("FTM Error: Initiate Session Failed");
    return false;
  }

  return xSemaphoreTake(ftmSemaphore, portMAX_DELAY) == pdPASS && ftmSuccess;
}

//Getting estimated time of arrival value from esp to paired esp
String getTime(){
  return String((distance/velocity)/60);
}

//Function makes a request to serverName and it retrieves a string with a JSON object.
String httpGETRequest() {

  //Instantiating the HTTP object
  HTTPClient http;

  //Initializing http connection
  http.begin(serverName + "getStatus/256");

  // Send HTTP POST request
  int httpResponseCode = http.GET();
  String payload = "{}";

  //Checking request response
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;
}


//Function that updates the ESP estimated time of arrival
void updateValletTime(){
  Serial.println("Updating JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){

    //Instantiating the HTTP object
    HTTPClient http;   

    //Initializing http connection
    http.begin(serverName + "updateValletTime");
    http.addHeader("Content-Type", "application/json");

    //Converting the string to JSON and sending the update
    String requestBody = JSON.stringify(getJson);
    int httpResponseCode = http.PUT(requestBody);    

    //Checking request response
    if(httpResponseCode>0){
      String response = http.getString();   
      Serial.println(httpResponseCode);
      Serial.println(response);          

    }else{

      Serial.print("Error on sending PUT Request: ");
      Serial.println(httpResponseCode);

    }

    //Closing http connection
    http.end();

  }else{
    Serial.println("Error in WiFi connection");
  }
}

void updateValletStatus(){
  Serial.println("Updating JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){

    //Instantiating the HTTP object
    HTTPClient http;   

    //Initializing http connection
    http.begin(serverName + "updateValletStatus");
    http.addHeader("Content-Type", "application/json");

    //Converting the string to JSON and sending the update
    String requestBody = JSON.stringify(getJson);
    int httpResponseCode = http.PUT(requestBody);   

    //Checking request response
    if(httpResponseCode>0){
      String response = http.getString();   
      Serial.println(httpResponseCode);
      Serial.println(response);          

    }else{

      Serial.print("Error on sending PUT Request: ");
      Serial.println(httpResponseCode);

    }

    //Closing http connection
    http.end();

  }else{
    Serial.println("Error in WiFi connection");
  }
}

void getValletSatus(char* card_id, float distance, const char* ssid_ap, int count){
  Serial.println("Getting JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
            
    //Receiving return string from the httpGETRequest function and converting it to JSON
    String valletInfo = httpGETRequest();
    JSONVar myObject = JSON.parse(valletInfo);
    
    //Checking if a value was returned by the endpoint
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    //Storing returned values in ESP global variables
    int prism = myObject[0]["PRISM"];
    int idVallet = myObject[0]["ID"];
    String status = myObject[0]["STATUS"];

    //Updating JSON values
    getJson["IdVallet"] = idVallet;
    getJson["Prism"] = prism;
    getJson["Status"] = status;
    getJson["IdValletParking"] = 1;
    getJson["Time"] = getTime();
    getJson["Card_Id"] = card_id;
    getJson["Distance"] = distance;
    getJson["Asid_AP"] = ssid_ap;
    getJson["Count"] = count;

  }
  else {
    Serial.println("Error in WiFi connection");
  }
}

void postDataToServer(char* card_id, float distance, const char* ssid_ap, int count) {
  Serial.println("Posting JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  //Check WiFi connection status
  if (WiFi.status()== WL_CONNECTED) {

    //Instantiating the HTTP object
    HTTPClient http;

    //Initializing http connection
    http.begin(serverName + "postVallet");
    http.addHeader("Content-Type", "application/json");

    //Adding Values to JSON
    getJson["Prism"] = PRISMA_NUMBER;
    getJson["Card_Id"] = card_id;
    getJson["Distance"] = distance;
    getJson["Asid_AP"] = ssid_ap;
    getJson["Count"] = count;

    //Converting the string to JSON and sending the post
    String requestBody = JSON.stringify(getJson);
    int httpResponseCode = http.POST(requestBody);

    //Checking request response
    if(httpResponseCode>0){
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    else {
    Serial.print("Error on sending HTTP POST: ");
    Serial.println(httpResponseCode);
    }
    //Closing http connection
    http.end();
  }
  else {
    Serial.println("Error in WiFi connection");
    }
}

// Creating RFID class
class LeitorRFID{
  private:
    // Declaring variables
    char codigoRFIDLido[100] = "";
    char codigoRFIDEsperado[100] = "";
    MFRC522 *rfid = NULL;
    int cartaoDetectado = 0;
    int cartaoJaLido = 0;

    //Function that returns code read in the reader
    char* processaCodigoLido(){
        char codigo[3*rfid->uid.size+1];
        codigo[0] = 0;
        char temp[10];
      
        codigo[3*rfid->uid.size+1] = 0;
        strcpy(codigoRFIDLido,codigo);
        return codigoRFIDLido;
      }

  public:

    //Function to start and instantiate the reader
    LeitorRFID(MFRC522 *leitor){
      rfid = leitor;
      rfid->PCD_Init();
    };

    //Function to return card type    
    char *tipoCartao(){
      MFRC522::PICC_Type piccType = rfid->PICC_GetType(rfid->uid.sak);
      Serial.println(rfid->PICC_GetTypeName(piccType));
      return((char *)rfid->PICC_GetTypeName(piccType));
    };

    //Function if the card has been detected
    int cartaoPresente(){
      return(cartaoDetectado);
    };

    //Function if the card has been read
    int cartaoFoiLido(){
      return(cartaoJaLido);
    };

    //Function to blink led if the car is moving
    void tinkleLed(){

      // Interval constant
      const long interval = 1000;

      if (millis() - previousMillis >= interval) {
          // save the last time you blinked the LED
          previousMillis = millis();
          

          // if the LED is off turn it on and vice-versa:
          if (ledState == LOW) {
            ledState = HIGH;
          } else {
            ledState = LOW;
          }

          // set the LED with the ledState of the variable:
          digitalWrite(LED4, ledState);
        }
    }

    //Function that defines a new distance and returns card read
    char* data(){
      getFtmReport();
      return (processaCodigoLido());
    }

    //Function that checks the current interaction with the ESP and performs the corresponding request
    void requestVerify(){

      //Start connecting to the server
      wifiConnection(AP_SSID, AP_PWD);
    
      switch(count){
        case 1:
          //Send prism value to server
          postDataToServer(data(), distance, ssid_ap, count);
          break;
        case 2:
          //Updating valet values ​​on the server
          getValletSatus(data(), distance, ssid_ap, count);
          delay(100);
          updateValletStatus();
          break;
        case 3:
          //Updating valet values ​​on the server
          getValletSatus(data(), distance, ssid_ap, count);
          delay(10);
          updateValletStatus();
          delay(10);
          updateValletTime();
          while(count == 3){
            tinkleLed();
            leCartao();

            //Start connecting to the ESP
            wifiConnection(ssid_ap, password_ap);

            //Updating the distance value
            getFtmReport();

            //Start connecting to the server
            wifiConnection(AP_SSID, AP_PWD);

            tinkleLed();
            leCartao();

            getValletSatus(data(), distance, ssid_ap, count);

            delay(10);

            updateValletTime();

            tinkleLed();
            leCartao();
          }
          break;
        case 4:
          //Updating valet values ​​on the server and resetting the counter
          getValletSatus(data(), distance, ssid_ap, count);
          delay(10);
          updateValletStatus();
          count = 0;
          break;
      }
      //Start connecting to the ESP
      wifiConnection(ssid_ap, password_ap);
    }

    //Function that checks the state of the system and executes the function determined for the moment
    void parking(){
      requestVerify();
      while(count%2 != 0){
        if (count == 3){
          //função que posta dados no servidor
          requestVerify();
        }
        tinkleLed();
        leCartao();
      }
      requestVerify();
    }

    // Functon to read card and giving feedback to user
    void leCartao(){
      if (rfid->PICC_IsNewCardPresent()) {
        iniciar = 7;
        Serial.println("\nCartao presente");
        cartaoDetectado = 1;
        if (rfid->PICC_ReadCardSerial()) {
          count++;
          Serial.println("Cartao lido");
          cartaoJaLido = 1;
          rfid->PICC_HaltA(); // Halt PICC
          rfid->PCD_StopCrypto1(); // Stop encryption on PCD
          digitalWrite(LED4, HIGH);
          tone(BUZZER, 3000, 1000);
        }
      }
      // Turn off led after reading
      else{
        cartaoDetectado = 0;
        iniciar = 10;
        digitalWrite(LED4, LOW);
      }
    };

    //Function that returns the RFID code read
    char *cartaoLido(){
      return(codigoRFIDLido);
    };

    // Restart reading
    void resetarLeitura(){
      cartaoDetectado = 0;
      cartaoJaLido = 0;
      iniciar = 10;
    }

    //Function that start a wifi conection and print '.' while connection is starting
    void wifiConnection(const char *login, const char *password){
      WiFi.begin(login, password);
      const long interval = 500;

      while (WiFi.status() != WL_CONNECTED) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
          // save the last time you blinked the LED
          previousMillis = currentMillis;
          

          // if the LED is off turn it on and vice-versa:
          if (ledState == LOW) {
            Serial.print(".");
            ledState = HIGH;
          } else {
            ledState = LOW;
          }

            // set the LED with the ledState of the variable:
        }
        tinkleLed();
        leCartao();
        
      }
    }
};

// Creating variable to instantiate a class
LeitorRFID *leitor = NULL;

void setup() {
  //Setup serial Communication.
  Serial.begin(115200);

  //Initializes the SPI bus by setting SCK, MOSI, and SS to outputs, pulling SCK and MOSI low, and SS high
  SPI.begin();

  // Defining data type of each port(input and output)
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED3, OUTPUT);

  // Turn on led
  digitalWrite(LED1, HIGH);

  // Wire.begin(4, 5);

  // acelerometerInit();

  // Setting reader class to a variable
  leitor = new LeitorRFID(&rfidBase);

  // Starting display
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();                
  lcd.backlight();
  
  ftmSemaphore = xSemaphoreCreateBinary();
  
  // Starting FTM event
  WiFi.onEvent(onFtmReport, ARDUINO_EVENT_WIFI_FTM_REPORT);
  
  // Connecting to WiFi
  Serial.println("Connecting to FTM Responder");
  leitor->wifiConnection(ssid_ap, password_ap);
 
  //Checking if the connection between the ESPs has happened
  if(WiFi.status()== WL_CONNECTED){
      Serial.println("\nConectado");    
  }
}

void loop() {

  // readAcelerometer();

  // Starting reader
  leitor->leCartao();

  //Checking if the driver has initiated the interaction
  if(count%2 != 0){

    //Starting function to access endpoints
    leitor->parking();
  }
  
  // Checking card
  if(leitor->cartaoFoiLido()){
    // Writing card type on cosole and reseting reading
    Serial.println(leitor->tipoCartao());
    Serial.println(leitor->cartaoLido());
    leitor->resetarLeitura();
  }

  // Defining position on display
  lcd.setCursor(4, 0);
  // Printing on display
  lcd.print("Distancia");
}