/*
 Theremin à commande optique destiné à l'initiation à la micro informatique et l'électronique

 Développé à la http://ingenierie.he-arc.ch/

 Haut-parleur 8 ohms connecté à D8 (via 220 ohms en série avec 10uF)
 Photo transistor "note" sur A7 (pull-up de 47kohms)
 Photo transistor "volume" (On-Off) sur A0 (pull-up de 47kohms)

Dans le menu Outils de l'IDE :
- Type de carte : "Arduino Nano"
- Processeur : "ATmega328"
- Port : "COM3" (ou autre proposé)
*/
#include <math.h>                 // fonctions mathématiques

// Déclaration des variables
  long frMin = 440, frMax = 1200; // fréquence minimale et maximale en Hz
  int minLight, maxLight;         // valeurs min et max lues sur le phototransistor (PT)
  long onOff;                     // valeur de seuil pour la commande de la note (lue par le PT)
  
  // Pour le vibrato
  int perVibrato = 15, ampVibrato = 10;
  
  /* invFreq  : note grave (au lieu d'aigüe) quand le phototransistor A7 est couvert
   * invOnOff : coupe le son (au lieu de l'activer) quand le phototransistor A0 est couvert
   * mute     : coupe le son si la note est proche des extrêmes (+/- seuil)
   * seuil    : valeur utilisée par mute
   * snap     : joue une note de la gamme chromatique (au lieu d'une fréquence)
   * notes[]  : tableau des fréquences des notes de la gamme chromatique */
  bool invFreq = false, invOnOff = false, mute = true, snap = true;
  int seuil, notes[127]; // fréquences des notes gamme chromatique

// Code exécuté à la mise sous tension ou Reset
void setup() {
  // Calibrage : couvrir le phototransistor A7 après le son grave, le découvrir après le son aigü
  calibrage();

  // Le seuil ON/OFF est au milieu des 2 valeurs lues au calibrage
  onOff = (minLight + maxLight) / 2;

  // Les valeurs proches de seuil de minLight et maxLight ne produiront pas de son
  seuil = (maxLight - minLight) / 8;

  initNotes();
}

// Code exécuté le plus souvent possible
void loop() {
  // Lecture des phototransistors
  int lectureNote = analogRead(A7); // lecture de la hauteur de la note
  int lectureOnOff = analogRead(A0); // lecture de la commande du volume (ON OFF)
  bool noteOn = true;

  // Conversion de la lecture en fréquence
  int freq = map(lectureNote, minLight, maxLight, frMin, frMax);

  // Joue la note
  if (invFreq) freq = frMin + frMax - freq;
  if (invOnOff)  noteOn = (lectureOnOff < onOff);   // vrai quand PT couvert
  else           noteOn = (lectureOnOff > onOff);   // vrai quand PT découvert
  if (snap) freq = closest(freq);                   // "arrondi" à la note la plus proche
  if (noteOn) tone(8, freq, 10);                    // joue une fréquence
}

// Calibrage
// Au branchement, on mesure la valeur des phototransistors recouverts et decouverts
void calibrage()
{
  tone(8, 440, 200);// Annonce le début du calibrage
  delay(1200);
  minLight = analogRead(A7); // valeur phototransistor sur A7 recouvert

  tone(8, 880, 200);// Annonce le début du calibrage
  delay(1200);
  maxLight = analogRead(A7); // valeur phototransistor sur A7 découvert

  // si le PT était d'abord découvert, on échange les valeurs
  if (maxLight < minLight) {
    int tmp  = minLight;
    minLight = maxLight;
    maxLight = tmp;
  }  
}

// Initialise le tableau des fréquences des notes
// d'après http://subsynth.sourceforge.net/midinote2freq.html
void initNotes() {
  int a = 440;   // Fréquence du LA6 : 440 Hz
  for (int i = 0; i < 128; i++)
    notes[i] = a * pow(2, (i - 69) / 12.);
}

// Retourne la valeur la plus proche de freq contenue dans le tableau des notes
int closest(int freq) {
  int lo = 0, hi = 127, mid;

  while (hi - lo > 1) {
    mid = floor((lo + hi) / 2);
    if (notes[mid] < freq) lo = mid ;
    else hi = mid;
  }

  if (freq - notes[lo] <= notes[hi] - freq) return notes[lo];
  else return notes[hi];
}
