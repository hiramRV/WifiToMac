/*
    This sketch shows how to use WiFi event handlers.
    Written by Markus Sattler, 2015-12-29.
    Updated for new event handlers by Ivan Grokhotkov, 2017-02-02.
    Modificado por Steven Rubio, 2019-07-19
*/
#include <ESP8266WiFi.h>
#include <stdio.h>

#ifndef APSSID
#define APSSID "esp8266"
#define APPSK  "esp8266"
#endif

const char* ssid     = APSSID;
const char* password = APPSK;
WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;
WiFiEventHandler probeRequestPrintHandler;
WiFiEventHandler probeRequestBlinkHandler;

//------------------------------------------------------
//Constantes a modificar
//Cantidad de Macs que quiero almacenar
const int canT = 1000;        //Cantidad Maxima de MACs a almacenar
const int RSSI_MAX = -120;    //RSSI Maximo a tolerar
String MACs[canT];
int Contador;
String item2;
bool MAC_Almacenada;
//------------------------------------------------------
void setup() {
  Contador = 0;
  Serial.begin(115200);
  WiFi.persistent(false);
  // Set up an access point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  //wifi.PHYMODE_B --> WIFI_PHY_MODE_11B, Mayor Rango
  //wifi.PHYMODE_G --> WIFI_PHY_MODE_11G, Rango Medio
  //wifi.PHYMODE_N --> WIFI_PHY_MODE_11N, Rango Corto
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);
  // Register event handlers.
  stationConnectedHandler = WiFi.onSoftAPModeStationConnected(&onStationConnected);
  stationDisconnectedHandler = WiFi.onSoftAPModeStationDisconnected(&onStationDisconnected);
  probeRequestPrintHandler = WiFi.onSoftAPModeProbeRequestReceived(&onProbeRequestPrint);
}

void onStationConnected(const WiFiEventSoftAPModeStationConnected& evt) {
  Serial.print("Station connected: ");
  Serial.println(macToString(evt.mac));
}

void onStationDisconnected(const WiFiEventSoftAPModeStationDisconnected& evt) {
  Serial.print("Station disconnected: ");
  Serial.println(macToString(evt.mac));
}

void onProbeRequestPrint(const WiFiEventSoftAPModeProbeRequestReceived& evt) {
  // Para limitar el area en que voy a reconocer Macs
  // [0,-40] --> Ideal
  // [-40,-60] ---> Tasa de transferencia estable. 
  // [-60,-70] ---> Enlace bueno
  // [-70,-80] ---> Enlace medio-bajo
  // [-80, XX] ---> Señal mínima aceptable
  
  MAC_Almacenada = true;
  item2 = String("_");
  //Ajusto la distancia a la que voy a filtrar los datos. 
  if ((evt.rssi) > RSSI_MAX){
        /*
        Serial.print("Probe de: ");
        Serial.print(macToString(evt.mac));
        Serial.print(" RSSI: ");
        Serial.println(evt.rssi);
        */
        //Guardo los caracteres de la MAC
        item2=macToString(evt.mac);
        
        //Reviso si ya la tengo almacenada
        if(MAC_Almacenada==true && Contador<canT){
          for (int i = 0; i<(canT-1); i++){
            if(item2==MACs[i]){
              MAC_Almacenada=false;
              }
            }   
          }
        //Si no la tengo almacenada, la guardo y despliego
        if(MAC_Almacenada && Contador<canT){
          MACs[Contador]=item2;
          Serial.print("MAC #");
          Serial.print(Contador+1);
          Serial.print(", Dirección: ");
          Serial.print(item2);
          Serial.print(" RSSI: ");
          Serial.println(evt.rssi);
          Contador++;
          }
        /*Si quiero comprobar con un dispositivo de prueba
        if(item2=="4c:66:41:17:ad:45"){
          Serial.print("Dispositivo de prueba, RSSI:");
          Serial.println(evt.rssi);
        }*/
  }
}

void loop() {
  /*Si quiero imprimir todas las MACs almacenadas luego de cierto tiempo
  Serial.print("MACs");
  for (int i = 0;(canT-1);i=i+1){
    Serial.println(MACs[0]);
  }
  */
  delay(1000);
}
String macToString(const unsigned char* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}
