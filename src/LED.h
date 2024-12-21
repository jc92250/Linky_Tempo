#ifndef LED_h
#define LED_h

#include <Arduino.h>

// ---------------------------------------------------------------- 
// Les différentes LEDs
// (en plus de la led builtin)
// Note: elles ne sont pas forcément connectées à l'Arduino
// (la construction minimale n'a que la led builtin)
// ---------------------------------------------------------------- 
#define LED_DETECT 10 // allumée lors de la détection du mode de la TIC
#define LED_HIST 5    // allumée si TIC en mode historique (le seul mode qu'on supporte)
#define LED_STD 8     // allumée si TIC en mode standard (on ne le supporte pas)

#define LED_ANNUL 17  // allumée si on a annulé le délestage

// ----------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------
void setup_LED() {
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_DETECT, LOW);
  digitalWrite(LED_HIST, LOW);
  digitalWrite(LED_STD, LOW);
  digitalWrite(LED_ANNUL, LOW);
}

void LED_DETECT_ON() {
  digitalWrite(LED_DETECT, HIGH);
}
void LED_DETECT_OFF() {
  digitalWrite(LED_DETECT, LOW);
}
void LED_HIST_ON() {
  digitalWrite(LED_HIST, HIGH);
}
void LED_STD_ON() {
  digitalWrite(LED_STD, HIGH);
}
void LED_ANNUL_ON() {
  digitalWrite(LED_ANNUL, HIGH);
}
void LED_ANNUL_OFF() {
  digitalWrite(LED_ANNUL, LOW);
}
// ----------------------------------------------------------------
// Gestion de la LED_BUILTIN
// - elle va basculer durant la détection du mode de la TIC
// - elle va clignoter indéfiniment deux fois toutes les deux
//   secondes si la TIC n'est pas dans le mode "historique"
// - elle va flasher une fois toutes les deux secondes si le
//   délestage est activé
// ----------------------------------------------------------------

void LED_Flashx2Forever() {
    while(true) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(10);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(10);
      digitalWrite(LED_BUILTIN, LOW);
      delay(2000);
    }
}

void LED_Flashx1Lent() {

}

void LED_Eteinte() {
  digitalWrite(LED_BUILTIN, LOW);
}

#endif
