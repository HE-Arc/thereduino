/*
  Theremin

  Theremin à commande optique destiné à l'initiation à la micro informatique et l'électronique

  Développé à la HE-Arc

  Haut-parleur 8 ohms connecté à D8 (via 220 ohms en série avec 10uF
  photo transistor "note" sur A7 (pull-up de 47 kohms)
  photo transistor "volume" (On-Off) sur A0 (pull-up de 47 kohms)

*****************************
  PROGRAMME DE TEST DU MATERIEL
*****************************

*/



void setup() {
  Serial.begin(9600);
  //  TEST DU HP

  digitalWrite(13, HIGH); // allume la LED pour indiquer la calibration
  tone(8, 880, 1000);
  delay(1000);
  tone(8, 900, 1000);
  delay(1000);
  tone(8, 1200, 1000);
  digitalWrite(13, LOW);
}

void loop() {
  // lecture des phototransistors
  int LectureNote = analogRead(A7); // lecture de la hauteur de la note
  int LectureON_OFF = analogRead(A0); // lecture de la commande du volume (ON OFF)

  Serial.print("A7 = "); Serial.print(LectureNote); Serial.print(" A0 = "); Serial.println(LectureON_OFF);
}






