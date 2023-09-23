#pragma once

#include <string>
#include <curl/curl.h>

// 回调函数，用于处理Curl响应
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

//成功返回0
int HO_get(const std::string& address, std::string& result);


