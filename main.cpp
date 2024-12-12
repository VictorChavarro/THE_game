#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "TTMapa.h" // Archivo con la imagen del fondo
#include "HatsuneMiku.h" // Archivo con la imagen de Miku en formato monocromático

// Pines para el display
#define TFT_DC 7
#define TFT_CS 6
#define TFT_MOSI 11
#define TFT_CLK 13
#define TFT_RST 10
#define TFT_MISO 12

// Botones
#define BUTTON_RIGHT 20
#define BUTTON_LEFT 19
#define BUTTON_JUMP 18

// Buzzer
#define BUZZER_PIN 9

// Configuración del display
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

// Variables de Miku
int MikuX = 0; // Posición inicial en la esquina inferior izquierda
int MikuY = 292; // Ajustado para que esté sobre los bloques inferiores
const int MikuWidth = 28;
const int MikuHeight = 23;
bool MikuJumping = false;
int MikuJumpHeight = 0;
bool MikuOnPlatform = false;

// Configuración de las plataformas
struct Platform {
  int x, y, width, height;
};

const Platform platforms[] PROGMEM = {
  {0, 292, 240, 28} // Plataforma sólida en la parte inferior (todo el ancho de la base)
};
const int platformCount = sizeof(platforms) / sizeof(Platform);

// Tonos ambientales generados a partir del archivo de audio
const int ambientSound[][2] = {
  {97, 250},  {195, 250}, {698, 250}, {110, 250},
  {130, 250}, {195, 250}, {130, 250}, {195, 250},
  {146, 250}, {220, 250}  // Secuencia simplificada de tonos
};
const int ambientSize = sizeof(ambientSound) / sizeof(ambientSound[0]);

void playAmbientSound() {
  static unsigned long lastNoteTime = 0;
  static int noteIndex = 0;

  unsigned long currentTime = millis();

  // Reproducir cada nota en orden
  if (currentTime - lastNoteTime > ambientSound[noteIndex][1]) {
    noTone(BUZZER_PIN); // Apaga el buzzer entre notas
    delay(20); // Pausa pequeña entre notas
    tone(BUZZER_PIN, ambientSound[noteIndex][0], ambientSound[noteIndex][1]);
    lastNoteTime = currentTime;
    noteIndex++;

    // Reiniciar el patrón ambiental
    if (noteIndex >= ambientSize) {
      noteIndex = 0;
    }
  }
}

// Funciones para dibujar elementos
void drawBackground() {
  tft.drawBitmap(0, 0, TTM, 200, 200, ILI9341_WHITE);
}

void drawMiku() {
  tft.drawBitmap(0, 0, Miku, MikuWidth, MikuHeight, ILI9341_BLUE);
}

void clearMiku() {
  tft.fillRect(MikuX, MikuY, MikuWidth, MikuHeight, ILI9341_BLACK);
}

void drawPlatforms() {
  for (int i = 0; i < platformCount; i++) {
    Platform platform;
    memcpy_P(&platform, &platforms[i], sizeof(Platform));
    tft.fillRect(platform.x, platform.y, platform.width, platform.height, ILI9341_BLUE);
  }
}

// Verificar colisiones con plataformas
void checkMikuPlatformCollision() {
  MikuOnPlatform = false;
  for (int i = 0; i < platformCount; i++) {
    Platform platform;
    memcpy_P(&platform, &platforms[i], sizeof(Platform));
    if (MikuX + MikuWidth > platform.x &&
        MikuX < platform.x + platform.width &&
        MikuY + MikuHeight >= platform.y &&
        MikuY + MikuHeight <= platform.y + platform.height) {
      MikuJumping = false;
      MikuY = platform.y - MikuHeight; // Ajustar sobre la plataforma
      MikuOnPlatform = true;
      return;
    }
  }
}

// Lógica del salto de Miku
void handleMikuJump() {
  if (MikuJumping) {
    MikuY -= 5;
    MikuJumpHeight += 5;
    if (MikuJumpHeight >= 50) { // Altura máxima del salto
      MikuJumping = false;
    }
  } else if (!MikuOnPlatform && MikuY < 292) {
    MikuY += 5; // Gravedad
  }
}

// Configuración inicial
void setup() {
  Serial.begin(9600);
  Serial.println("Inicio del Setup");

  // Inicializar display
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  drawBackground();
  drawPlatforms();

  //tft.setCursor(0, 0);
  //tft.print("prueba");


  // Configurar pines de botones
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_JUMP, INPUT_PULLUP);

  // Configurar buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  // Dibujar Miku inicial
  drawMiku();
  Serial.println("Setup completado");
}

// Lógica principal
void loop() {
  clearMiku();

  // Leer botones
  if (digitalRead(BUTTON_RIGHT) == LOW) {
    MikuX += 5;
    if (MikuX + MikuWidth > 240) {
      MikuX = 240 - MikuWidth; // Mantener dentro del límite derecho
    }
    Serial.println("Miku se mueve a la derecha");
  }
  if (digitalRead(BUTTON_LEFT) == LOW) {
    MikuX -= 5;
    if (MikuX < 0) {
      MikuX = 0; // Mantener dentro del límite izquierdo
    }
    Serial.println("Miku se mueve a la izquierda");
  }
  if (digitalRead(BUTTON_JUMP) == LOW && !MikuJumping && MikuOnPlatform) {
    MikuJumping = true;
    MikuJumpHeight = 0;
    Serial.println("Miku salta");
  }

  handleMikuJump();
  checkMikuPlatformCollision();
  drawMiku();

  // Reproducir sonido ambiental
  playAmbientSound();
}
