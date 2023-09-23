#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtStableDiffusion.h"

#include <curl\curl.h>
#include "JsonOperator.h"
#include "OtherFunction.h"
#include "HttpOperator.h"

#include "Timer.h"

#include <QSplashScreen>
#include <qthread.h>
#include <QMessageBox>
#include <qtimer.h>
#include <qclipboard.h>
#include <qdatetime.h>
#include <QFileDialog>
#include <QBuffer>
#include <QDragEnterEvent>
#include <QUrl>
#include <QMimeData>
#include <QDropEvent>
#include <QDrag>
#include <QRect>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QtNetwork/qhostinfo.h>
#include <qthreadpool.h>
#include <qrunnable.h>
#include <qsettings.h>


#include <QDir>
#include <QMutex>

extern QMutex mutex, mutex2;

class CurlThread : public QRunnable {
    //Q_OBJECT

//signals:
//    void requestFinished(const QString& response);

public:
    //explicit CurlThread(QObject* parent = nullptr)
    //    : QThread(parent) 
    //{
    //}

    CurlThread(QString& param, QString* param_res, double* param_ping) : url(param) , res_data(param_res), ping(param_ping) {};

    void run() override 
    {
        CURL* curl;
        CURLcode res;

        Timer time_1;


        // 初始化Curl
        curl = curl_easy_init();
        if (curl) {
            // 设置请求的URL
            curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());

            // 设置响应数据的处理函数
            QString response_data;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);


            time_1.starttimer();
            // 执行GET请求

            QMutex mutex;
            mutex.lock();

            res = curl_easy_perform(curl);

            mutex.unlock();

            *ping = time_1.endtimer() / 1000.0;//获取ping

            if (res == CURLE_OK) 
            {
                // 请求成功，发送响应数据到主线程
                *res_data = response_data;
                //qDebug() << response_data;
                //emit requestFinished(response_data);
            }
            else 
            {
                *res_data = "";
                //emit requestFinished("");
            }

            // 清理Curl资源
            curl_easy_cleanup(curl);
        }
        else 
        {
            *res_data = "";
            //emit requestFinished("");
        }

    }

private:

    QString url;
    QString* res_data;
    double* ping;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, QString* output) 
    {
        size_t total_size = size * nmemb;
        output->append(QString::fromStdString(static_cast<char*>(contents)));
        return total_size;
    }
};

class CurlThread_img : public QRunnable {
    //Q_OBJECT

        //signals:
        //    void requestFinished(const QString& response);

public:
    //explicit CurlThread_img(QObject* parent = nullptr)
    //    : QThread(parent)
    //{
    //}

    CurlThread_img(QString& param, QString& param_2, QString* param_res) : url(param), post(param_2), res_data(param_res){};

    void run() override
    {
        CURL* curl;
        CURLcode res;

        // 初始化Curl
        curl = curl_easy_init();
        if (curl) {
            // 设置请求的URL
            curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());

            // 设置POST请求
            curl_easy_setopt(curl, CURLOPT_POST, 1);

            // 设置POST数据
            std::string post_data = post.toStdString();
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());

            // 设置HTTP请求头
            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, "accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // 设置响应数据的处理函数
            QString response_data;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);


            // 执行POST请求
            QMutex mutex2;
            mutex2.lock();


            res = curl_easy_perform(curl);


            mutex2.unlock();

            curl_slist_free_all(headers);
            if (res == CURLE_OK)
            {
                // 请求成功，发送响应数据到主线程
                *res_data = response_data;
                //qDebug() << response_data << url << post;
                //emit requestFinished(response_data);
            }
            else
            {
                *res_data = "";
                //emit requestFinished("");
            }

            // 清理Curl资源
            curl_easy_cleanup(curl);
        }
        else
        {
            *res_data = "";
            //emit requestFinished("");
        }

    }

private:

    QString url;
    QString post;
    QString* res_data;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, QString* output)
    {
        size_t total_size = size * nmemb;
        output->append(QString::fromStdString(static_cast<char*>(contents)));
        return total_size;
    }
};


class QtStableDiffusion : public QMainWindow
{
    Q_OBJECT

public:
    QtStableDiffusion(QWidget *parent = nullptr);
    ~QtStableDiffusion();

    //const char* aimurl;
    //QString aimurl_res;
    QString img;
    QPixmap pixmap, pixmap_org;
    QTimer* main_1s;

    QThreadPool threadPool;//线程池

    double ping = 0.0f;
    bool isImgFromClipborad = false;

    bool isConnect = false;

    int progress_now = 0;
    int progress_total = 0;

    void connect_false(QString errmsg);

    int test = 0;//断开连接延迟

    //函数重写

    /* dragEnterEvent 过滤无用Event */
    void dragEnterEvent(QDragEnterEvent* event) override
    {
        if (event->mimeData()->hasUrls())
            event->acceptProposedAction();
    }
    /* dropEvent 过滤无用urls，并将有效部分交给文件读取方式加载 */
    void dropEvent(QDropEvent* event) override
    {
        QList<QUrl> urls = event->mimeData()->urls();
        QStringList files;
        QString suffixs = "bmp jpg jpeg png gif";

        for (QUrl url : urls) {
            QFileInfo file(url.toLocalFile());	//toLocalFile可以获取文件路径，而非QUrl的file://开头的路径

            if (file.isFile() && suffixs.contains(file.suffix())) //过滤掉目录和不支持的后缀，如果要支持目录，则要自己遍历每一个目录。
                files.append(file.filePath());
        }

        if (!files.isEmpty())
        {
            isImgFromClipborad = false;
            ui.lineEdit_4->setText(files[0]);
            pixmap_org = QPixmap(ui.lineEdit_4->text());
            ui.label_27->setPixmap(pixmap_org.scaled(ui.label_27->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    void resizeEvent(QResizeEvent* event) 
    {
        ui.label_8->setPixmap(pixmap.scaled(ui.label_8->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 在QLabel显示图片
        ui.label_27->setPixmap(pixmap_org.scaled(ui.label_27->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 在QLabel显示图片
    }

    //https ://blog.csdn.net/GarfieldGCat/article/details/112504575

public slots:
    void on_pushButton_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_2_sliderMoved(int position);

    void on_horizontalSlider_3_sliderMoved(int position);

    void on_pushButton_2_clicked();//文生图

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();//保存图片

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();//图生图

    void on_pushButton_8_clicked();//选照片

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();//保存参数

    void on_pushButton_11_clicked();//载入参数

    void on_main_1s_timeout();


private:
    Ui::QtStableDiffusionClass ui;
};

