#include <WiFi.h>
#include <WebServer.h>

//--------------------------------------------- (PINS) ------------
#define RENA 25
#define RIN1 26
#define RIN2 27
#define RIN3 32
#define RIN4 33
#define RENB 14

//--------------------------------------------- (VARIABLES) --------
int speedLinear = 150;
int speedTurn = 150;
WebServer server(80);

const char* ap_ssid = "MEPER_ESP32";
const char* ap_password = "12345678";

//--------------------------------------------- (MOTOR FUNCTIONS) ---
void forward() {
  digitalWrite(RIN1, HIGH);
  digitalWrite(RIN2, LOW);
  analogWrite(RENA, speedLinear);
  digitalWrite(RIN3, HIGH);
  digitalWrite(RIN4, LOW);
  analogWrite(RENB, speedLinear);
}

void backward() {
  digitalWrite(RIN1, LOW);
  digitalWrite(RIN2, HIGH);
  analogWrite(RENA, speedLinear);
  digitalWrite(RIN3, LOW);
  digitalWrite(RIN4, HIGH);
  analogWrite(RENB, speedLinear);
}

void right() {
  digitalWrite(RIN1, LOW);
  digitalWrite(RIN2, HIGH);
  analogWrite(RENA, speedTurn);
  digitalWrite(RIN3, HIGH);
  digitalWrite(RIN4, LOW);
  analogWrite(RENB, speedTurn);
}

void left() {
  digitalWrite(RIN1, HIGH);
  digitalWrite(RIN2, LOW);
  analogWrite(RENA, speedTurn);
  digitalWrite(RIN3, LOW);
  digitalWrite(RIN4, HIGH);
  analogWrite(RENB, speedTurn);
}

void stopMotors() {
  digitalWrite(RIN1, LOW);
  digitalWrite(RIN2, LOW);
  digitalWrite(RIN3, LOW);
  digitalWrite(RIN4, LOW);
  analogWrite(RENA, 0);
  analogWrite(RENB, 0);
}

//--------------------------------------------- (WEB HANDLERS) ---
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
  <title>MEPER</title>
  <style>
    * { box-sizing: border-box; }
    body {
      margin: 0;
      min-height: 100vh;
      background: #f0f0f0;
      font-family: Arial, sans-serif;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 10px;
    }
    .container {
      width: 100%;
      max-width: 500px;
      height: 90vh;
      max-height: 700px;
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      background: #fff;
      border-radius: 20px;
      box-shadow: 0 0 20px rgba(0,0,0,0.1);
      padding: 20px;
    }
    .top-bar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 0 5px;
      margin-bottom: 10px;
    }
    .title {
      font-size: 24px;
      font-weight: bold;
      color: #333;
    }
    .speed-indicators {
      display: flex;
      gap: 15px;
      font-size: 16px;
      font-weight: bold;
      color: #333;
    }
    .speed-indicators span {
      background: #f0f0f0;
      padding: 4px 10px;
      border-radius: 12px;
    }
    .main-area {
      display: flex;
      flex: 1;
      align-items: center;
      justify-content: space-around;
      gap: 20px;
      min-height: 0;
    }
    .left-buttons {
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 20px;
    }
    .right-speed {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      gap: 40px;   /* AUMENTADO de 20px para 40px */
      height: 100%;
    }
    .bottom-row {
      display: flex;
      justify-content: flex-end;
      gap: 40px;
      margin-top: 20px;
      padding: 0 10px;
    }
    .button-group {
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 5px;
    }
    .btn {
      width: 240px;
      height: 240px;
      border: none;
      border-radius: 20px;
      background: #4CAF50;
      box-shadow: 3px 3px 8px rgba(0,0,0,0.2);
      cursor: pointer;
      touch-action: manipulation;
      transition: transform 0.1s;
    }
    .btn:active {
      transform: scale(0.90);
      background: #45a049;
    }
    .label {
      font-size: 16px;
      font-weight: bold;
      color: #333;
      user-select: none;
    }
    .speed-wrapper {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      width: 100%;
    }
    .speed-slider-horizontal {
      -webkit-appearance: none;
      appearance: none;
      width: 200px;
      height: 10px;
      background: #ccc;
      border-radius: 5px;
      outline: none;
      cursor: pointer;
    }
    .speed-slider-horizontal::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 30px;
      height: 30px;
      border-radius: 50%;
      background: #4CAF50;
      cursor: pointer;
    }
    .speed-slider-horizontal::-moz-range-thumb {
      width: 30px;
      height: 30px;
      border-radius: 50%;
      background: #4CAF50;
      cursor: pointer;
      border: none;
    }
    .slider-label {
      font-size: 14px;
      font-weight: bold;
      color: #555;
      margin-top: 4px;
    }
    @media (max-width: 500px) {
      .btn { width: 180px; height: 180px; border-radius: 15px; }
      .left-buttons { gap: 15px; }
      .bottom-row { gap: 25px; }
      .speed-slider-horizontal { width: 150px; }
      .title { font-size: 20px; }
      .speed-indicators { font-size: 14px; gap: 10px; }
      .right-speed { gap: 30px; }   /* ajustado para telas médias */
    }
    @media (max-width: 400px) {
      .btn { width: 140px; height: 140px; border-radius: 12px; }
      .left-buttons { gap: 10px; }
      .bottom-row { gap: 15px; }
      .speed-slider-horizontal { width: 120px; }
      .right-speed { gap: 20px; }   /* ajustado para telas pequenas */
    }
  </style>
