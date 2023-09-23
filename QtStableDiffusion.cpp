#include "QtStableDiffusion.h"



constexpr auto delay_show_precision = 0;
QString version = "1.0.0.0";//在version.ini里面修改

QtStableDiffusion::QtStableDiffusion(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //载入样式表
    QFile styleFile(":/qss/qss1.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&styleFile);
        this->setStyleSheet(stream.readAll());
        styleFile.close();
    }
    
    //载入版本信息
    QSettings settings(":/ini/version.ini", QSettings::IniFormat);
    version = settings.value("General/version").toString();
    this->setWindowTitle(QString("QtStableDiffusion - V%1").arg(version));


    main_1s = new(QTimer);
    connect(main_1s, SIGNAL(timeout()), this, SLOT(on_main_1s_timeout()));
    main_1s->start(500);


    ui.label_3->setStyleSheet("color:red;");


    //获取本地ip
    QString localhostName = QHostInfo::localHostName();
    QString tmpipv4;
    QHostInfo info = QHostInfo::fromName(localhostName);
    //返回值是该主机名的信息
    //获取主机名关联的IP地址
    QHostAddress address = info.addresses().first();
    for (QHostAddress address : info.addresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            tmpipv4 = address.toString();
        }
    }
    ui.lineEdit->setText(tmpipv4 + ":7860");



    on_pushButton_clicked();
}

QtStableDiffusion::~QtStableDiffusion()
{

}

//void QtStableDiffusion::curl_run(QString url, QString* res)
//{
//
//    //CurlThread* curlt = new CurlThread(url, res, &ping);
//    //curlt->start();
//
//}

void QtStableDiffusion::connect_false(QString errmsg)
{
    ui.label_4->setText("-- ms");
    ui.label_3->setText("未连接");
    ui.label_3->setStyleSheet("color:red;");
    QMessageBox::information(this, QString("信息"), QString("连接失败！\n\n错误信息：\n%1").arg(QString(errmsg.isEmpty() ? "Unkonow" : errmsg)));
    isConnect = false;
    progress_now = 0;
    progress_total = 0;
    ui.comboBox_2->clear();
}

void QtStableDiffusion::on_horizontalSlider_sliderMoved(int position)
{
    ui.label_11->setText(QString::number(position));
}

void QtStableDiffusion::on_horizontalSlider_2_sliderMoved(int position)
{
    ui.label_16->setText(QString::number(position / 2.0f));
}

void QtStableDiffusion::on_horizontalSlider_3_sliderMoved(int position)
{
    ui.label_26->setText(QString::fromStdString(formatDoubleToDecimalPlaces(position / 100.0, 2)));
}

void QtStableDiffusion::on_pushButton_2_clicked()
{
    if (!isConnect)
    {
        QMessageBox::information(this, "信息", "请先连接 Stable Diffusion 后再试！");
        return;
    }

    if (ui.spinBox->text() == "" || ui.spinBox_2->text() == "" || ui.lineEdit_2->text() == "" || ui.lineEdit_3->text() == "")
    {
        QMessageBox::information(this, "信息", "参数有误，请检查出图参数！");
        return;
    }

    QString t_url = ui.lineEdit->text() + "/sdapi/v1/txt2img";
    QString t_post = "{\"prompt\": \"%1\",\"steps\": %2,\"width\": %3,\"height\": %4,\"cfg_scale\": %5,\"negative_prompt\": \"%6\",\"sampler_name\": \"%7\",\"seed\": \"%8\",\"subseed\": \"%9\""
        ",\"restore_faces\": %10,\"tiling\": %11,\"override_settings\":{\"sd_model_checkpoint\": \"%12\"}}";
    t_post = t_post.arg(ui.plainTextEdit->toPlainText())
        .arg(ui.horizontalSlider->value())
        .arg(ui.spinBox->text())
        .arg(ui.spinBox_2->text())
        .arg(ui.horizontalSlider_2->value() / 2.0f)//5
        .arg(ui.plainTextEdit_2->toPlainText())
        .arg(ui.comboBox->currentText())
        .arg(ui.lineEdit_2->text())
        .arg(ui.lineEdit_3->text())
        .arg(ui.checkBox->isChecked() ? "true":"false")
        .arg(ui.checkBox_2->isChecked() ? "true" : "false")
        .arg(ui.comboBox_2->currentText())
        ;


    //post
    CurlThread_img* curlp = new CurlThread_img(t_url, t_post, &img);
    threadPool.start(curlp);

}

