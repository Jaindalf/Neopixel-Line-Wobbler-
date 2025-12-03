#include <Adafruit_NeoPixel.h>
#define LED_PIN 3
#define LED_COUNT 21

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

char instruc = 0;
int inAttack=0;
int lives=3;


unsigned long startAttack = 0;


char recvOneChar() {
  if (Serial.available() > 0) {
    return Serial.read();
  }
  return 0;
}

class Player{
  public:
      int startIndex;
      int length;
      int lives;
      int inAttack;

  public:

     Player(int s,int le,int li,int at){
      startIndex=s;
      length=le;
      lives=li;
      inAttack=at;
    }

      void setcolor(){
        for(int i=startIndex;i<length+startIndex;i++){
          strip.setPixelColor(i,strip.Color(0,0,255));
        }
      }

      void Attack(){
        for(int i=startIndex;i<length+startIndex+2;i++){
          strip.setPixelColor(i,strip.Color(0,0,255));
        }
      }


      

};


class Enemy{
  public:
      int startIndex;
      int length;

  public:
    Enemy(int si,int le){
      startIndex=si;
      length=le;

    }

    void setcolor(){
        strip.fill(strip.Color(255,0,0),startIndex-length,startIndex);
    }
};
Player blue(0,3,3,0);
Enemy  red(LED_COUNT,5);

void setup() {
  strip.begin();
  strip.show();
  Serial.begin(9600);

  Serial.println("S--->BACKWARD\n A---->ATTACK\n W---->FORWARD");
}

void loop() {

  //int blue = 0;                   // blue starts at left
  //int red = LED_COUNT - 1;        // red starts at right

  while (true) {

    // read serial input
    char incoming = recvOneChar();
    if (incoming != 0) instruc = incoming;

    // if player pressed 'S'
    if (instruc == 'S') {
      if (blue.startIndex > 0) blue.startIndex --;    // push blue backward
      instruc = 0;                // IMPORTANT reset
    }
    
    
    if (instruc == 'W') {
      if (blue.startIndex<LED_COUNT) blue.startIndex++;    // push blue backward
      instruc = 0;                // IMPORTANT reset
    }
    
    
    if (instruc == 'A') {
       blue.inAttack=1;
       startAttack=millis();
       instruc=0;

                  
    }

    // move both automatically
    blue.startIndex += 1;
    red.startIndex  -= 1;

    // draw LEDs
    strip.clear();
    blue.setcolor();
    red.setcolor();
   

    if(blue.inAttack==1){
      blue.Attack();
      

      if(millis()-startAttack>1500){
        blue.inAttack=0;
      }
      
      if(blue.startIndex>=(red.startIndex-red.length)){
        strip.fill(strip.Color(0, 0, 255), 0, LED_COUNT);
        strip.show();
        Serial.println("YOU WON!!!");
        delay(2000);
        blue.startIndex=0;
      	red.startIndex=LED_COUNT;
        return;
        
      
      }
        
    }
    strip.show();

    delay(300);

    // lose condition
    if (blue.startIndex>=(red.startIndex-red.length)) {
      strip.fill(strip.Color(0, 255, 0), 0, LED_COUNT); // green = game over
      strip.show();
      blue.lives=blue.lives-1;
      delay(1500);
      strip.clear();
      blue.startIndex=0;
      red.startIndex=LED_COUNT;
     // strip.show();

      return;  // restart game
    }
    if(blue.lives==0){
      Serial.println("Game Over");
      blue.lives=3;
      return;
    }
    
  }
}
