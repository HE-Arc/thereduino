/*
 Theremin à commande optique destiné à l'initiation à la micro informatique et l'électronique.

 Développé à la http://ingenierie.he-arc.ch/

 Haut-parleur 8 ohms connecté à D8  (via 220 ohms en série avec 10uF)
 Photo transistor "note" sur A7     (pull-up de 47kohms)
 Photo transistor "volume"          (On-Off) sur A0 (pull-up de 47kohms)

Dans le menu Outils de l'IDE :
- Type de carte : "Arduino Nano"
- Processeur :    "ATmega328P (OldBootloader)"
- Port :          "COM X"           (choisir le plus grand nombre proposé pour X)
*/

#include <math.h>         // bibliothèques des fonctions mathématiques de base

/*
 * VARIABLES : Déclaration des variables qui seront utilisées dans le code
 */
long frMin = 440;         // fréquences minimale et maximale en Hz                                        (par défaut : 440 et 1200)
long frMax = 1200;

int minLight;             // valeurs minimale et maximale lues sur le phototransistor (PT)
int maxLight;         

long onOff;               // valeur de seuil pour la commande de la note (lue par le PT)

int perVibrato = 15;      // vibrato                                                                      (par défaut : 15 et 10)
int ampVibrato = 10;

bool invFreq = false;     // note grave (au lieu d'aigüe) quand le phototransistor A7 est couvert         (par défaut : false)
bool invOnOff = false;    // coupe le son (au lieu de l'activer) quand le phototransistor A0 est couvert  (par défaut : false)

bool mute = true;         // coupe le son si la note est proche des extrêmes (+/- seuil)                  (par défaut : true)
bool snap = true;         // joue une note de la gamme chromatique (au lieu d'une fréquence)              (par défaut : true)

int seuil;                // valeur utilisée par mute
int notes[127];           // tableau des fréquences des notes de la gamme chromatique

/*
 * FONCTIONS PRINCIPALES
 */
 //SETUP : Exécution au démarrage ou reset
void setup() 
{
  // 1. Calibrage
  calibrage();

  //2. Initialisation des seuils
  onOff = (minLight + maxLight) / 2;      // le seuil se situe au milieu des deux valeurs lues lors du calibrage
  seuil = (maxLight - minLight) / 8;      // les valeurs proches des seuils minimum et maximum ne produiront pas de son

  //3. Initialisation des notes
  initNotes();
}

//LOOP : Exécution en boucle
void loop() 
{
  onOff = (minLight + maxLight) / 2;
  // Serial.println(onOff);
 
  //1. Récupération des valeurs envoyées par les phototranonsistors
  int lectureNote = analogRead(A7);       // hauteur de la note
  int lectureOnOff = analogRead(A0);      // volume du son (ON/OFF)
  
  bool noteOn = true;

  //2. Conversion de la lecture en fréquence
  int freq = map(lectureNote, minLight, maxLight, frMin, frMax);

  //3. Sortie du son
  if (invFreq)                            // si vrai, on inverse la fréquence (initilisé à FALSE par défaut)
  {
    freq = frMin + frMax - freq;          
  }
  if (invOnOff)                           // vrai si le phototransistor est couvert  (initialisé à FALSE par défaut)
  {
    noteOn = (lectureOnOff < onOff);       
  }
  else                                    // vrai si le phototransistor est découvert
  {
    noteOn = (lectureOnOff > onOff);       
  }
  if (snap)                               // si vrai, on prend la valeur la plus proche (initialisé à TRUE par défaut)     
  {
    freq = closest(freq);                 
  }
  if (noteOn)                             // si vrai, on joue la note
  {
    tone(8, freq, 10);                    
  }
}

/*
 * FONCTIONS : fonctions annexes utilisées dans les deux fonctions principales
 */
// CALIBRAGE : Exécution lors du SETUP afin de calibrer les phototransistors. Au branchement, on mesure la valeur des phototransistors recouverts et decouverts
void calibrage()
{
  //1. Récupération de la valeur de la lumière lorsque les phototransistors sont couverts
  tone(8, 440, 200);              // bip sonore qui annonce le début du calibrage (La grave)
  delay(1200);                    // délai pour couvrir les phototransistors
  minLight = analogRead(A7);      // récupération de la valeur minimum

  //2. Récupération de la valeur de la lumière lorsque les phototransistors sont découverts
  tone(8, 880, 200);              // bip sonore qui annonce le début du calibrage (La aigu)
  delay(1200);                    // délai pour découvrir les phototransistors
  maxLight = analogRead(A7);      // récupération de la valeur maximum

  //3. Vérification des valeurs obtenues - dans le cas où la mesure s'est faite à l'envers, on inverse les valeurs
  if (maxLight < minLight) 
  {
    int tmp  = minLight;
    minLight = maxLight;
    maxLight = tmp;
  }  
}

//INITIALISATION DES NOTES : Exécution lors du SETUP après la phase de CALIBRATION, initilise le tableau des fréquences de notes
void initNotes() 
{
  //1. Définition de la note de base (La3 - 440Hz)
  int a = 440;

  //2. Remplissage du tableau de notes : d'après http://subsynth.sourceforge.net/midinote2freq.html
  for (int i = 0; i < 127; i++)
  {
    notes[i] = a * pow(2, (i - 69) / 12.);
  }
}

//CLOSEST : Exécution dans le LOOP, permet de trouver la fréquence la plus proche d'une des notes contenues dans le tableau
int closest(int freq) 
{
  //1. Définition des variables utilisées
  int lo = 0, hi = 126, mid;

  //2. Recherche de la bonne fréquence à renvoyer
  while (hi - lo > 1) 
  {
    mid = floor((lo + hi) / 2);
    
    if (notes[mid] < freq)                    // réduction de la zone de recherche sur la moitié du tableau
    {
      lo = mid ;                           
    }
    else 
    {
      hi = mid;
    }
  }

  if (freq - notes[lo] <= notes[hi] - freq)   // retourne la fréquence la plus proche de celle qui a été mesurée
  {
    return notes[lo];                     
  }
  else
  {
    return notes[hi];
  }
}
