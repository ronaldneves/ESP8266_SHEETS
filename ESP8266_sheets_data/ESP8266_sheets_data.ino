#include "ESP8266WiFi.h"
#include "HTTPSRedirect.h"

// ## variaveis
unsigned long previousMillis = 0;
const char* nome_wifi = "sua_rede_wifi"; // alterar para uma rede wi-fi real
const char* senha_wifi = "senha_rede_wifi"; // alterar para a senha da rede wi-fi

HTTPSRedirect* cliente;

String pega_id_implementacao = ""; // alterar com id da implementação do Google Apps Script
String id_implementacao = "";

const char* host = "script.google.com";
const int porta_http = 443;
String payload_dados = "{\"command\": \"adicionar_linha\", \"sheet_name\": \"Página1\", \"values\": ";
String payload;
String url = "";
int sensor1 = 0;


// ## setup
void setup() {
  Serial.begin(9600);
  setup_wifi(nome_wifi, senha_wifi);  
  setup_http(cliente, porta_http);
  checa_conexao(cliente, porta_http);
}


// ## loop
void loop() {
  sensor1 = random(0, 101);

  if (millis() - previousMillis >= 10000) {
    previousMillis = millis();
    checa_conexao(cliente, porta_http);
    envia_payload();
  }
}


// ## funções
void setup_wifi(const char* rede, const char* senha) {
  WiFi.begin(rede, senha);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(". ");
  }
  
  Serial.println(" ");
  Serial.print("Conectado na rede: ");
  Serial.println(rede);
}


void setup_http(HTTPSRedirect* cliente, int porta_http) {
  cliente = new HTTPSRedirect(porta_http);
  cliente->setInsecure();
  cliente->setPrintResponseBody(true);
  cliente->setContentTypeHeader("application/json");
  Serial.println("Conectando ao servidor Google. . .");

  bool flag = false;

  for (int i = 0; i < 5; i++) {
    int valor_retorno = cliente->connect(host, porta_http);

    if (valor_retorno == 1) {
      flag = true;
      Serial.println("Conectado ao servidor com sucesso.");
      break;
    } 
		else {
      Serial.println("Erro ao conectar no servidor.");
    }
  }

  if (!flag) {
    Serial.print("Erro ao conectar no host ");
    Serial.println(host);
  }

  delete cliente;
  cliente = nullptr;
}


void checa_conexao(HTTPSRedirect* cliente, int porta_http) {
  static bool flag = false;

  if (!flag) {
    cliente = new HTTPSRedirect(porta_http);
    cliente->setInsecure();
    cliente->setPrintResponseBody(true);
    cliente->setContentTypeHeader("application/json");
    flag = true;
  }

  if (cliente != nullptr) {
    if (!cliente->connected()) {
      cliente->connect(host, porta_http);
    }
  } else {
    Serial.println("Erro ao conectar no servidor http");
  }
}


void envia_payload() {
  payload = payload_dados + "\"" + String(sensor1) + "\"}";

  if (cliente->POST(url, host, payload)) {
    Serial.println("Dados enviados para o servidor.");
  } 
  
  else {
    Serial.println("Erro ao enviar dados para o servidor.");
  }
}