#pragma once
#include <vector>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

int JO_readjson(const char* json, const char* key, std::string& value);

int JO_readjson(const char* json, const char* key, int& value);

int JO_readjson(const char* json, const char* key, long long& value);

int JO_readjson(const char* json, const char* key, double& value);

int JO_readjson(const char* json, const char* key, float& value);

int JO_readjson(const char* json, const char* key, bool& value);

int JO_readjson_array(const char* json, const char* key, std::vector<std::string>& str_arr);

int JO_readjson_object(const char* json, const char* key, std::string& obj_con);

