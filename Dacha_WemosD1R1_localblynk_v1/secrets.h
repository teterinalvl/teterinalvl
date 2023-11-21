#ifndef secrets_h
#define secrets_h

const char* wifi_ssid = "Keenetic_79";
const char* wifi_password = "WY2joH78";


// vp - virtualpin, hp - hardwarepin
#define vp_BLYNK_1 V1
#define vp_BLYNK_2 V2
#define vp_BLYNK_3 V3
#define vp_BLYNK_4 V4
#define vp_BLYNK_5 V5
#define vp_BLYNK_10 V10
#define vp_BLYNK_11 V11
#define vp_BLYNK_12 V12
#define vp_BLYNK_13 V13
#define vp_BLYNK_14 V14
// #define hp_pinD1 5           //D1
// #define hp_pinD2 4           //D2
// #define hp_pinD3 0           //D3
// #define hp_pinD4 2           //D4
// #define hp_pinD5 14          //D5
// #define hp_pinD6 12          //D6
// #define hp_pinD7 13          //D7
// #define hp_pinA0 A0          //ADC A0

#define pinONE_WIRE_BUS_1 D3                    //Pin для OneWire
#define pinONE_WIRE_BUS_2 D6                    //Pin для OneWire

char blynk_auth_token[] = "pVHSS_hZXn1t1ORW3_N-r8PMX3Y5p1oS";   // BLYNK Auth Token Dacha
char blynk_local_server_ip[] = "192.168.1.250";                 // BLYNK Local Server device IP address.
int blynk_local_server_port = 8080;                 // BLYNK Local Server device IP address.


//MQ2PIN A0
//DHTPIN 0        	// D3
#endif