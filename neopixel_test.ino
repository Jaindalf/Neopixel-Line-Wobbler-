#include <Adafruit_NeoPixel.h>
#define LED_PIN 3
#define LED_COUNT 21

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

char instruc = 0;
int inAttack = 0;
int lives = 3;

unsigned long startAttack = 0;

class Player
{
public:
  int startIndex;
  int length;
  int lives;
  int inAttack;

public:
  Player(int s, int le, int li, int at)
  {
    startIndex = s;
    length = le;
    lives = li;
    inAttack = at;
  }

  void setcolor()
  {
    for (int i = startIndex; i < length + startIndex; i++)
    {
      strip.setPixelColor(i, strip.Color(0, 0, 255));
    }
  }

  void Attack()
  {

    this->length += 2;
    for (int i = startIndex; i < length + startIndex; i++)
    {
      strip.setPixelColor(i, strip.Color(0, 0, 255));
    }
  }

};

class Enemy
{
public:
  int startIndex;
  int length;

public:
  Enemy(int si, int le)
  {
    startIndex = si;
    length = le;
  }

  void setcolor()
  {
    strip.fill(strip.Color(255, 0, 0), startIndex - length, startIndex);
  }

};

Player blue(0, 3, 3, 0);
Enemy red(LED_COUNT - 1, 5);

char recvOneChar()
{
  if (Serial.available() > 0)
  {
    return Serial.read();
  }
  return 0;
}


 




void setup()
{
  strip.begin();
  strip.show();
  Serial.begin(9600);

  Serial.println("S--->BACKWARD\n A---->ATTACK\n W---->FORWARD");
}

int checkCollision(Player& blue,Enemy& red)
{
  if (blue.startIndex >= (red.startIndex - red.length))
  {
    return 1;
  }

  else
    return -1;
}
void loop()
{
   // read serial input
    char incoming = recvOneChar();
    if (incoming != 0)
      instruc = incoming;

    // if player pressed 'S'
    if (instruc == 'S')
    {
      if (blue.startIndex > 0){
        blue.startIndex--;
      Serial.println("Moving Backward"); // push blue backward
         
      } 
      instruc = 0;                                   // IMPORTANT reset
    }

    if (instruc == 'W')
    {
      if (blue.startIndex + blue.length < LED_COUNT){
        blue.startIndex++;
      Serial.println("Moving Forward");
      }
      instruc = 0; // IMPORTANT reset
    }

    if (instruc == 'A')
    {
      if(blue.inAttack==0){
      blue.inAttack = 1;
      startAttack = millis();

      Serial.println("Attacking");
      }

      else if (blue.inAttack==1)
      {
        Serial.println("Spamming Attack");
      }
      instruc = 0;
    }

    // move both automatically
    blue.startIndex += 1;
    red.startIndex -= 1;

    // draw LEDs
    strip.clear();
    blue.setcolor();
    red.setcolor();

    if (blue.inAttack == 1)
    {
      blue.Attack();
    }
    if (millis() - startAttack > 1500)
    {
      blue.inAttack = 0;
      blue.length -= 2;
    }

    if (checkCollision(blue,red) == 1 && blue.inAttack == 1)
    {

      strip.fill(strip.Color(127, 127, 127), 0, LED_COUNT); // WIN
      strip.show();
      Serial.println("YOU WON");

      delay(2000);
      strip.clear();
      blue.startIndex = 0;
      red.startIndex = LED_COUNT - 1;
      return;
    }

    else if (checkCollision(blue,red) == 1 && blue.inAttack == 0)
    {
      strip.fill(strip.Color(255, 0, 0), 0, LED_COUNT); // lose
      strip.show();
      Serial.println("YOU LOST");

      delay(2000);
      strip.clear();
      blue.startIndex = 0;
      red.startIndex = LED_COUNT - 1;
      return;
    }
  
  strip.show();

  delay(300);
}
