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

#include "app.h"

/** Bluetooth broadcast name */
char g_ble_dev_name[10] = "R4K-Oil";
/** LoRa packet data */
lora_data_s oil_lora_data;
/** SHTC3 Air temp/humidity */
SHTC3 shtc3;
/** Union to split 16 bit to 2x8bit */
u_16 u_16lora;

/**
 * @brief Set the up app object
 * Init BLE even if we don't want to use it
 * for proper sleep
 * 
 */
void setup_app(void) 
{
  g_enable_ble = true;
  api_set_version(1, 0 ,2);
}

/**
 * @brief Initialize app
 * Warm up the sensor and put it to sleep
 * Stop API behaviour, we want full control
 * 
 * @return true 
 * @return false 
 */
bool init_app(void)
{
  API_LOG("APP", "R4K-Oil started");
  SHTC3_Status_TypeDef shtc3_status;
  bool init_result;

  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  pinMode(ECHO_PIN, INPUT);
  
  Wire.begin();
  shtc3_status = shtc3.begin();
  if(shtc3_status == SHTC3_Status_Nominal)
  {
    Wire.setClock(400000);

    shtc3.setMode(SHTC3_CMD_CSD_TF_NPM);
    shtc3.sleep();

    api_timer_stop();
    g_lorawan_settings.send_repeat_time = WARM_TIME;
    api_timer_restart(WARM_TIME);

    init_result = true;
  } else {
    init_result = false;
  }

  return init_result;
}

/**
 * @brief WisBlock API event handler
 * Wake up every REST_TIME and check batt/temp/ultrasonics
 * 
 */
void app_event_handler(void)
{
  digitalWrite(LED_GREEN, LOW);
  if ((g_task_event_type & STATUS) == STATUS)
	{
	  g_task_event_type &= N_STATUS;

    API_LOG("APP", "Woke up");
    digitalWrite(WB_IO2, HIGH);

    read_batt_lora();
    read_shtc3();
    read_ultra();

    send_lora_data((uint8_t*)&oil_lora_data, sizeof(lora_data_s));

    if(g_join_result)
    {
      g_lorawan_settings.send_repeat_time = REST_TIME;
      api_timer_restart(REST_TIME);
    } else {
      g_lorawan_settings.send_repeat_time = WARM_TIME;
      api_timer_restart(WARM_TIME);
    }

    digitalWrite(WB_IO2, LOW);
  }
}

/**
 * @brief Send LoRa Packet
 * 
 * @param lora_data data to send
 */
void send_lora_data(uint8_t* lora_data, uint16_t size)
{
	lmh_error_status result = send_lora_packet(lora_data, size);
	switch (result)
	{
		case LMH_SUCCESS:
		API_LOG("APP", "LoRa packet sent");
		break;

		case LMH_BUSY:
		API_LOG("APP", "LoRa radio busy");
		break;

		case LMH_ERROR:
		API_LOG("APP", "LoRa radio error");
		break;
	}
}

/**
 * @brief check temp (and hunidity for fun)
 * If checksum passes, sensor readings are vaild
 * 
 */
void read_shtc3(void)
{
  API_LOG("SHTC3", "Reading");
  shtc3.wake();
  shtc3.update();
  shtc3.sleep();

  if(shtc3.passIDcrc) 
  {
    /** -2 Adjustment for MCU/Board heat */
    uint8_t temp = static_cast<uint8_t>(shtc3.toDegC()-2);
    uint8_t humi = static_cast<uint8_t>(shtc3.toPercent()-2);
    oil_lora_data.temp = temp;
    oil_lora_data.humi = humi;
    std::string disp = std::to_string(temp) + "C " + std::to_string(humi) + "%";
    API_LOG("SHTC3", disp);
  } else {
    API_LOG("SHTC3", "Failed checksum");
  }
}

/**
 * @brief Get reading from HC-SR04
 * 
 */
void read_ultra(void)
{
  API_LOG("HC-SR04", "Reading");
  uint32_t time;
  uint32_t mm;
  uint32_t cm;
  std::string level_disp;
  /** velocity of sound = 331.6+0.6*25℃(m/s) */
  float sound_vel = 331.6 + 0.6 * oil_lora_data.temp;
  /** max measure distance is 4m,the velocity of sound is 331.6m/s in 0℃,TIME_OUT=4*2/331.6*1000000=24125us */
  uint16_t time_out = 4 * 2 / sound_vel * 1000000;
  float ratio = sound_vel / 1000 / 2;

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  time = pulseIn(ECHO_PIN, HIGH); 
  time = time * 0.7726;

  if(time < time_out)
  {
    mm = time * ratio;
    cm = mm / 10;
  } else {
    cm = 0;
  }

  oil_lora_data.level = cm;
  level_disp = std::to_string(cm) + "cm";
  API_LOG("HC-SR04", level_disp.c_str());
}

/**
 * @brief Read battery to send over LoRa
 * 
 */
void read_batt_lora(void)
{
  float batt_level_mv = read_batt();
  uint16_t batt_level_lora = static_cast<uint16_t>(batt_level_mv);
  u_16lora.data = batt_level_lora;
  oil_lora_data.batt_1 = u_16lora.piece[0];
  oil_lora_data.batt_2 = u_16lora.piece[1];
  API_LOG("BATT", batt_level_mv);
}

/**
 * @brief API Event handler to deal with LoRa data
 * 
 */
void lora_data_handler(void)
{
  if ((g_task_event_type & LORA_JOIN_FIN) == LORA_JOIN_FIN)
  {
    g_task_event_type &= N_LORA_JOIN_FIN;
    if (g_join_result)
    {
      digitalWrite(LED_GREEN, LOW);
    }
  }

  /** Sleep radio after each transmission has finished */
  if ((g_task_event_type & LORA_TX_FIN) == LORA_TX_FIN)
	{
		g_task_event_type &= N_LORA_TX_FIN;
    Radio.Sleep();
  }
}

/**
 * @brief API Event handlder to deal with BLE data
 * 
 */
void ble_data_handler(void)
{
	if (g_enable_ble)
	{
		if ((g_task_event_type & BLE_DATA) == BLE_DATA)
		{
			g_task_event_type &= N_BLE_DATA;

			while (g_ble_uart.available() > 0)
			{
				at_serial_input(uint8_t(g_ble_uart.read()));
				delay(5);
			}
			at_serial_input(uint8_t('\n'));
		}
	}
}