void QtStableDiffusion::on_pushButton_3_clicked()
{
    QClipboard* clip = QApplication::clipboard();
    clip->setPixmap(pixmap);

    QMessageBox::information(this, "信息", "操作完成！");
}

void QtStableDiffusion::on_pushButton_4_clicked()
{
    if (!isConnect)
    {
        QMessageBox::information(this, "信息", "请先连接 Stable Diffusion 后再试！");
        return;
    }

    std::string result_model;

    ui.comboBox_2->clear();

    HO_get(QString(ui.lineEdit->text() + "/sdapi/v1/sd-models").toStdString(), result_model);

    //json
    rapidjson::Document doc;
    doc.Parse(result_model.c_str());
    if (!doc.IsArray()) 
    {
        // JSON数据不是一个数组
        return;
    }
    // 遍历数组中的每个对象
    for (rapidjson::SizeType i = 0; i < doc.Size(); i++) 
    {
        const rapidjson::Value& item = doc[i];
        if (item.IsObject()) 
        {
            // 访问对象中的键值对
            const char* title = item["title"].GetString();

            ui.comboBox_2->addItem(QString::fromStdString(title));
        }
    }

}

void QtStableDiffusion::on_pushButton_5_clicked()
{
    QDir dir(QCoreApplication::applicationDirPath());
    if (!dir.exists("images_save"))
    {
        dir.mkdir("images_save");
    }
    QString filename = QCoreApplication::applicationDirPath() + "/images_save/" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh.mm.ss") + ".png";
    if (!pixmap.save(filename))
    {
        QMessageBox::information(this, "信息", QString::fromStdString("保存失败！\n\n错误信息：\naim_file_path: %1").arg(filename));
    }
    else
    {
        QMessageBox::information(this, "信息", "成功保存至：\"" + filename + "\"");
    }

}

void QtStableDiffusion::on_pushButton_6_clicked()
{
    if (!isConnect)
    {
        QMessageBox::information(this, "信息", "请先连接 Stable Diffusion 后再试！");
        return;
    }

    QString t_url = ui.lineEdit->text() + "/sdapi/v1/skip";
    QString t_post = "";
    t_post = t_post.arg(ui.plainTextEdit->toPlainText());

    static QString uesless_skip;

    //post
    CurlThread_img* curlp = new CurlThread_img(t_url, t_post, &uesless_skip);
    threadPool.start(curlp);

    //QMessageBox::information(this, "信息", "操作完成！");
}


void QtStableDiffusion::on_pushButton_7_clicked()
{
    //图生图
    if (!isConnect)
    {
        QMessageBox::information(this, "信息", "请先连接 Stable Diffusion 后再试！");
        return;
    }

    if (ui.spinBox->text() == "" || ui.spinBox_2->text() == "" || ui.lineEdit_2->text() == "" || ui.lineEdit_3->text() == "" || (ui.lineEdit_4->text() == "" && !isImgFromClipborad))
    {
        QMessageBox::information(this, "信息", "参数有误，请检查出图参数！");
        return;
    }

    QString t_url = ui.lineEdit->text() + "/sdapi/v1/img2img";
    QPixmap load_pic;

    if (isImgFromClipborad)
    {
        load_pic = pixmap_org;
    }
    else
    {
        load_pic.load(ui.lineEdit_4->text());
    }
    QByteArray ba;
    QBuffer buf(&ba);
    load_pic.save(&buf, "png");
    std::string base64 = ba.toBase64().toStdString();
    

    QString t_post = "{\"prompt\": \"%1\",\"steps\": %2,\"width\": %3,\"height\": %4,\"cfg_scale\": %5,\"negative_prompt\": \"%6\",\"sampler_name\": \"%7\",\"seed\": \"%8\",\"subseed\": \"%9\""
        ",\"restore_faces\": %10,\"tiling\": %11,\"override_settings\":{\"sd_model_checkpoint\": \"%12\"},\"init_images\": [\"%13\"],\"denoising_strength\": %14}";
    t_post = t_post.arg(ui.plainTextEdit->toPlainText())
        .arg(ui.horizontalSlider->value())
        .arg(ui.spinBox->text())
        .arg(ui.spinBox_2->text())
        .arg(ui.horizontalSlider_2->value() / 2.0f)//5
        .arg(ui.plainTextEdit_2->toPlainText())
        .arg(ui.comboBox->currentText())
        .arg(ui.lineEdit_2->text())
        .arg(ui.lineEdit_3->text())
        .arg(ui.checkBox->isChecked() ? "true" : "false")
        .arg(ui.checkBox_2->isChecked() ? "true" : "false")
        .arg(ui.comboBox_2->currentText())
        .arg(QString::fromStdString(base64))
        .arg( ui.horizontalSlider_3->value() / 100.0)
        ;

    //post
    CurlThread_img* curlp = new CurlThread_img(t_url, t_post, &img);
    threadPool.start(curlp);
}

