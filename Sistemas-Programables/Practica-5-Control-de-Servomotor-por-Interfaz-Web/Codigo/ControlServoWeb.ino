#include <WiFiS3.h>
#include <Servo.h>

// ---------- CAMBIA ESTO ----------
const char* SSID = "NOMBRE_DE_TU_WIFI";
const char* PASS = "TU_PASSWORD";
// ---------------------------------

const int PIN_SERVO = 9;

Servo servo;
WiFiServer servidor(80);

int angulo = 90;

const char PAGINA[] PROGMEM = R"HTML(
<!DOCTYPE html><html><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Control de servo</title>
<style>
body{font-family:system-ui,sans-serif;background:#1c1c1a;color:#eee;
     display:flex;flex-direction:column;align-items:center;
     justify-content:center;height:100vh;margin:0}
h1{font-size:20px;font-weight:500}
#val{font-size:64px;font-weight:600;margin:10px 0}
input[type=range]{width:80%;max-width:400px;height:40px}
.btns{margin-top:24px}
button{font-size:16px;padding:10px 20px;margin:4px;border:0;
       border-radius:6px;background:#3a3a38;color:#eee}
button:active{background:#5a5a58}
</style></head><body>
<h1>Angulo del servo</h1>
<div id="val">90&deg;</div>
<input type="range" id="sl" min="0" max="180" value="90">
<div class="btns">
  <button onclick="ir(0)">0&deg;</button>
  <button onclick="ir(90)">90&deg;</button>
  <button onclick="ir(180)">180&deg;</button>
</div>
<script>
const sl=document.getElementById('sl'),val=document.getElementById('val');
let t=null;
function mandar(a){fetch('/set?a='+a);}
function ir(a){sl.value=a;val.textContent=a+'°';mandar(a);}
sl.addEventListener('input',()=>{
  val.textContent=sl.value+'°';
  clearTimeout(t);
  t=setTimeout(()=>mandar(sl.value),40);
});
</script></body></html>
)HTML";

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);

  servo.attach(PIN_SERVO);
  servo.write(angulo);

  Serial.print("\nConectando a ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASS);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 40) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nNO SE PUDO CONECTAR");
    Serial.println("Revisa nombre, password, y que la red sea de 2.4 GHz");
    return;
  }

  Serial.println("\nConectado");
  Serial.print(">>> ABRE ESTA DIRECCION EN EL NAVEGADOR:  http://");
  Serial.println(WiFi.localIP());

  servidor.begin();
}

void loop() {
  WiFiClient cliente = servidor.available();
  if (!cliente) return;

  String peticion = cliente.readStringUntil('\n');
  while (cliente.available()) cliente.read();

  int pos = peticion.indexOf("/set?a=");

  if (pos >= 0) {
    int valor = peticion.substring(pos + 7).toInt();

    if (valor >= 0 && valor <= 180) {
      angulo = valor;
      servo.write(angulo);
      Serial.print("Angulo: ");
      Serial.println(angulo);
    }

    cliente.println("HTTP/1.1 200 OK");
    cliente.println("Content-Type: text/plain");
    cliente.println("Connection: close");
    cliente.println();
    cliente.println(angulo);
  }
  else {
    cliente.println("HTTP/1.1 200 OK");
    cliente.println("Content-Type: text/html");
    cliente.println("Connection: close");
    cliente.println();
    cliente.print(PAGINA);
  }

  delay(5);
  cliente.stop();
}
