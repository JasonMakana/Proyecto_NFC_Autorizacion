#include <SPI.h>
#include <Adafruit_PN532.h>

// Pines SPI para PN532
#define SCK  12 
#define MISO 13
#define MOSI 11
#define SS   5

// Pines para LED RGB
#define LED_R 40
#define LED_G 41
#define LED_B 42

// Creacion del objeto PN532 usando SPI
Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);

// ID's de las tarjetas con autorización
uint8_t tarjetaAutorizada1[] = {0x7D, 0xF2, 0x17, 0x07}; 
uint8_t tarjetaAutorizada2[] = {0x7E, 0xD7, 0x2A, 0x07};

// Funcion para el led
void encenderColor(int r, int g, int b) {
  digitalWrite(LED_R, r);
  digitalWrite(LED_G, g);
  digitalWrite(LED_B, b);
}

void setup() {
  Serial.begin(115200);

  // Inicializacion de los pines LED
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  
  // Azul de espera inicial
  encenderColor(0, 0, 1); 

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("PN532 no detectado");
    encenderColor(1, 0, 0); // Rojo si hay error de conexión
    while (1);
  }
  
  // Activación del modo lectura
  nfc.SAMConfig();
  Serial.println("Sistema Listo. Esperando tarjeta...");
}

void loop() {
  uint8_t uid[7]; // Variable para el ID de la tarjeta
  uint8_t uidLength; // Variable de la longitud

  // LED en Azul mientras espera
  encenderColor(0, 0, 1);

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.print("\nUID detectado: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(uid[i], HEX); Serial.print(" ");
    }

    bool accesoConcedido = false;

    // Validación (comparamos solo los primeros 4 bytes si es Mifare Classic)
    if (memcmp(uid, tarjetaAutorizada1, 4) == 0 || memcmp(uid, tarjetaAutorizada2, 4) == 0) {
      accesoConcedido = true;
    }

    // Valida si tiene acceso o no
    if (accesoConcedido) {
      Serial.println("\n[ACCESO CONCEDIDO]");
      encenderColor(0, 1, 0); // Verde
    } else {
      Serial.println("\n[ACCESO DENEGADO]");
      encenderColor(1, 0, 0); // Rojo
    }

    delay(2000); // El color se mantiene 2 segundos
  }
}