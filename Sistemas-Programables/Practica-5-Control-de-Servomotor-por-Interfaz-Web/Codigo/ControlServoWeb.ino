/*
  Practica 5 - Control de un servomotor mediante una interfaz web
  Arduino UNO R4 WiFi + servomotor en el pin 9.

  El Arduino levanta un servidor HTTP simple. La pagina que sirve tiene un control
  deslizante (slider); al moverlo, el navegador manda GET /servo?angulo=NN por
  fetch (sin recargar la pagina) y el Arduino mueve el servo a ese angulo en
  tiempo real.
*/

#include <WiFiS3.h>
#include <Servo.h>

const char* ssid = "NOMBRE_DE_TU_RED";
const char* password = "CONTRASENA_DE_TU_RED";

const int PIN_SERVO = 9;

WiFiServer server(80);
Servo servoMotor;
int anguloActual = 90;

void enviarPagina(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();

  client.println("<!DOCTYPE html><html><head><meta charset='utf-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<title>Control de Servomotor</title></head><body>");
  client.println("<h1>Control de Servomotor por Interfaz Web</h1>");
  client.print("<p>Angulo actual: <span id='valor'>");
  client.print(anguloActual);
  client.println("</span> grados</p>");
  client.print("<input type='range' min='0' max='180' value='");
  client.print(anguloActual);
  client.println("' id='slider' style='width:300px'>");

  // Al mover el slider se manda la peticion sin recargar la pagina.
  client.println("<script>");
  client.println("const slider = document.getElementById('slider');");
  client.println("const valor = document.getElementById('valor');");
  client.println("slider.addEventListener('input', () => {");
  client.println("  valor.textContent = slider.value;");
  client.println("  fetch('/servo?angulo=' + slider.value);");
  client.println("});");
  client.println("</script>");
  client.println("</body></html>");
}

void setup() {
  Serial.begin(9600);

  servoMotor.attach(PIN_SERVO);
  servoMotor.write(anguloActual);

  WiFi.begin(ssid, password);
  Serial.print("Conectando a ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado. Abre esta direccion en el navegador: http://");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String lineaActual = "";
  String primeraLinea = "";

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n') {
        if (primeraLinea.length() == 0) primeraLinea = lineaActual;
        if (lineaActual.length() == 0) break;  // linea en blanco: fin de encabezados
        lineaActual = "";
      } else if (c != '\r') {
        lineaActual += c;
      }
    }
  }

  // La peticion del slider llega como: GET /servo?angulo=120 HTTP/1.1
  int idx = primeraLinea.indexOf("angulo=");
  if (idx != -1) {
    int fin = primeraLinea.indexOf(' ', idx);
    String valor = primeraLinea.substring(idx + 7, fin);
    anguloActual = constrain(valor.toInt(), 0, 180);
    servoMotor.write(anguloActual);
    Serial.print("Angulo recibido: ");
    Serial.println(anguloActual);
  }

  enviarPagina(client);
  client.stop();
}
