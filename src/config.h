#ifndef config_h
#define config_h
//
// Paramètres de l'application
//
// ---------------------------------------------------------------- 
// A quel pourcentage de la puissance souscrite faut-il délester
// Pour les tests, on peut utiliser 5 (5%) qui permet de déclencher
// le délestage via l'allumage d'une plaque électrique
// En production, ce sera probablement 80%
// ---------------------------------------------------------------- 
//
const unsigned long PERCENT_DELESTAGE = 80;

// ---------------------------------------------------------------- 
// Durée minimale d'un délestage (1000 = 1sec)
// L'objectif est de ne pas rallumer le chauffage trop souvent
// pour éviter des alternances de délestages trop fréquents
// 300000 = 5mn
// 180000 = 3mn
// ---------------------------------------------------------------- 
const unsigned long TEMPS_DELESTAGE  =  240000; // 4mn

// ---------------------------------------------------------------- 
// Quelle période Tempo déclenche le délestage
// Voici les périodes tempo:        heures pleines heures creuses
//                   jour Bleu          HPJB           HCJB
//                   jour Blanc         HPJW           HCJW
//                   jour Rouge         HPJR           HCJR
// On se propose de délester aussi bien les HP jour blanc que
// les HP jour rouge
// ---------------------------------------------------------------- 
const char *PERIODE_DELESTAGE_W = "HPJW";
const char *PERIODE_DELESTAGE_R = "HPJR";

// ---------------------------------------------------------------- 
// Durée du debounce du bouton (1000 = 1sec)
// ---------------------------------------------------------------- 
const unsigned long TEMPS_DEBOUNCE = 1000; // 1sec

#endif