</head>
<body>
<div class="container">
  <div class="top-bar">
    <div class="title">MEPER</div>
    <div class="speed-indicators">
      <span>Linear: <span id="linearValue">150</span></span>
      <span>Turn: <span id="turnValue">150</span></span>
    </div>
  </div>

  <div class="main-area">
    <div class="left-buttons">
      <div class="button-group">
        <button class="btn" ontouchstart="sendCmd('forward')" ontouchend="sendCmd('stop')" onmousedown="sendCmd('forward')" onmouseup="sendCmd('stop')"></button>
        <span class="label">Forward</span>
      </div>
      <div class="button-group">
        <button class="btn" ontouchstart="sendCmd('backward')" ontouchend="sendCmd('stop')" onmousedown="sendCmd('backward')" onmouseup="sendCmd('stop')"></button>
        <span class="label">Backward</span>
      </div>
    </div>

    <div class="right-speed">
      <div class="speed-wrapper">
        <input type="range" min="0" max="255" value="150" class="speed-slider-horizontal" id="linearSlider" oninput="updateLinearSpeed(this.value)">
        <div class="slider-label">Linear</div>
      </div>
      <div class="speed-wrapper">
        <input type="range" min="0" max="255" value="150" class="speed-slider-horizontal" id="turnSlider" oninput="updateTurnSpeed(this.value)">
        <div class="slider-label">Turn</div>
      </div>
    </div>
  </div>

  <div class="bottom-row">
    <div class="button-group">
      <span class="label">Left</span>
      <button class="btn" ontouchstart="sendCmd('left')" ontouchend="sendCmd('stop')" onmousedown="sendCmd('left')" onmouseup="sendCmd('stop')"></button>
    </div>
    <div class="button-group">
      <span class="label">Right</span>
      <button class="btn" ontouchstart="sendCmd('right')" ontouchend="sendCmd('stop')" onmousedown="sendCmd('right')" onmouseup="sendCmd('stop')"></button>
    </div>
  </div>
</div>

<script>
  function sendCmd(cmd) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/" + cmd, true);
    xhr.send();
  }
  
  function updateLinearSpeed(value) {
    document.getElementById('linearValue').textContent = value;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/speed?value=" + value, true);
    xhr.send();
  }
  
  function updateTurnSpeed(value) {
    document.getElementById('turnValue').textContent = value;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/turnSpeed?value=" + value, true);
    xhr.send();
  }
</script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleSpeed() {
  if (server.hasArg("value")) {
    speedLinear = server.arg("value").toInt();
    Serial.print("Velocidade linear alterada para: ");
    Serial.println(speedLinear);
  }
  server.send(200, "text/plain", "OK");
}

void handleTurnSpeed() {
  if (server.hasArg("value")) {
    speedTurn = server.arg("value").toInt();
    Serial.print("Velocidade de giro alterada para: ");
    Serial.println(speedTurn);
  }
  server.send(200, "text/plain", "OK");
}

void handleCommand() {
  String command = server.uri();
  if (command == "/forward") forward();
  else if (command == "/backward") backward();
  else if (command == "/left") left();
  else if (command == "/right") right();
  else if (command == "/stop") stopMotors();
  server.send(200, "text/plain", "OK");
}

//--------------------------------------------- (SETUP) ------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RENA, OUTPUT);
  pinMode(RIN1, OUTPUT);
  pinMode(RIN2, OUTPUT);
  pinMode(RIN3, OUTPUT);
  pinMode(RIN4, OUTPUT);
  pinMode(RENB, OUTPUT);

  stopMotors();

  Serial.println("Iniciando Access Point...");
  WiFi.softAP(ap_ssid, ap_password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.print("SSID: ");
  Serial.println(ap_ssid);
  Serial.println("Conecte seu telefone a esta rede e acesse http://192.168.4.1");

  server.on("/", handleRoot);
  server.on("/speed", handleSpeed);
  server.on("/turnSpeed", handleTurnSpeed);
  server.on("/forward", handleCommand);
  server.on("/backward", handleCommand);
  server.on("/left", handleCommand);
  server.on("/right", handleCommand);
  server.on("/stop", handleCommand);

  server.begin();
  Serial.println("Servidor web iniciado!");
}

//--------------------------------------------- (MAIN LOOP) ---------
void loop() {
  server.handleClient();
  delay(10);
}
