#include "influx_logger.h"
#include "influx_secrets.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>
#include <time.h>

void influx_log(char* data_format) {
  CURL *curl;
  CURLcode response;

  curl = curl_easy_init();
  if (!curl) {
    LOG_DEBUG("hi");
    exit(1);
  }
  
  char data[256];
  
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  int64_t timestamp_ns = ((int64_t)ts.tv_sec * 1000000000) + ts.tv_nsec;
  snprintf(data, sizeof(data), data_format, timestamp_ns);
  const char* url = INFLUX_URL;

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, INFLUX_TOKEN); 
  
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  response = curl_easy_perform(curl);
  if (response != CURLE_OK) {
    LOG_ERROR("test HTTP request failed");
  } else {
    LOG_INFO("test http request succeeded!");
    uint16_t http_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    LOG_INFO("http response status %d", http_code);
  }

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  curl_slist_free_all(headers);
}

void influx_log_raw(uint8_t unit, uint16_t register_num, uint16_t value) {
  char* data_format = "raw,unit=%d reg%d=%di \%lld";
  char data[512];
  snprintf(data, sizeof(data), data_format, unit, register_num, value);
  DEBUG_INFO("influx_log_raw: %s", data);
  influx_log(data);
}

void influx_log_test() {
  LOG_INFO("CURL TEST");
  char* test_data_format = "test,unit=127 reg12345=32767i,reg12346=32766i %lld";

  influx_log(test_data_format);

  LOG_INFO("CURL TEST OVER");
}