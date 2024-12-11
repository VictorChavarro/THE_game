#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "TTMapa.h" // Imagen del fondo
#include "HatsuneMiku.h" // Imagen de Miku (se asume que esta imagen está en formato adecuado)

// Pines para el display
#define TFT_CS 6
#define TFT_DC 7
#define TFT_RST 10

// Botones
#define BUTTON_RIGHT 20
#define BUTTON_LEFT 19
#define BUTTON_JUMP 18

// Configuración del display
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Variables de Miku
int MikuX = 0; // Posición inicial
int MikuY = 292; // Ajustado sobre los bloques
int MikuWidth = 23;
int MikuHeight = 28;
bool MikuJumping = false;
int MikuJumpHeight = 0;
bool MikuOnPlatform = false;

// Configuración de las plataformas
struct Platform {
  int x, y, width, height;
};

Platform platforms[] = {
  {0, 292, 240, 28} // Plataforma en la parte inferior
};
int platformCount = sizeof(platforms) / sizeof(Platform);

// Funciones para dibujar elementos
void drawBackground() {
  // Dibujar el fondo (esto podría ser grande, ajusta para optimizar)
  tft.drawBitmap(0, 0, TTM, 240, 320, ILI9341_WHITE);
}

void drawMiku() {
  tft.drawBitmap(MikuX, MikuY, Miku, MikuWidth, MikuHeight, ILI9341_BLUE);
}

void clearMiku() {
  tft.fillRect(MikuX, MikuY, MikuWidth, MikuHeight, ILI9341_BLACK);
}

void drawPlatforms() {
  for (int i = 0; i < platformCount; i++) {
    tft.fillRect(platforms[i].x, platforms[i].y, platforms[i].width, platforms[i].height, ILI9341_BLUE);
  }
}

// Verificar colisiones con plataformas
void checkMikuPlatformCollision() {
  MikuOnPlatform = false;
  for (int i = 0; i < platformCount; i++) {
    if (MikuX + MikuWidth > platforms[i].x &&
        MikuX < platforms[i].x + platforms[i].width &&
        MikuY + MikuHeight >= platforms[i].y &&
        MikuY + MikuHeight <= platforms[i].y + platforms[i].height) {
      MikuJumping = false;
      MikuY = platforms[i].y - MikuHeight; // Ajustar sobre la plataforma
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
    if (MikuJumpHeight >= 50) { // Altura máxima
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
  tft.fillScreen(ILI9341_RED); // Comprobación inicial
  Serial.println("Display inicializado");

  drawBackground();
  drawPlatforms();

  // Configurar botones
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_JUMP, INPUT_PULLUP);
  Serial.println("Botones configurados");

  // Dibujar Miku inicial
  drawMiku();
  Serial.println("Miku dibujado");
}

// Lógica principal
void loop() {
  clearMiku();

  // Leer botones
  if (digitalRead(BUTTON_RIGHT) == LOW) {
    MikuX += 5;
    if (MikuX + MikuWidth > 240) {
      MikuX = 240 - MikuWidth; // Mantener dentro de los límites
    }
    Serial.println("Miku se mueve a la derecha");
  }
  if (digitalRead(BUTTON_LEFT) == LOW) {
    MikuX -= 5;
    if (MikuX < 0) {
      MikuX = 0; // Mantener dentro de los límites
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
  delay(30);
}
