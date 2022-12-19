// Including librarys
#include <Wire.h>
#include <MFRC522.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h> 
#include <string.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>

//Defining compiler constants
#define RFID_SS_SDA 10
#define RFID_RST 14
#define BUZZER 18
#define LED4 8
#define PRISMA_NUMBER 256
#define OPERATION "Inteli"
#define LENGHT_ARRAY 100


// Instantiating objects
xSemaphoreHandle ftmSemaphore;
MFRC522 rfidBase = MFRC522(RFID_SS_SDA, RFID_RST);
String jsonVars;
JSONVar getJson;

// Instantiating variables
String serverName = "http://10.128.64.59:2707/";
String codeRegion;
float distance;
int iniciar = 0, ledState = LOW;
int count = 0;
unsigned long previousMillis = 0;
bool count_c = true, ftmSuccess = true;
double velocityMensured = 0;
double indexDistance;


// Defining constants
String slave_Fouded;
String SSIDs[LENGHT_ARRAY];
int32_t RSSIs[LENGHT_ARRAY];
const char* password_ap = "RX6]F^SEIHu[4HYj";
const char* AP_SSID = "Inteli-COLLEGE";
const char* AP_PWD = "QazWsx@123";

const double velocity = 2.0;

const int IdValletParking = 1;

const uint8_t FTM_FRAME_COUNT = 16;
const uint16_t FTM_BURST_PERIOD = 2;

//Function that reset the values of the netowork arrays
void resetNetworks(){
  for(int i=0; i <LENGHT_ARRAY; i++){
    SSIDs[i] = "";
    RSSIs[i] = NULL;
  }
}

//Funtcion that scan the wifi network and get the slaves networks to the arrays and yours force of signal
void foundSlave() {
  //Scanning Networks until found someone
  int8_t scanResults = WiFi.scanNetworks();
  while(scanResults == 0){
    scanResults = WiFi.scanNetworks();
    
  }
  //reset slaves
  int SlaveCnt = 0;
  if (scanResults != 0) {

    for (int i = 0; i < scanResults; ++i) {
    
      // Check if the current device starts with `Slave`
      if (WiFi.SSID(i).indexOf("Slave_") == 0) {
        SSIDs[SlaveCnt] = WiFi.SSID(i);
        RSSIs[SlaveCnt] = WiFi.RSSI(i);
    
        SlaveCnt++;
      }
    }
}
}

//Function to get the value of meters by the slave and your respective region
void getDistanceSlave(String slave){
  int firstIndex = slave.indexOf("_");
  int secondIndex = slave.indexOf("_", firstIndex + 1 );
  indexDistance = slave.substring(firstIndex+1, secondIndex).toDouble();
  codeRegion = slave.substring(secondIndex+1);

}

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
    getDistanceSlave(slave_Fouded);
    distance =  (float)report->dist_est / 100.0 - 41 +(indexDistance*20);    
    
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
  getDistanceSlave(slave_Fouded);
  return String(((distance+(indexDistance*20))/velocity)/60);
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

void getValletSatus(char* card_id, float distance, String ssid_ap, int count){
  Serial.println("Getting JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    String valletInfo;
            
    //Receiving return string from the httpGETRequest function and converting it to JSON
    do{
      valletInfo = httpGETRequest();   
    }while(valletInfo == "{}");
    
    JSONVar myObject = JSON.parse(valletInfo);
    
    //Checking if a value was returned by the endpoint
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    //Storing returned values in ESP global variables
    int idVallet = myObject[0]["ID"];
    String status = myObject[0]["STATUS"];

    //Updating JSON values
    getJson["IdVallet"] = idVallet;
    getJson["Prism"] = PRISMA_NUMBER;
    getJson["Status"] = status;
    getJson["IdValletParking"] = 1;
    getJson["Time"] = getTime();
    getJson["Card_Id"] = card_id;
    getJson["Distance"] = distance;
    getJson["CodeRegion"] = codeRegion;
    getJson["Asid_AP"] = ssid_ap;
    getJson["Count"] = count;

  }
  else {
    Serial.println("Error in WiFi connection");
  }
}

