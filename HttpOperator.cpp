#include "HttpOperator.h"
#include <qdebug.h>


//非线程，会卡

// 回调函数，用于处理Curl响应
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) 
{
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

int HO_get(const std::string &address, std::string& result)
{
    CURL* curl;
    CURLcode res;

    // 初始化Curl
    curl = curl_easy_init();
    if (curl) {
        // 设置请求的URL
        curl_easy_setopt(curl, CURLOPT_URL, address.c_str());//一定要c_str
        curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP); // 设置使用HTTP协议

        // 设置请求方法为GET
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

        //不接收响应头数据0代表不接收 1代表接收
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);

        // 设置请求头
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "accept: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        //设置等待时间
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);

        // 设置响应数据的处理函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        // 执行GET请求
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            //qDebug() << "Curl请求失败: " << curl_easy_strerror(res) << "地址：" << address;
            return -1;
        }
        else 
        {
            // 请求成功，输出响应数据
            //std::cout << "响应数据:\n" << result << std::endl;
            //qDebug() << "响应数据:" << result;
        }

        // 清理Curl资源
        curl_easy_cleanup(curl);
    }
    else 
    {
        //std::cerr << "Curl初始化失败" << std::endl;
        return -2;
    }

    return 0;
}
