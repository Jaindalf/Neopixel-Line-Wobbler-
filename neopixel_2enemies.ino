#include <Adafruit_NeoPixel.h>
#define LED_PIN 3
#define LED_COUNT 100

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

char instruc = 0;
int inAttack=0;
int lives=3;
char incoming;
int red1_steps = 0;
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
      int collisionCount;

  public:
    Enemy(int si,int le,int cc){
      startIndex=si;
      length=le;
      collisionCount=cc;

    }

    void setcolor(){
        strip.fill(strip.Color(255,0,0),startIndex-length+1,length);
    }
};
Player blue(0,3,3,0);
Enemy  red1(LED_COUNT-1,5,0);
Enemy  red2(LED_COUNT-1,1,0);

void setup() {
  strip.begin();
  strip.show();
  Serial.begin(9600);

  Serial.println("S--->BACKWARD\n A---->ATTACK\n W---->FORWARD");
}

void loop() {

    if(Serial.available()) {
      incoming = Serial.read();
    }
    if (incoming != 0){
       instruc = incoming;
    }

    if(instruc!=0){
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
    }

    strip.clear();
    blue.setcolor();
    red1.setcolor();
    if(red1_steps>=10){
        red2.setcolor();
        red2.startIndex-=1;
   
    }
    strip.show();


    if(blue.inAttack==1){
      blue.Attack();
      

      if(millis()-startAttack>1500){
        blue.inAttack=0;
      }
      
      if(blue.startIndex>=(red1.startIndex-red1.length)){
        strip.fill(strip.Color(0, 0, 255), 0, LED_COUNT);
        strip.show();
        Serial.println("YOU WON!!!");
        delay(2000);
        blue.startIndex=0;
      	red1.startIndex=LED_COUNT;
        red1_steps=0;
       // break;
        
      
      }
        
    }

    
    
    
    

    strip.show();

    delay(300);

    // lose condition
    if (blue.startIndex>=(red1.startIndex-red1.length)) {
      strip.fill(strip.Color(0, 255, 0), 0, LED_COUNT); // green = game over
      strip.show();
      blue.lives=blue.lives-1;
      delay(1500);
      strip.clear();
      blue.startIndex=0;
      red1.startIndex=LED_COUNT;
      red1_steps=0;
     // strip.show();

      //break;  // restart game
    }
    if(blue.lives==0){
      Serial.println("Game Over");
      blue.lives=3;
      red1_steps=0;
      //break;
    }
    blue.startIndex += 1;
    red1.startIndex  -= 1;
    red1_steps++; 
  }

