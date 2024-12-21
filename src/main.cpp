//
// ---------------------------------------------------------------- 
// Programme de délestage
// Inputs: les données de la TIC, données envoyées par le compteur
//         Linky
// Output: la commande du fil pilote en mode Hors-gel
//
// Pour minimiser la consommtion, on ne va utiliser que la LED intégrée.
// Elle va clignoter rapidement lors de la lecture initiale de la TIC en vue
// de déterminer si on est en mode "historique" ou en mode "standard".
// Si on est en mode "standard", vu qu'on ne supporte pas ce mode, la LED va
// alors flasher deux fois ("non non") toutes les deux secondes indéfiniment.
// Il faudra faire un RESET de l'arduino pour déclancher une nouvelle détection.
// Si on est en mode "historique" qui est le mode supporté, il y a deux cas:
// - nominal: la LED reste éteinte
// - délestage pour cause de HPJW ou HPJR: la LED clignote
// - délestage pour cause de surconsommation: la LED clignote
//
// TODO: implifier le code de LibTeleInfo pour ne conserver que les
//       infos utiles de la TIC (pour économiser de la RAM)
// TODO: temporisation à l'allumage (si on ne branche pas tout en
//       même temps pour ce qui est de I1, I2 et A)
//       faire cela dans le setup_test_led par exemple, en faisant
//       clignoter les deux LEDs en alternance
// TODO: initialiser la sortie fil pilote en hors-gel ?
// TODO: que se passe t-il si l'ensemble des radiateurs seuls
//       provoque un délestage?
//       ce n'est pas possible car l'ensemble fait 4500W
//       séjour  = 1000W + 1500W
//       bureau  = 1000W
//       chambre = 1000W
// TODO: définir de quelles LEDs on a besoin
//       - trame reçue (qui confirme qu'on est bien connecté à la TIC)
//       - délestage en JPJW ou HPJR
//       - délestage car surconsommation
// TODO: fiabiliser la détection du mode histo
//       (quid si I1-I2 ne sont pas branchés lors de la détection)
//       éventuellement, hardcoder le mode histo
// TODO: vérifier que ça marche si
//       1) histo est hardcodé et
//       2) I1-I2 sont branchés plus tard
// ---------------------------------------------------------------- 
//
#include <Arduino.h>
#include "LED.h"
#include "LibTeleinfo.h"
#include "setup_detect_TIC_mode.h"
#include "callbacks.h"

// ---------------------------------------------------------------- 
// Gestion de la TIC
// Le flux d'info est reçu via le Serial (port RX)
// ---------------------------------------------------------------- 

// Dans quel mode est la TIC (standard ou historique ?) 
_Mode_e mode_tic;

// L'objet qui permet de récupérer le détail de la TIC
// Plus d'info ici: https://hallard.me/libteleinfo/
TInfo tinfo;

// ---------------------------------------------------------------- 
// Commande des fils pilote
// ---------------------------------------------------------------- 
#define PIN_PILOTE 7


// ---------------------------------------------------------------- 
// Annulation du délestage Jour Blanc:
// Par défaut, on déleste en HPJW
// Un bouton permet d'annuler ce délestage
// Cette annulation n'est effective que si on est déjà en délestage
// Et elle court jusqu'à la prochaine heure creuse
// Si on ré-appuie, on déleste à nouveau
// Une LED jaune allumée indique qu'on n'est pas en délestage
// La LED est manipulée via LED_ANNUL_ON et LED_ANNUL_OFF
//
// PIN_BTN_ANNUL: Le bouton permettant d'annuler le délestage HPJW
//                Il mets à la masse cette entrée
// annulDelestageHPJW: false par défaut
//                     passe à true si l'utilisateur clicke le btn
//                     repasse à false au prochain click
//                     passe à false à la prochaine HP
// debounce: qd a été observé le dernier changement d'état du btn
//           tout changement dans les 0,5sec qui suivent sera ignoré
// ---------------------------------------------------------------- 
#define PIN_BTN_ANNUL 14
bool annulDelestageHPJW;
uint32_t debounce;

