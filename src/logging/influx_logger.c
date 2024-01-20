#include "influx_logger.h"
#include "influx_secrets.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

void influx_log_test() {
  LOG_INFO("CURL TEST");
  CURL *curl;
  CURLcode response;

  curl = curl_easy_init();
  if (!curl) {
    LOG_DEBUG("hi");
    exit(1);
  }

  const char* url = INFLUX_URL;
  curl_easy_setopt(curl, CURLOPT_URL, url);
  
  // TODO:  add test line protocol, make HTTP request

  LOG_INFO("CURL TEST OVER");
}