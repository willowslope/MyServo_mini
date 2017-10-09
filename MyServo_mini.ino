 
// ****************************************************************
// MyServoControler_mini
//  Ver0.00 2017.10.06
//  copyright 坂柳
//  Hardware構成
//  マイコン：wroom-02
//  IO:
//   P4,5,12,13 サーボ
// ****************************************************************


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <math.h>
#include <FS.h>

//*******************************************************************************
//* 定数、変数
//*******************************************************************************
//サーボ関連
#define ServoCH 4
Servo myservo[ServoCH];         //サーボ構造体
int servo_val[ServoCH];          //サーボ指令値

//サーバー
ESP8266WebServer server(80);  //webサーバー
IPAddress HOST_IP = IPAddress(192, 168, 0, 10);
IPAddress SUB_MASK = IPAddress(255, 255, 255, 0);

//ポート設定
#define ServoCH1 14
#define ServoCH2 12
#define ServoCH3 13
#define ServoCH4 15


//*******************************************************************************
//* プロトタイプ宣言
//*******************************************************************************
void servo_ctrl(void);

//*******************************************************************************
//* HomePage
//*******************************************************************************
// -----------------------------------
// Top Page
//------------------------------------
void handleTopPage() {
  Serial.println("TopPage");
  File fd = SPIFFS.open("/Propo.html","r");
  String html = fd.readString();
  fd.close();
  
  server.send(200, "text/html", html);
}
// -----------------------------------
// Not Found
//------------------------------------
void handleNotFound() {
  Serial.println("NotFound");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
// -----------------------------------
// Servo Control
//------------------------------------
bool ctrl_exec = false;
void handleCtrl() {
  Serial.println("CtrlPage");
  int i;
  char buff[10];
  String ch_str, servo_str;
  int t_servo_val[ServoCH];
  for (i = 0; i < ServoCH; i++) {
    ch_str = "SERVO" + String(i);
    if (server.hasArg(ch_str) ){
      servo_str = server.arg(ch_str);
      servo_val[i] = servo_str.toInt();
      if (servo_val[i] > 180) servo_val[i] = 180;
      if (servo_val[i] <   0) servo_val[i] = 0;
    }
  }
  server.send(200, "text/plain", "Ctrl");
  ctrl_exec = true;
}
//------------------------------
// Control関数
//------------------------------
void servo_ctrl(void) {
  if(!ctrl_exec) return;
  ctrl_exec = false;

  //Range変換
  int i;
  for (i = 0; i < ServoCH; i++) {
    myservo[i].write(servo_val[i]);
  }
}

// *****************************************************************************************
// * 初期化
// *****************************************************************************************
//------------------------------
// IO初期化
//------------------------------
void setup_io(void) {
  //init servo
  myservo[0].attach(ServoCH1);   // attaches the servo on GIO4 to the servo object
  myservo[1].attach(ServoCH2);   // attaches the servo on GIO5 to the servo object
  myservo[2].attach(ServoCH3);  // attaches the servo on GI12 to the servo object
  myservo[3].attach(ServoCH4);  // attaches the servo on GI13 to the servo object
}
// ------------------------------------
void setup_com(void) {      //通信初期化
  String ssid = "ESP_"+String(ESP.getChipId(),HEX);
  // シリアル通信
  Serial.begin(115200);

  // WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(HOST_IP, HOST_IP, SUB_MASK);
  WiFi.softAP(ssid.c_str());
}
// ------------------------------------
void setup_ram(void) {
  //サーボ初期値
  int i;
  for (i = 0; i < ServoCH; i++)
  {
    servo_val[i] = 90;
  }
}
// ------------------------------------
void setup_spiffs(void){
  SPIFFS.begin();
}
// ------------------------------------
void setup_http(void) {
  server.on("/", handleTopPage);
  server.on("/Ctrl", handleCtrl);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}
// ------------------------------------
void setup(void) {

  setup_io();
  setup_ram();
  setup_com();
  setup_spiffs();
  setup_http();
}
// *****************************************************************************************
// * Loop処理
// *****************************************************************************************
void loop(void) {
  server.handleClient();
  servo_ctrl();
}


