// Placa ESP32 Dev Module

#include "OV7670.h"

//#include <Adafruit_GFX.h>    // Core graphics library
//#include <Adafruit_ST7735.h> // Hardware-specific library

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClient.h>
#include "BMP.h"

const int SIOD = 21; //SDA
const int SIOC = 22; //SCL

const int VSYNC = 34;
const int HREF = 35;

const int XCLK = 32;
const int PCLK = 33;

const int D0 = 27;
const int D1 = 26;
const int D2 = 25;
const int D3 = 15;
const int D4 = 14;
const int D5 = 13;
const int D6 = 12;
const int D7 = 4;

#define ssid        "Whistler-2.4Ghz"
#define password    "Whistler2017!"

OV7670 *camera;

WiFiMulti wifiMulti;
WiFiServer server(80);

unsigned char bmpHeader[BMP::headerSize];

void serve()
{
String  WebPage =  "<!DOCTYPE html>\n";
        WebPage +=  "<html>\n";
          WebPage += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"; 
          WebPage += "<head>\n";
            WebPage += "<title>MyBot Camera</title><style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}</style>";
            WebPage += "<style>html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}img{transform: rotate(-270deg)}</style>\n"; //Se necessário dentro do Style: img{transform: rotate(-90deg)}
          WebPage += "</head>\n";
          WebPage += "<body>\n";
            WebPage += "<table align='center'>";
              WebPage += "<tr><td><h1>Camera - MyBot</h1></td></tr>\n";
              WebPage += "<tr><td>\n";
                WebPage += "<p><img id='a' style=height:480px; width:640px; src='/camera' onload='this.style.display=\"initial\";var b = document.getElementById(\"b\"); b.style.display=\"none\"; b.src=\"camera?\"+Date.now();'>";
                WebPage += "<img id='b' style=height:480px; width:640px; style='display:none' src='/camera' onload='this.style.display=\"initial\"; var a = document.getElementById(\"a\"); a.style.display=\"none\"; a.src=\"camera?\"+Date.now();'></p>";
              WebPage += "</td></tr>\n";
            WebPage += "</table>\n";
         WebPage += "</body>\n";   
        WebPage +=  "</html>\n";

  WiFiClient client = server.available();
  if (client) 
  {
    //Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        //Serial.write(c);
        if (c == '\n') 
        {
          if (currentLine.length() == 0) 
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(WebPage);
            client.println();
            break;
          } 
          else 
          {
            currentLine = "";
          }
        } 
        else if (c != '\r') 
        {
          currentLine += c;
        }
        
        if(currentLine.endsWith("GET /camera"))
        {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:image/bmp");
            client.println();
            
            client.write(bmpHeader, BMP::headerSize);
            client.write(camera->frame, camera->xres * camera->yres * 2);
        }
      }
    }
    // close the connection:
    client.stop();
    //Serial.println("Client Disconnected.");
  }  
}

void setup() 
{
  Serial.begin(115200);

  wifiMulti.addAP(ssid, password);
  Serial.println("Connecting Wifi...");
  if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
  }
  
  camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  BMP::construct16BitHeader(bmpHeader, camera->xres, camera->yres);
  
  server.begin();
}

void loop()
{
  camera->oneFrame();
  serve();
}
