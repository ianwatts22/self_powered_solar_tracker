#include <SPI.h>
#include <EEPROM.h>
#include <boards.h>
#include <RBL_nRF8001.h>
#include <Servo.h>
Servo servoH;
Servo servoV;
int anglet;
int angleb;
int top = analogRead(A4);
int bottom = analogRead(A5);
int left = analogRead(A3);
int right = analogRead(A2);
int temp = analogRead(A0);
int irr = ((top+bottom+left+right)/4);
char temp_char[15];
char irr_char[15];
char anglet_char[15];
char angleb_char[15];


void setup() {
  ble_set_name("LED");
  ble_begin();
  pinMode(A0, INPUT);   //temperature sensor
  pinMode(A1, INPUT);   //solar output
  pinMode(A2, INPUT);   //right
  pinMode(A3, INPUT);   //left
  pinMode(A4, INPUT);   //top
  pinMode(A5, INPUT);   //bottom
  servoH.attach(10);
  servo.attach(6);
  Serial.begin(57600); }


void loop() {
  horiz();
  vert();
  bluetooth();
  lowPower(); }


void lowPower() {
  int power = analogRead(A1);
  while (power < 3.8) {
    servoV.write(20);
    servoV.detach();
    servoH.detach();
    power = analogRead(A1);
    delay(10000); }
  servoH.attach(10);
  servoV.attach(6); }


void vert() {
  int maxDiff = 20;
  int sunFound = 60;
  int maxPos = 106;
  int servopos = servoV.read();
  Serial.println("here 2");
  int dTime = 25;
  int dTime2 = 50;
 
  int top = analogRead(A4);
  int bottom = analogRead(A5);
  if (servopos >= maxPos) {
    servopos = servoV.read();
    servoV.write(maxPos - 1); }


  int n = 0;
  while (top < sunFound && bottom < sunFound && n <= 0) {
    servopos = servoV.read();
    for (int i = 0; i < 105; i++) {
      servoV.write(i);
      top = analogRead(A4);
      bottom = analogRead(A5);
      delay(dTime2); } 
    top = analogRead(A4);
    bottom = analogRead(A5);
    n++; }
        
  while ((bottom - top) > maxDiff && servopos <= maxPos  && servopos >= 0) {    //r < l
    top = analogRead(A4);
    bottom = analogRead(A5);
    servopos = servoV.read();
    servoV.write(servopos + 1);
    delay(dTime);
    bluetooth(); }
  while ((top - bottom) > maxDiff && servopos < maxPos && servopos >= 0) {    //l < r
    top = analogRead(A4);
    bottom = analogRead(A5);
    servopos = servoV.read();
    servoV.write(servopos - 1);
    delay(dTime);
    bluetooth(); } }


void horiz() {
  int maxDiff = 20;
  int sunFound = 60;
  int maxPos = 20;
  int servopos = servoH.read();
  Serial.println("here 0");
  int dTime = 25;
  int dTime2 = 50;
 
  int right = analogRead(A2);
  int left = analogRead(A3);


  int servoVert = 72;
  int n = 0;
  int minMax = -1;
  while (right < sunFound && left < sunFound && n <=2) {
    servopos = servoH.read();
    minMax = pow(-1,n);
    servoVert = servoV.read();
    if (servoVert > 0) {
      servoV.write(servoVert - 36*n);
      servoVert = servoV.read(); }
    servoH.write(90+90*minMax);
    if (servoH.read() == 0) { servoH.write(1); }
    if (servoH.read() == 180) { servoH.write(179); }
    delay(dTime);
    while (right < sunFound && left < sunFound && servoH.read() != 180 && servoH.read() != 0) {
      servoH.write(servopos - minMax*pastFlat());
      servopos = servoH.read();
      bluetooth();
      right = analogRead(A2);
      left = analogRead(A3);
      delay(dTime2); }
    right = analogRead(A2);
    left = analogRead(A3);
    n++; }
  
  while ((left - right) > maxDiff) {    //r < l
    right = analogRead(A2);
    left = analogRead(A3);
    servopos = servoH.read();
    servoH.write(servopos - pastFlat());
    delay(dTime); 
    bluetooth(); }
  while ((right - left) > maxDiff) {    //l < r
    right = analogRead(A2);
    left = analogRead(A3);
    servopos = servoH.read();
    servoH.write(servopos + pastFlat());
    delay(dTime);
    bluetooth(); } }


int pastFlat() {
  int pastFlat = 1;
  int vertpos = servoV.read();
  if (vertpos < 20) { return -1; }
  else { return 1; } }




unsigned char buf[16] = {0};
unsigned char len = 0;
 
void bluetooth() {
  temp = analogRead(A0);
  irr = (analogRead(A2)+analogRead(A3)+analogRead(A4)+analogRead(A5))/4;
  anglet = servoV.read();
  angleb = 110 - servoH.read();
  
  itoa(temp, temp_char, 10);
  itoa(irr, irr_char, 10);
  itoa(anglet, anglet_char, 10);
  itoa(angleb, angleb_char, 10);
  
  if ( ble_connected() ) {
    ble_write('T'); ble_write('e');  ble_write('m');  ble_write('p'); ble_write('e'); ble_write('r'); ble_write('a');
    ble_write('t'); ble_write('u'); ble_write('r'); ble_write('e'); ble_write(':'); ble_write(' ');
    int i = 0;
    while(temp_char[i]) { ble_write(temp_char[i++]); }
  
    ble_write('\n');
 
    ble_write('I'); ble_write('r'); ble_write('r'); ble_write('a'); ble_write('d'); ble_write('i'); ble_write('a');
    ble_write('n'); ble_write('c'); ble_write('e'); ble_write(':'); ble_write(' ');
    i = 0;
    while(irr_char[i]) { ble_write(irr_char[i++]); }
      
    ble_write('\n');
 
    ble_write('S'); ble_write('u'); ble_write('n'); ble_write(' '); ble_write('P'); ble_write('o'); ble_write('s');        
    ble_write('i'); ble_write('t'); ble_write('i'); ble_write('o'); ble_write('n'); ble_write(':'); ble_write(' ');
    i = 0;
    while(anglet_char[i]) { ble_write(anglet_char[i++]); }
    ble_write(',');
    i = 0;
    while(angleb_char[i]) { ble_write(angleb_char[i++]); } }
 
  ble_do_events(); }
