#include "JsonOperator.h"

//#include <qdebug.h>

int JO_readjson(const char* json, const char* key, std::string& value)
{
    rapidjson::Document doc;
    doc.Parse(json);

    if (!doc.HasParseError()) 
    {
        value = doc[key].GetString();
        return 0;
    }

    return -1;
}

int JO_readjson(const char* json, const char* key, int& value)
{
    rapidjson::Document doc;
    doc.Parse(json);

    if (!doc.HasParseError())
    {
        value = doc[key].GetInt();
        return 0;
    }

    return -1;
}

int JO_readjson(const char* json, const char* key, long long& value)
{
    rapidjson::Document doc;
    doc.Parse(json);

    if (!doc.HasParseError())
    {
        value = doc[key].GetInt64();
        return 0;
    }

    return -1;
}

int JO_readjson(const char* json, const char* key, double& value)
{
    rapidjson::Document doc;
    doc.Parse(json);

    if (!doc.HasParseError())
    {
        value = doc[key].GetDouble();
        return 0;
    }

    return -1;
}

int JO_readjson(const char* json, const char* key, float& value)
{
    rapidjson::Document doc;
    doc.Parse(json);

    if (!doc.HasParseError())
    {
        value = doc[key].GetFloat();
        return 0;
    }

    return -1;
}

int JO_readjson(const char* json, const char* key, bool& value)
{
    rapidjson::Document doc;
    doc.Parse(json);

    if (!doc.HasParseError())
    {
        value = doc[key].GetBool();
        return 0;
    }

    return -1;
}

int JO_readjson_array(const char* json, const char* key, std::vector<std::string>& str_arr)
{
    str_arr.clear();
    rapidjson::Document doc;
    doc.Parse(json);

    if (key == nullptr)
    {
        if (!doc.IsArray()) 
        {
            // JSON数据不是一个数组
            return -1;
        }
        else
        {
            
            for (rapidjson::SizeType i = 0; i < doc.Size(); i++)
            {
                rapidjson::Value& item = doc[i];

                str_arr.push_back(item.GetString());
            }
            return 0;
        }
    }
    else
    {
        if (!doc.HasParseError())
        {
            const rapidjson::Value& imageArray = doc[key];
            for (rapidjson::SizeType i = 0; i < imageArray.Size(); i++)
            {
                if (imageArray[i].IsString())
                {
                    //qDebug() << imageArray[i].GetString();
                    str_arr.push_back(imageArray[i].GetString());
                }
            }
            return 0;
        }
    }


    return -1;
}

int JO_readjson_object(const char* json, const char* key, std::string& obj_con)
{
    rapidjson::Document doc;
    doc.Parse(json);

    if (!doc.HasParseError())
    {
        const rapidjson::Value& imageArray = doc[key];
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        imageArray.Accept(writer);
        obj_con = buffer.GetString();
        return 0;
    }

    return -1;
}
