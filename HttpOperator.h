#pragma once

#include <string>
#include <curl/curl.h>

// �ص����������ڴ���Curl��Ӧ
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

//�ɹ�����0
int HO_get(const std::string& address, std::string& result);


