# Linky_Tempo
Délestage fil pilote

L'objectif de ce petit montage est de délester mon chauffage électrique (via ses fils pilote):
- en période HPJR (heure pleine jour rouge)
- en période HPJW (heure pleine jour blanc) avec possibilité d'annuler le délestage
- en cas de surconsommation

L'arduino est connecté à la sortie TIC (télé-information client) du compteur Linky.
Le compteur doit être configuré en mode "Historique" (c'est son mode par défaut)
Parmi les informations reçues, on s'intéresse:
- à la période en cours de l'abonnement Tempo
- à la consommation instantanée

Lorsque la période en cours est HPJR (heure pleine jour rouge), l'Arduino envoie un signal de délestage aux fils pilote des radiateurs.
Durant cette période, il n'y a aucun moyen d'annuler ce délestage.

Lorsque la période en cours est HPJW (heure pleine jour blanc), l'Arduino envoie également un signal de délestage aux fils pilote des radiateurs.
Durant cette période, il est possible d'annuler le délestage via un appui sur le bouton poussoir.
Le délestage est alors annulé jusqu'à la fin de la période HPJW (typiquement, dès que le compteur passe en heure creuse)
Un second appui sur le bouton poussoir réactive le délestage.
Ce bouton poussoir n'a d'effet qu'en période HPJW.

Par ailleurs, le délestage peut également être activé lors d'une surconsomation (typiquement, si un four et des plaques électriques sont allumés alors que le chauffage est par ailleur activé)
Ce délestage intervient dès que la consommation électrique instantanée atteint 80% de la puissance souscrite.

Lorsque le délestage est activé, il dure au minimum 4mn (pour tenir compte de l'aspect intermittent de la consommation électrique d'un four ou d'une plaque électrique)

Le fichier config.h définit les paramètres dont les valeurs peuvent être ajustées.
