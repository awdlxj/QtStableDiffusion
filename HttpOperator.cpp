#include "HttpOperator.h"
#include <qdebug.h>


//���̣߳��Ῠ

// �ص����������ڴ���Curl��Ӧ
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

    // ��ʼ��Curl
    curl = curl_easy_init();
    if (curl) {
        // ���������URL
        curl_easy_setopt(curl, CURLOPT_URL, address.c_str());//һ��Ҫc_str
        curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP); // ����ʹ��HTTPЭ��

        // �������󷽷�ΪGET
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

        //��������Ӧͷ����0�������� 1�������
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);

        // ��������ͷ
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "accept: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        //���õȴ�ʱ��
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);

        // ������Ӧ���ݵĴ�����
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        // ִ��GET����
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            //qDebug() << "Curl����ʧ��: " << curl_easy_strerror(res) << "��ַ��" << address;
            return -1;
        }
        else 
        {
            // ����ɹ��������Ӧ����
            //std::cout << "��Ӧ����:\n" << result << std::endl;
            //qDebug() << "��Ӧ����:" << result;
        }

        // ����Curl��Դ
        curl_easy_cleanup(curl);
    }
    else 
    {
        //std::cerr << "Curl��ʼ��ʧ��" << std::endl;
        return -2;
    }

    return 0;
}
