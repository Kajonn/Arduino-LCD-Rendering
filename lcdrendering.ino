/*

*/

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// setting animation speed (delay between frames)
int x = 10000;

constexpr int lcdWidth = 4;
constexpr int lcdHeight = 2;
constexpr int charPxWidth = 5;
constexpr int charPxHeight = 8;
constexpr int customChars = 8;
constexpr int pixelWidth = lcdWidth*charPxWidth;
constexpr int pixelHeight = lcdHeight*charPxHeight;

struct GenChar {  
  byte b[charPxHeight];
  void print() const{
    Serial.println(); 
    for(int py = 0; py < charPxHeight; py++) {    
     Serial.println(); 
     for(int px = 0; px < charPxWidth; px++) {
       Serial.print( (b[ py ] >> (charPxWidth-1)-px) & B1 );
       Serial.print(" ");
     }
     Serial.println();
     Serial.flush();
   }
  }
};

constexpr int getOffset(int xlcd, int ylcd) {
  return (xlcd*charPxWidth)+(ylcd*pixelWidth*charPxHeight);
}

constexpr int ix(int i0, int px, int py) {
  return i0 + (py*pixelWidth)+(px);
}

inline GenChar getChar(int x, int y, byte *image) {
   const int i0 = getOffset(x, y);
   //Serial.println();
   GenChar rchar;   
   for(int py = 0; py < charPxHeight; py++) {
     //Serial.println();
     byte value = 0;
     for(int px = 0; px < charPxWidth; px++) {
        const auto imagePixelValue = image[ ix( i0, px, py ) ];
        //Serial.print( imagePixelValue );
        //Serial.print(" ");
        value |= (imagePixelValue & B1) << (charPxWidth-1)-px;
     }
     //Serial.println();
     //Serial.flush();
     rchar.b[ py ] = value;
   }
   
   return rchar;
}

//image 0,0 is 
void displayImage(byte *image, LiquidCrystal &lcd) {
  
  int currentChar = 0;
  for(int x = 0; x < lcdWidth; x++)
  {
    for(int y = 0; y < lcdHeight; y++)
    {
      auto genChar = getChar(x,y,image);
      //genChar.print();
     
      lcd.createChar( currentChar, genChar.b);
      lcd.setCursor(x,y);
      lcd.write((byte)currentChar);
      
      currentChar++;
      if(currentChar==customChars) {
        currentChar = 0;        
      }      
      
    }
  }
  
}

struct BouncingBall {
  float x = 1;
  float y = 1;
  float vx = 20.0;
  float vy = 20.0;
  
  float xMaxBorder = pixelWidth-1;
  float xMinBorder = 1;
  float yMaxBorder = pixelHeight-1;
  float yMinBorder = 1;
  
  void force(float x, float y) {
    
  }
  
  void update(float dt) {
    
    x += vx*dt;
    y += vy*dt;
    
    if(x >= xMaxBorder || x < xMinBorder) {      
      vx = -vx;  
    }
    
    if(y >= yMaxBorder || y < yMinBorder) {      
      vy = -vy;  
    }
  }
  
  void draw(byte *targetImage) {  
    targetImage[ static_cast<int>(x)+(static_cast<int>(y)*pixelWidth) ] = 1;
  }
};

void drawBorder(byte *image) {
    for(int x = 0; x < pixelWidth; x++)
  {  
    for(int y = 0; y < pixelHeight; y++) 
    {
      if( y==0 || x == 0 || y == pixelHeight-1 || x == pixelWidth-1)
        image[ x+(y*pixelWidth) ] = 1;
      else   
        image[ x+(y*pixelWidth) ] = 0;
    }
  }
}

void printImage(byte *image) {
    Serial.println();
  for(int y = 0; y < pixelHeight; y++)   
  {  
    for(int x = 0; x < pixelWidth; x++)
    {
      Serial.print(image[ x+(y*pixelWidth) ]);
      Serial.print(" ");
    }
    Serial.println();    
  }
  Serial.flush();
}

const int nBalls = 20;
BouncingBall ball[nBalls];

byte *image;

void setup() {  
  const int imageSize = pixelWidth * pixelHeight;
  image=new byte[imageSize];

  Serial.begin(9600);
  
  for(int i = 0; i<nBalls; i++) {
    ball[i].x = random(1, pixelWidth-1);
    ball[i].y = random(1, pixelHeight-1);
    ball[i].vx = random(5, 20);
    ball[i].vy = random(5, 20);    
  }
  
	// set up the LCD's number of columns and rows:
	lcd.begin(16,2);
	// clear the LCD screen:
	lcd.clear();
	displayImage(image, lcd);	
}

constexpr float stepTime = 30.0;

void loop() {
  
  drawBorder(image);  
    
  for(int i = 0; i<nBalls; i++) {
    ball[i].update(stepTime/1000.0);
    ball[i].draw(image);  
  }
    
  lcd.clear();
  displayImage(image, lcd);	
  
  delay(stepTime);
}     