void QtStableDiffusion::on_pushButton_8_clicked()
{
    QString fimename_t = QFileDialog::getOpenFileName(this, QStringLiteral("请选择图片文件："), QCoreApplication::applicationDirPath(), QStringLiteral("图片文件(*.png;*.jpg;*.bmp;*.jpeg;*.gif)\n所有文件(*.*)"));
    if (!fimename_t.isEmpty())
    {
        ui.lineEdit_4->setText(fimename_t);//bmp jpg jpeg png gif
        pixmap_org = QPixmap(ui.lineEdit_4->text());
        ui.label_27->setPixmap(pixmap_org.scaled(ui.label_27->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        isImgFromClipborad = false;
    }

}

void QtStableDiffusion::on_pushButton_9_clicked()
{
    
    QClipboard* clipboard = QGuiApplication::clipboard();
    QPixmap pixmap_temp ;
    pixmap_temp = clipboard->pixmap();
    if (pixmap_temp.isNull())
    {
        QMessageBox::information(this, "信息", "剪贴板未包含任何有效图片！");
        isImgFromClipborad = false;


    }
    else
    {
        pixmap_org = pixmap_temp;
        isImgFromClipborad = true;
    }
    ui.label_27->setPixmap(pixmap_org.scaled(ui.label_27->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void QtStableDiffusion::on_pushButton_10_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, QStringLiteral("请选择要保存到的位置，输入文件名"), QCoreApplication::applicationDirPath(), "配置文件(*.ini)");
    // 检查用户是否取消了保存操作
    if (filePath.isEmpty()) {
        // 用户取消了保存操作
        return;
    }


    // 创建QSettings对象并指定INI文件路径
    QSettings settings(filePath, QSettings::IniFormat);

    // 写入配置项
    settings.setValue("General/ModelName", ui.comboBox_2->currentText());
    settings.setValue("General/PositivePromt", ui.plainTextEdit->toPlainText());
    settings.setValue("General/NegativePromt", ui.plainTextEdit_2->toPlainText());

    settings.setValue("General/SampleMethod", ui.comboBox->currentText());
    settings.setValue("General/Steps", ui.horizontalSlider->value());
    settings.setValue("General/CFG", ui.horizontalSlider_2->value() / 2.0f);

    settings.setValue("General/Hight", ui.spinBox->value());
    settings.setValue("General/Width", ui.spinBox_2->value());

    settings.setValue("General/Seed", ui.lineEdit_2->text());
    settings.setValue("General/SubSeed", ui.lineEdit_3->text());

    settings.setValue("General/Face", ui.checkBox->isChecked());
    settings.setValue("General/Ground", ui.checkBox_2->isChecked());

    QMessageBox::information(this, "信息", QString("操作完成！（配置文件：%1）").arg(filePath));
}

void QtStableDiffusion::on_pushButton_11_clicked()
{
    QString fimename_t = QFileDialog::getOpenFileName(this, QStringLiteral("请选择配置文件："), QCoreApplication::applicationDirPath(), QStringLiteral("配置文件(*.ini)"));
    if (fimename_t.isEmpty())
    {
        return;
    }

    QSettings settings(fimename_t, QSettings::IniFormat);


    ui.comboBox_2->setCurrentText(settings.value("General/ModelName").toString());


    ui.plainTextEdit->setPlainText(settings.value("General/PositivePromt").toString());
    ui.plainTextEdit_2->setPlainText(settings.value("General/NegativePromt").toString());

    ui.comboBox->setCurrentText(settings.value("General/SampleMethod").toString());
    ui.horizontalSlider->setSliderPosition(settings.value("General/Steps").toInt());
    ui.horizontalSlider_2->setSliderPosition(settings.value("General/CFG").toFloat() * 2);
    on_horizontalSlider_sliderMoved(ui.horizontalSlider->sliderPosition());
    on_horizontalSlider_2_sliderMoved(ui.horizontalSlider_2->sliderPosition());

    ui.spinBox->setValue(settings.value("General/Hight").toInt());
    ui.spinBox_2->setValue(settings.value("General/Width").toInt());

    ui.lineEdit_2->setText(settings.value("General/Seed").toString());
    ui.lineEdit_3->setText(settings.value("General/SubSeed").toString());

    ui.checkBox->setChecked(settings.value("General/Face").toBool());
    ui.checkBox_2->setChecked(settings.value("General/Ground").toBool());

}

void QtStableDiffusion::on_main_1s_timeout()
{
    if (isConnect)
    {
        Timer time_1;
        time_1.starttimer();
        static QString res_;
        QString url_t = ui.lineEdit->text() + "/sdapi/v1/progress?skip_current_image=true";

        CurlThread* threadp = new CurlThread(url_t, &res_, &ping);
        threadPool.start(threadp);
        //curl_run(ui.lineEdit->text() + "/sdapi/v1/progress?skip_current_image=true", &res_);
        static int i = 0;
        if (res_.isEmpty() && i > 1)
        {
            connect_false("Connect_timeout");
            i = 0;
        }
        else
        {
            if (res_.isEmpty())
            {
                i++;
            }
            else
            {
                if (res_.contains("progress") && res_.contains("eta_relative") && res_.contains("state"))
                {
                    ui.label_4->setText(QString::fromStdString(formatDoubleToDecimalPlaces(ping, delay_show_precision)) + " ms");
                    double res_eta;
                    JO_readjson(res_.toStdString().c_str(), "progress", res_eta);
                    ui.progressBar->setValue(res_eta * 100);

                    JO_readjson(res_.toStdString().c_str(), "eta_relative", res_eta);
                    ui.label_7->setText(QString::fromStdString(formatDoubleToDecimalPlaces(res_eta, 2)) + " 秒");

                    std::string state;
                    JO_readjson_object(res_.toStdString().c_str(), "state", state);
                    JO_readjson(state.c_str(), "sampling_step", progress_now);
                    JO_readjson(state.c_str(), "sampling_steps", progress_total);
                }
                else
                {
                    if (test > 0)
                    {
                        test--;
                    }
                    else
                    {
                        isConnect = false;
                        progress_now = 0;
                        progress_total = 0;
                        ui.comboBox_2->clear();

                        ui.label_4->setText(QString::fromStdString(formatDoubleToDecimalPlaces(ping, delay_show_precision)) + " ms");
                        ui.label_3->setText("连接异常");
                        ui.label_3->setStyleSheet("color:red;");
                        QTextDocument textdoc;
                        textdoc.setHtml(res_);
                        QMessageBox::information(this, QString("信息"), QString("服务器返回了异常数据，请稍后再试！\n\n错误信息：\n%1").arg(textdoc.toPlainText()));

                    }
                    
                }

            }
            
        }

    }

    //显示图片

    std::vector<std::string> img_arr;
    JO_readjson_array(img.toStdString().c_str(), "images", img_arr);
    //{
    //    "images": [
    //        ""
    //    ] ,
    //        "parameters" : {
    //        "enable_hr": false,
    //            "denoising_strength" : 0,
    //            "firstphase_width" : 0,
    //            "firstphase_height" : 0,
    //            "hr_scale" : 2,
    //            "hr_upscaler" : null,
    //            "hr_second_pass_steps" : 0,
    //            "hr_resize_x" : 0,
    //            "hr_resize_y" : 0,
    //            "hr_sampler_name" : null,
    //            "hr_prompt" : "",
    //            "hr_negative_prompt" : "",
    //            "prompt" : "",
    //            "styles" : null,
    //            "seed" : -1,
    //            "subseed" : -1,
    //            "subseed_strength" : 0,
    //            "seed_resize_from_h" : -1,
    //            "seed_resize_from_w" : -1,
    //            "sampler_name" : null,
    //            "batch_size" : 1,
    //            "n_iter" : 1,
    //            "steps" : 50,
    //            "cfg_scale" : 7,
    //            "width" : 512,
    //            "height" : 512,
    //            "restore_faces" : false,
    //            "tiling" : false,
    //            "do_not_save_samples" : false,
    //            "do_not_save_grid" : false,
    //            "negative_prompt" : null,
    //            "eta" : null,
    //            "s_min_uncond" : 0,
    //            "s_churn" : 0,
    //            "s_tmax" : null,
    //            "s_tmin" : 0,
    //            "s_noise" : 1,
    //            "override_settings" : null,
    //            "override_settings_restore_afterwards" : true,
    //            "script_args" : [] ,
    //            "sampler_index" : "Euler",
    //            "script_name" : null,
    //            "send_images" : true,
    //            "save_images" : false,
    //            "alwayson_scripts" : {}
    //    },
    //        "info": "{\"prompt\": \"\", \"all_prompts\": [\"\"], \"negative_prompt\": \"\", \"all_negative_prompts\": [\"\"], \"seed\": 3821593032, \"all_seeds\": [3821593032], \"subseed\": 1625302862, \"all_subseeds\": [1625302862], \"subseed_strength\": 0, \"width\": 512, \"height\": 512, \"sampler_name\": \"Euler\", \"cfg_scale\": 7.0, \"steps\": 50, \"batch_size\": 1, \"restore_faces\": false, \"face_restoration_model\": null, \"sd_model_hash\": \"e3edb8a26f\", \"seed_resize_from_w\": -1, \"seed_resize_from_h\": -1, \"denoising_strength\": 0, \"extra_generation_params\": {}, \"index_of_first_image\": 0, \"infotexts\": [\"Steps: 50, Sampler: Euler, CFG scale: 7.0, Seed: 3821593032, Size: 512x512, Model hash: e3edb8a26f, Model: ghostmix_v20Bakedvae, Denoising strength: 0, Clip skip: 2, Version: v1.5.1\"], \"styles\": [], \"job_timestamp\": \"20230904165315\", \"clip_skip\": 2, \"is_using_inpainting_conditioning\": false}"
    //}
    std::string parameters;
    JO_readjson(img.toStdString().c_str(), "info", parameters);
    int seed = 0, subseed = 0;
    JO_readjson(parameters.c_str(), "seed", seed);
    JO_readjson(parameters.c_str(), "subseed", subseed);
    ui.label_21->setText("<html><head/><body><p>种子/子种子：<span style=\" font-weight:700; color:#ff0000;\">" + QString::number(seed) + "</span>/<span style=\" font-weight:700; color:#ff0000;\">" + QString::number(subseed) + "</span></p></body></html>");


    if (img_arr.empty())
    {
    }
    else
    {
        pixmap.loadFromData(QByteArray::fromBase64(QString::fromStdString(img_arr[0]).toLocal8Bit()));

        ui.label_8->setPixmap(pixmap.scaled(ui.label_8->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 在QLabel显示图片
    }

    if (pixmap_org.isNull())
    {
    }
    else
    {
        ui.label_27->setPixmap(pixmap_org.scaled(ui.label_27->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 在QLabel显示图片
    }
    

    if (ui.progressBar->value() == 0 && ui.label_7->text() == "0.00 秒")
    {
        ui.label_18->setText("空闲中");
        ui.label_18->setStyleSheet("color:green;");
    }
    else
    {
        if (progress_now != 0)
        {
            ui.label_18->setText(QString::fromStdString("忙碌中  ( %1 / %2 )").arg(progress_now).arg(progress_total));
        }
        else
        {
            ui.label_18->setText("忙碌中");
        }
        ui.label_18->setStyleSheet("color:red;");
    }

    if (!isConnect)
    {
        ui.label_18->setText("请先连接 Stable Diffusion API");
        ui.label_18->setStyleSheet("color:blue;");
    }

}

void QtStableDiffusion::on_pushButton_clicked()
{
    std::string res;
    
    int res_num;
    if ((res_num = HO_get(ui.lineEdit->text().toStdString() + "/user", res) != 0)|| res != "null")
    {
        if (res_num == 0)
        {
            isConnect = false;
            progress_now = 0;
            progress_total = 0;
            ui.comboBox_2->clear();

            ui.label_4->setText(QString::fromStdString(formatDoubleToDecimalPlaces(ping, delay_show_precision)) + " ms");
            ui.label_3->setText("连接异常");
            ui.label_3->setStyleSheet("color:red;");
            QTextDocument textdoc;
            textdoc.setHtml(QString::fromStdString(res));
            QMessageBox::information(this, QString("信息"), QString("服务器返回了异常数据，请稍后再试！\n\n错误信息：\n%1").arg(textdoc.toPlainText()));
        }
        else
        {
            connect_false("Connect_failed");
        }
        
    }
    else
    {
        ui.label_4->setText(QString::fromStdString(formatDoubleToDecimalPlaces(ping, delay_show_precision)) + " ms");
        ui.label_3->setText("已连接");
        ui.label_3->setStyleSheet("color:green;");
        isConnect = true;

        test = 2;

        on_pushButton_4_clicked();
        
    }
}
