#include <Arduino.h>

#include "config/device_config.h"
#include "config/thresholds.h"
#include "config/wifi_config.h"
#include "network/http_sender.h"
#include "pins.h"
#include "sensors/current_sensor_sim.h"
#include "sensors/humidity_sensor.h"
#include "sensors/temperature_sensor.h"

namespace {
TemperatureSensor g_temperatureSensor(PIN_DS18B20);
HumiditySensor g_humiditySensor(PIN_DHT22);
CurrentSensorSim g_currentSensorSim;
HttpSender g_httpSender(WIFI_SSID, WIFI_PASSWORD, API_URL);

unsigned long g_lastPostMs = 0;
}

void setup() {
  Serial.begin(115200);
  delay(300);

  Serial.println("Inicializando firmware de monitoramento...");

  g_temperatureSensor.begin();
  g_humiditySensor.begin();
  g_currentSensorSim.begin();
  g_httpSender.begin();

  Serial.println("Inicialização concluída.");
  Serial.println("-> dentro do intervalo de postagem, aguardando 10 segundos...");
}

void loop() {
  Serial.begin(115200);
  
  unsigned long now = millis();
  if (now - g_lastPostMs < POST_INTERVAL_MS) {
    //Serial.printf("now=%lu, g_lastPostMs=%lu, diff=%lu, POST_INTERVAL_MS=%lu\n", now, g_lastPostMs, now - g_lastPostMs, POST_INTERVAL_MS);
    delay(50);
    return;
  }
  Serial.println("-> intervalo de postagem atingido, coletando dados e enviando...");

  g_lastPostMs = now;

  // Leitura dos sensores físicos.
  float ds18b20TempC = g_temperatureSensor.readCelsius();
  float dht22Humidity = g_humiditySensor.readHumidity();
  float dht22TempC = g_humiditySensor.readTemperatureCelsius();

  // Leitura de corrente simulada enquanto o sensor real não está disponível.
  float simulatedCurrentA = g_currentSensorSim.readAmperes();

  TelemetryPayload payload;
  payload.deviceId = DEVICE_ID;
  payload.uptimeMs = now;
  payload.ds18b20TemperatureC = ds18b20TempC;
  payload.dht22TemperatureC = dht22TempC;
  payload.dht22HumidityPct = dht22Humidity;
  payload.simulatedCurrentA = simulatedCurrentA;

  Serial.println("-> Dados coletados, preparando para envio...");
  Serial.println("");

  // Envio HTTP POST em JSON a cada 10 segundos.
  bool ok = g_httpSender.postTelemetry(payload);

  Serial.println("");
  // Exemplo de uso dos thresholds locais (ainda não altera fluxo principal).
  const bool tempCritical = ds18b20TempC >= TEMP_CRITICAL_C || dht22TempC >= TEMP_CRITICAL_C;

  Serial.println("---- TELEMETRIA ENVIADA ----");
  Serial.printf(
      "Payload => DS18B20: %.2fC | DHT22 Temp: %.2fC | DHT22 Umidade: %.2f%% | Corrente(sim): %.2fA | envio=%s | tempCritical=%s\n",
      ds18b20TempC,
      dht22TempC,
      dht22Humidity,
      simulatedCurrentA,
      ok ? "OK" : "FALHOU",
      tempCritical ? "SIM" : "NAO");
  Serial.println("");
}
