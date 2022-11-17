// Including librarys
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <MFRC522.h>
#include <SPI.h>
#include <WiFi.h>

// Defining the variables that represent the ports
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define RFID_SS_SDA 21
#define RFID_RST 14

#define BUZZER 18
#define LED1 36
#define LED2 39
#define LED3 45
#define LED4 37

// Declaring variables
int iniciar = 0;

bool ftmSuccess = true;

MFRC522 rfidBase = MFRC522(RFID_SS_SDA, RFID_RST);

// Defining constants
const int I2C_SDA = 2;
const int I2C_SCL = 1;

const uint8_t FTM_FRAME_COUNT = 16;

const uint16_t FTM_BURST_PERIOD = 2;

xSemaphoreHandle ftmSemaphore;

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
    Serial.printf("FTM Estimate: Distance: %.2f m, Return Time: %u ns\n", (float)report->dist_est / 100.0 - 41, report->rtt_est);
    
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

// Creating RFID class
class LeitorRFID{
  private:
    // Declaring variables
    char codigoRFIDLido[100] = "";
    char codigoRFIDEsperado[100] = "";
    MFRC522 *rfid = NULL;
    int cartaoDetectado = 0;
    int cartaoJaLido = 0;

    // void processaCodigoLido(){
    //   char codigo[3*rfid->uid.size+1];
    //   codigo[0] = 0;
    //   char temp[10];
    //   for(int i=0; i < rfid->uid.size; i++){
    //     sprintf(temp,"%X",rfid->uid.uidByte[i]);
    //     strcat(codigo,temp);
    //   }
    //   codigo[3*rfid->uid.size+1] = 0;
    //   strcpy(codigoRFIDLido,codigo);
    //   Serial.println(codigoRFIDLido);
    // }
  public:
    LeitorRFID(MFRC522 *leitor){
      rfid = leitor;
      rfid->PCD_Init();
    };
    char *tipoCartao(){
      MFRC522::PICC_Type piccType = rfid->PICC_GetType(rfid->uid.sak);
      Serial.println(rfid->PICC_GetTypeName(piccType));
      return((char *)rfid->PICC_GetTypeName(piccType));
    };
    int cartaoPresente(){
      return(cartaoDetectado);
    };
    int cartaoFoiLido(){
      return(cartaoJaLido);
    };

    // Reading card and giving feedback to user
    void leCartao(){
      if (rfid->PICC_IsNewCardPresent()) {
        iniciar = 7;
        Serial.println("Cartao presente");
        cartaoDetectado = 1;
        if (rfid->PICC_ReadCardSerial()) {
          Serial.println("Cartao lido");
          digitalWrite(LED4, HIGH);
          cartaoJaLido = 1;
          //processaCodigoLido();
          rfid->PICC_HaltA(); // Halt PICC
          rfid->PCD_StopCrypto1(); // Stop encryption on PCD
          tone(BUZZER, 3000, 1000);
          delay(1000);
        }
      }
      // Turn off led after reading
      else{
        cartaoDetectado = 0;
        iniciar = 10;
        digitalWrite(LED4, LOW);
      }
    };
    char *cartaoLido(){
      return(codigoRFIDLido);
    };

    // Restart reading
    void resetarLeitura(){
      cartaoDetectado = 0;
      cartaoJaLido = 0;
      iniciar = 10;
    }
};

// Creating variable to instantiate a class
LeitorRFID *leitor = NULL;

void setup() {
  Serial.begin(115200);
  SPI.begin();

  // Defining data type of each port(input and output)
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED3, OUTPUT);

  // Turn on led
  digitalWrite(LED1, HIGH);

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
  WiFi.begin("Grupo5", "Grupo5-123");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  // Starting reader
  leitor->leCartao();
  
  // Checking card
  if(leitor->cartaoFoiLido()){
    // Writing card type on cosole and reseting reading
    Serial.println(leitor->tipoCartao());
    Serial.println(leitor->cartaoLido());
    leitor->resetarLeitura();
    delay(2500);
  }

  // Defining position on display
  lcd.setCursor(4, 0);
  // Printing on display
  lcd.print("Distancia");

  getFtmReport();

  delay(1000);
}