void postDataToServer(char* card_id, float distance, String ssid_ap, int count) {
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
    getJson["CodeRegion"] = codeRegion;
    getJson["Ssid_AP"] = ssid_ap;
    getJson["Operation"] = OPERATION;

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
      for(int i=0; i < rfid->uid.size; i++){
        sprintf(temp,"%X",rfid->uid.uidByte[i]);
        strcat(codigo,temp);
      }
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

    //This function try to connect to the slave with the most strenght signal
    String connect(){
      do{
        foundSlave();
      }while(SSIDs[0]=="");
      
      int32_t maxVal = RSSIs[0];
      int index = 0;
      for (int i = 0; i < (sizeof(RSSIs) / sizeof(RSSIs[0])); i++) {
        if (RSSIs[i]!= 0 && RSSIs[i] > maxVal) {
          maxVal = RSSIs[i];
          index = i;
        }
      }
      // Connecting to WiFi
      wifiConnection(SSIDs[index].c_str(), password_ap);

      
      // clean up ram
      WiFi.scanDelete();
      return SSIDs[index];
    }

    //Function to blink led if the car is moving
    void tinkleLed(){

      // Interval constant
      const long interval = 500;

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

    //Function that checks the current interaction with the ESP and performs the corresponding request
    void requestVerify(){      

      //Start connecting to the ESP
      slave_Fouded = connect();
      
      //Updating the distance value
      getFtmReport();
      
      //Reseting the array networks
      resetNetworks();

      //Start connecting to the server
      wifiConnection(AP_SSID, AP_PWD);
    
      switch(count){
        case 1:
          //Send prism value to server
          postDataToServer(processaCodigoLido(), distance, slave_Fouded, count);
          count_c = false;
          break;
        case 2:
          //Updating valet values ​​on the server
          getValletSatus(processaCodigoLido(), distance, slave_Fouded, count);
          delay(10);
          //Updating the values of database
          updateValletStatus();
          count_c = false;
          break;
        case 3:
          tinkleLed();
          leCartao();
          //Updating valet values ​​on the server
          getValletSatus(processaCodigoLido(), distance, slave_Fouded, count);
          tinkleLed();
          leCartao();
          delay(10);
          //Updating the values of database
          updateValletStatus();
          while(count == 3){
            //Start connecting to the ESP
            slave_Fouded = connect();
            tinkleLed();
            leCartao();

            //Updating the distance value
            getFtmReport();
            //Reseting the array networks
            resetNetworks();
            tinkleLed();
            leCartao();
            //Start connecting to the server
            wifiConnection(AP_SSID, AP_PWD);
            tinkleLed();
            leCartao();
            //Updating valet values ​​on the server
            getValletSatus(processaCodigoLido(), distance, slave_Fouded, count);
            tinkleLed();
            leCartao();
            delay(10);
            //Updating valet values of time ​​on the server
            updateValletTime();
            tinkleLed();
            leCartao();
          }
          break;
        case 4:
          //Updating valet values ​​on the server and resetting the counter
          getValletSatus(processaCodigoLido(), distance, slave_Fouded, count);
          delay(10);
          //Updating the values of database
          updateValletStatus();

          //Reseting state variables
          count_c = false;
          count = 0;
          break;
      }
      
    }

    // Functon to read card and giving feedback to user
    void leCartao(){
      if (rfid->PICC_IsNewCardPresent()) {
        iniciar = 7;
        Serial.println("\nCartao presente");
        cartaoDetectado = 1;
        if (rfid->PICC_ReadCardSerial()) {
          count++;
          count_c = true;
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
    void wifiConnection(const char* login, const char* password){
      if(WiFi.status() == WL_CONNECTED){
        WiFi.disconnect();
      }
      WiFi.begin(login, password);
      const long interval = 500;
      const long intervalSlave = 4000;
      long previousSlave = millis();

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
            WiFi.begin(login, password);
          }

            // set the LED with the ledState of the variable:
        }
        tinkleLed();
        leCartao();

        //Verify if the connection with the slave didnt happen, then try to found another Slave
        if(millis() - previousSlave >= intervalSlave && login != AP_SSID){
          WiFi.disconnect();
          resetNetworks();
          requestVerify();
        }
        
      }
      Serial.println("\nConectado a: "+String(login));
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
  pinMode(LED4, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  

  // Setting reader class to a variable
  leitor = new LeitorRFID(&rfidBase);

  
  ftmSemaphore = xSemaphoreCreateBinary();
  
  // Starting FTM event
  WiFi.onEvent(onFtmReport, ARDUINO_EVENT_WIFI_FTM_REPORT);
  
}

void loop() {

  // Starting reader
  leitor->leCartao();

  //Checking if the driver has initiated the interaction
  if(count != 0 && count_c==true){
    //Starting function to access endpoints
    leitor->requestVerify();
  }

  //If the state of system is impar then tinkle the led
  if(count%2 !=0){
    leitor->tinkleLed();
  }

}