// ---------------------------------------------------------------- 
// setup des différentes pins de l'arduino
// - en input pour la TIC
// - en output pour la commande des fils pilote
// ---------------------------------------------------------------- 
void setup_pins() {

  // Commande des fils pilote
  pinMode(PIN_PILOTE, OUTPUT);
  // Les LED
  pinMode(LED_DETECT, OUTPUT);
  pinMode(LED_STD, OUTPUT);
  pinMode(LED_HIST, OUTPUT);
  pinMode(LED_ANNUL, OUTPUT);
  // Le bouton
  pinMode(PIN_BTN_ANNUL, INPUT_PULLUP);
}

// ---------------------------------------------------------------- 
// setup
// ---------------------------------------------------------------- 
void setup() {

  setup_pins();
  setup_LED();
  
  // init interface série suivant mode TIC
  // TODO: forcer le mode historique car:
  // - on sait qu'on est en mode historique
  // - on ne veut pas prendre le risque que le code bascule en standard si la détection du mode historique échoue
	mode_tic = setup_detect_TIC_mode();

  // On ne supporte que le mode "historique"
  // Si on n'arrive pas à détecter ce mode, on ne peut rien faire
  if (mode_tic != TINFO_MODE_HISTORIQUE) {
    // On ne supporte pas le mode standard
    LED_STD_ON();
    // On se met en mode Hors-gel pour limiter la consommation
    digitalWrite(PIN_PILOTE, HIGH); // TODO: LOW or HIGH
    // Et la LED rouge va flasher indéfiniment pour montrer le problème
    LED_Flashx2Forever();
  }
  LED_HIST_ON();

   // init interface TIC
  tinfo.init(mode_tic);

  // soyons appelés lorsqu'on a reçu une nouvelle trame
  tinfo.attachADPS(AttachADPSCallback); // TODO: utile ?
  tinfo.attachUpdatedFrame(UpdatedFrameCallback);
  tinfo.attachData(AttachDataCallback);

  // on déleste en HPJW par défaut
  annulDelestageHPJW = false;
  debounce = millis();
}

// ---------------------------------------------------------------- 
// loop
// ---------------------------------------------------------------- 
// Si délestage, c'est depuis quand, basé sur milli()
uint32_t startDelestage;

void loop() {
  uint32_t currentTime = millis();

  // réception d'un caractère de la TIC
  // on laisse tinfo essayer de décoder l'ensemble
  if (Serial.available()) tinfo.process(Serial.read());

  // On ne peut annuler le délestage qu'en HPJW
  // On s'assure donc que ette annulation ne reste pas active trop longtemps
  if (!delestage_HPJW) annulDelestageHPJW = false;

  // pression sur le bouton ?
  // le bouton n'est actif qu'en HPJW
  if (delestage_HPJW) {
    if (currentTime - debounce > TEMPS_DEBOUNCE) {
      if (digitalRead(PIN_BTN_ANNUL) == LOW) { // bouton pressé
        // on bascule l'annulation ou non du délestage HPJW
        debounce = currentTime;
        annulDelestageHPJW = !annulDelestageHPJW;
      }
    }
  }

  // on indique (via la LED dédiée) si on déleste ou pas en HPJW
  digitalWrite(LED_ANNUL, annulDelestageHPJW ? HIGH : LOW);

  // on indique au fil pilote si on déleste ou pas
  if ((delestage_HPJW && !annulDelestageHPJW) || delestage_HPJR || delestage_CONSO) {
    // On doit délester
    LED_Flashx1Lent();
    digitalWrite(PIN_PILOTE, HIGH);
    // En on repousse la fin du délestage
    startDelestage = currentTime;
    // Comme on ne sait pas si on était déjà en mode délestage, on définit les pins de sortie en conséquence
  } else if (currentTime - startDelestage > TEMPS_DELESTAGE) {
    // On n'a plus besoin de délester
    // et ceci, depuis assez longtemps (TEMPS_DELESTAGE)
    // Ce temps d'attente est pour éviter de basculer trop souvent
    // Par exemple, lorsque le four ou une plaque de cuisson est en maintient de température
    // On peut enfin annuler le délestage
    LED_Eteinte();
    digitalWrite(PIN_PILOTE, LOW);
  }
 /*!delestage_HPJR && !delestage_CONSO &&*/ 
}
