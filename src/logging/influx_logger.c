#include "influx_logger.h"
#include "influx_secrets.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <time.h>

void influx_log(char* data) {
  CURL *curl;
  CURLcode response;

  curl = curl_easy_init();
  if (!curl) {
    LOG_DEBUG("hi");
    exit(1);
  }
  
  char data_plus_ts[512];
  
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  int64_t timestamp_ns = ((int64_t)ts.tv_sec * 1000000000) + ts.tv_nsec;
  snprintf(data_plus_ts, sizeof(data_plus_ts), "%s %lld", data, timestamp_ns);
  const char* url = INFLUX_URL;
  LOG_INFO("influx data plus timestamp: %s", data_plus_ts);
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, INFLUX_TOKEN); 
  
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_plus_ts);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  response = curl_easy_perform(curl);
  if (response != CURLE_OK) {
    LOG_ERROR("influx HTTP request failed (%d)", response);
  } else {
    uint16_t http_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    LOG_INFO("http request succeeded!  influx http response status %d", http_code);
  }

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  curl_slist_free_all(headers);
}

void influx_log_raw(uint8_t unit, uint16_t register_num, uint16_t value) {
  char* data_format = "raw,unit=%d reg%d=%di";
  char data[512];
  snprintf(data, sizeof(data), data_format, unit, register_num, value);
  LOG_INFO("influx_log_raw: %s", data);
  influx_log(data);
}

void influx_log_response(struct Modbus_Response* response) {
  LOG_INFO("==== start influx_log_response");
  LOG_INFO("unit number: %d, base register: %d", response->data->unit, response->context->base_register);
  char* data_prefix = "modbus,unit=%d ";

  // we know the max length of register number and uint16_t (both max 65535, so 5 digits),
  // we can/do allocate the string buffer entirely ahead of time.
  // fmt regXXXXX=XXXXXi, is 16 bytes
  // kmo 22 jan 2024 14h26
  

  unsigned int register_count = response->data->byte_count / 2;    // 16 bits = 1 word = 2 bytes
  LOG_INFO("influx_log_response:  register_count = %d", register_count);
  
  char register_values_string[16 * register_count];
  // char* register_values_string_cursor = register_values_string;
  
  unsigned int cursor = 0;
  while (cursor < register_count) {
    uint16_t register_number = response->context->base_register + cursor;
    LOG_INFO("influx_log_response: register number %d", register_number);

    uint16_t register_value = response->data->register_data[cursor];
    LOG_INFO("influx_log_response: register value %d", register_value);
    char this_pair[16];
    
    snprintf(this_pair, 15, "reg%d=%di", register_number, register_value);

    // if not last register/value pair, add a comma
    if (cursor < (register_count - 1)) {
      strcat(this_pair, ",");
    }

    strcat(register_values_string, this_pair);

    cursor++;

    LOG_DEBUG("current register/values string: %s", register_values_string);
  }

  LOG_INFO("==== end influx_log_response");
}