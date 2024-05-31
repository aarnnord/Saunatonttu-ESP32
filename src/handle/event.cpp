
#include <Arduino.h>
#include <UniversalTelegramBot.h>

#include "../../.env.h"
#include "app.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "ruuvi_tag.h"

bool kiuas_before = false;
float temperature_before = 0.0;
bool ready_before = false;

#define BUF_SIZE (1024)

float getTemperature() {
  uart_port_t uart_num = UART_NUM_1;  // Use UART1
  int tx_io_num = GPIO_NUM_17;        // Example TX pin
  int rx_io_num = GPIO_NUM_16;        // Example RX pin

  // const int uart_num = UART_NUM_1;
  uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
  };
  // Configure UART1 parameters
  uart_param_config(uart_num, &uart_config);
  // Set UART1 pins(TX: IO4, RX: I05)
  uart_set_pin(uart_num, tx_io_num, rx_io_num, UART_PIN_NO_CHANGE,
               UART_PIN_NO_CHANGE);

  // Install UART driver (we don't need an event queue here)
  // In this example we don't even use a buffer for sending data.
  uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0);

  char data[BUF_SIZE];

  // Read data from UART
  int length = uart_read_bytes(uart_num, (float *)data, BUF_SIZE - 1,
                               100 / portTICK_PERIOD_MS);
  if (length > 0) {
    // Null-terminate the received string
    data[length] = '\0';
    // Convert the received string to a float
    float temperature = strtof(data, NULL);
    // Process the temperature as needed
    printf("Received Temperature: %.2f\n", temperature);

    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay to avoid flooding

    return temperature;
  }
}

void handleEvent(bool kiuas, float temperature, UniversalTelegramBot bot) {
  if (kiuas_before != kiuas) {
    Serial.println("Kiuas changed from " + String(kiuas_before) + " to " +
                   String(kiuas));

    // Handle Kiuas events
    if (kiuas) {
      bot.sendMessage(SAUNA_CHAT,
                      "Sauna lämpiää, lämpötila " + String(temperature) + "°C",
                      "Markdown");
    }

    if (!kiuas) {
      bot.sendMessage(SAUNA_CHAT,
                      "Sauna sammui, lämpötila " + String(temperature) + "°C",
                      "Markdown");
      ready_before = false;
    }

    kiuas_before = kiuas;
  }

  if (temperature_before != temperature) {
    Serial.println("Temperature changed from " + String(temperature_before) +
                   " to " + String(temperature));

    // Handle Temperature events
    if (temperature >= 70 and kiuas and !ready_before) {
      bot.sendMessage(SAUNA_CHAT,
                      "Sauna valmis, lämpötila " + String(temperature) + "°C",
                      "Markdown");
    }

    temperature_before = temperature;
  }
}

bool getKiuas() { return kiuas_before; }