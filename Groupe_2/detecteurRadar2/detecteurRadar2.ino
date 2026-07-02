#include <LiquidCrystal.h>
#include <Servo.h>

const int TRIG_PIN=7;
const int ECHO_PIN=6;
const int SERVO_PIN=8;
const int BUZZER_PIN=10;
const int LED_PIN=13;

LiquidCrystal lcd(12,11,5,4,3,2);
Servo radarServo;

const int SEUIL_DETECTION= 30;  
const int VITESSE_SERVO= 1;  
const int ANGLE_MIN= 0;
const int ANGLE_MAX=180;

int  angle= ANGLE_MIN;
int  direction= 1;           
bool alerteActive=false;
bool servoEnPause = false;     
int anglePause = 0;

float mesurerDistance() {
  digitalWrite(TRIG_PIN,LOW);  //envoie d'un signal LOW sur la broche TRIG
  delayMicroseconds(2);  //Pause de 2 microsecondes 
  digitalWrite(TRIG_PIN,HIGH);  //on déclenche l'émission d'ondes ultrasonores
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW); // on ressoi

  long duree = pulseIn(ECHO_PIN, HIGH, 30000); 
  if (duree == 0) return 999;  //renvoi la valeur d'erreur pour indiquer pas d'objet          
  return duree*0.0343 / 2.0;  //vitesse du son dans l'air divisé par deux   (Distance = (Vitesse du son × Durée) / 2)
}

void afficherLCD(int ang, float dist) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Angle: ");
  lcd.print(ang);
  lcd.print((char)223); //pour afficher le symbole degre
  lcd.print("   ");

  lcd.setCursor(0, 1);
  if (dist >= 999) {
    lcd.print("Dist: ---  cm");
  } else {
    lcd.print("Dist: ");
    lcd.print((int)dist);
    lcd.print(" cm    ");
  }
}

void afficherAlerte(float dist, int ang) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("! OBSTACLE !");
  lcd.setCursor(0, 1);
  lcd.print("Angle: ");
  lcd.print(ang);
  lcd.print((char)223);
  lcd.print("  ");
  lcd.print((int)dist);  //affichage en entier
  lcd.print("cm");
}

void gererAlarme(bool activer) {
  alerteActive = activer;
  if (activer) {
    tone(BUZZER_PIN, 1000);   //son a 1khz
    digitalWrite(LED_PIN, HIGH);
  } else {
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, LOW);
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN, LOW);  //initialisation de la led
  radarServo.attach(SERVO_PIN);
  radarServo.write(ANGLE_MIN);

  lcd.begin(16,2);
  lcd.print("  RADAR ACTIF   ");
  lcd.setCursor(0, 1);
  lcd.print("  Initialisation... ");
  delay(1500);
}

void loop() {
  float distance = mesurerDistance(); // Mesure de la distance
  bool objetDetecte = (distance < SEUIL_DETECTION && distance > 2); // Vérifie si un objet est détecté entre 2cm et 30 cm
  
  if (objetDetecte) {      
    if (!servoEnPause) {  //Si le servo n'estt pas en pause on le met en pause
      servoEnPause = true;  
      anglePause = angle;  //on memorise l'angle
      gererAlarme(true);   //on allume l'alarme
      Serial.println("****OBSTACLE DETECTE - SERVO EN PAUSE****");
    }  
    afficherAlerte(distance, anglePause);
    
    Serial.print("!!! ALERTE !!! Angle: ");
    Serial.print(anglePause);
    Serial.print("°  |  Distance: ");
    Serial.print(distance);
    Serial.println(" cm  [SERVO EN PAUSE]");
    delay(100);
  }else{
    if(servoEnPause){
      servoEnPause=false;
      gererAlarme(false);
     }
    angle += direction;  //on augmente l'angle d'un degré
    if(angle>=ANGLE_MAX) direction=-1;
    if(angle<=ANGLE_MIN) direction=1;
    radarServo.write(angle);  //on positionne le servo a la position de l'angle
    delay(VITESSE_SERVO);
    afficherLCD(angle, distance);
    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print("°  |  Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    delay(50); 
  }
}
