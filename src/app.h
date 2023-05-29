/**
 * @file app.h
 * @author Jamie Howse (r4wknet@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>
#include <WisBlock-API.h>
#include <SparkFun_SHTC3.h>
#include <Wire.h>

/** 21600000 ms = 6 hrs */
#define REST_TIME 120000
/** 1800000 ms = 30 mins */
#define WARM_TIME 30000

/** RAK19003 UART TXD/RXD
 * TXD = Trigger, RXD = Echo
 * If you want to use I2C pins, use the following
 * SCL = Trigger, SDA = Echo
 * ECHO_PIN = 13
 * TRIG_PIN = 14
 * 
 */
static const uint8_t ECHO_PIN = 15;
static const uint8_t TRIG_PIN = 16;


/** Forward declaration */
void setup_app(void);
bool init_app(void);
void app_event_handler(void);
void send_lora_data(uint8_t* lora_data, uint16_t size);
void lora_data_handler(void);
void ble_data_handler(void);
void read_ultra(void);
void read_shtc3(void);
void read_batt_lora(void);

/** LoRa struct */
struct lora_data_s
{
    uint8_t temp = 0;
    uint8_t humi = 0;
    uint8_t level = 0;
    uint8_t batt_1 = 0;
    uint8_t batt_2 = 0;
};

/** Union to split a uint16_t into two uint_8t's */
union u_16
{
  uint16_t data = 0;
  uint8_t piece[2];
};
