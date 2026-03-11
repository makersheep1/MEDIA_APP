#include "weatherwindow.h"
#include "weathertool.h"

#include <QDateTime>
#include <QEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QPainter>
#include <QPixmap>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>

WeatherWindow::WeatherWindow(QWidget *parent)
    : QWidget(parent)
    , mNetAccessManager(nullptr)
{
    setupUI();
    lblHighCurve->installEventFilter(this);
    lblLowCurve->installEventFilter(this);
    setFixedSize(800, 480);

    mNetAccessManager = new QNetworkAccessManager(this);
    connect(mNetAccessManager, &QNetworkAccessManager::finished, this, &WeatherWindow::onReplied);

    mTypeMap.insert("暴雪", ":/weather/pic/weather/type/BaoXue.png");
    mTypeMap.insert("暴雨", ":/weather/pic/weather/type/BaoYu.png");
    mTypeMap.insert("暴雨到大暴雨", ":/weather/pic/weather/type/BaoYuDaoDaBaoYu.png");
    mTypeMap.insert("大暴雨", ":/weather/pic/weather/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨到特大暴雨", ":/weather/pic/weather/type/DaBaoYuDaoTeDaBaoYu.png");
    mTypeMap.insert("大到暴雪", ":/weather/pic/weather/type/DaDaoBaoXue.png");
    mTypeMap.insert("大雪", ":/weather/pic/weather/type/DaXue.png");
    mTypeMap.insert("大雨", ":/weather/pic/weather/type/DaYu.png");
    mTypeMap.insert("冻雨", ":/weather/pic/weather/type/DongYu.png");
    mTypeMap.insert("多云", ":/weather/pic/weather/type/DuoYun.png");
    mTypeMap.insert("浮沉", ":/weather/pic/weather/type/FuChen.png");
    mTypeMap.insert("雷阵雨", ":/weather/pic/weather/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹", ":/weather/pic/weather/type/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("霾", ":/weather/pic/weather/type/Mai.png");
    mTypeMap.insert("强沙尘暴", ":/weather/pic/weather/type/QiangShaChenBao.png");
    mTypeMap.insert("晴", ":/weather/pic/weather/type/Qing.png");
    mTypeMap.insert("沙尘暴", ":/weather/pic/weather/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨", ":/weather/pic/weather/type/TeDaBaoYu.png");
    mTypeMap.insert("undefined", ":/weather/pic/weather/type/undefined.png");
    mTypeMap.insert("雾", ":/weather/pic/weather/type/Wu.png");
    mTypeMap.insert("小到中雪", ":/weather/pic/weather/type/XiaoDaoZhongXue.png");
    mTypeMap.insert("小到中雨", ":/weather/pic/weather/type/XiaoDaoZhongYu.png");
    mTypeMap.insert("小雪", ":/weather/pic/weather/type/XiaoXue.png");
    mTypeMap.insert("小雨", ":/weather/pic/weather/type/XiaoYu.png");
    mTypeMap.insert("雪", ":/weather/pic/weather/type/Xue.png");
    mTypeMap.insert("扬沙", ":/weather/pic/weather/type/YangSha.png");
    mTypeMap.insert("阴", ":/weather/pic/weather/type/Yin.png");
    mTypeMap.insert("雨", ":/weather/pic/weather/type/Yu.png");
    mTypeMap.insert("雨夹雪", ":/weather/pic/weather/type/YuJiaXue.png");
    mTypeMap.insert("阵雪", ":/weather/pic/weather/type/ZhenXue.png");
    mTypeMap.insert("阵雨", ":/weather/pic/weather/type/ZhenYu.png");
    mTypeMap.insert("中到大雪", ":/weather/pic/weather/type/ZhongDaoDaXue.png");
    mTypeMap.insert("中到大雨", ":/weather/pic/weather/type/ZhongDaoDaYu.png");
    mTypeMap.insert("中雪", ":/weather/pic/weather/type/ZhongXue.png");
    mTypeMap.insert("中雨", ":/weather/pic/weather/type/ZhongYu.png");

    updateUI();

    getWeatherInfo("杭州");
}

WeatherWindow::~WeatherWindow() {}

void WeatherWindow::setupUI()
{
    setAttribute(Qt::WA_StyledBackground, true);

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    mainWidget = new QWidget(this);
    mainWidget->setObjectName("weatherRoot");
    mainWidget->setStyleSheet(
        "QWidget#weatherRoot{border-image: url(:/weather/pic/weather/background.png);}"
        "QLabel{font: 25 10pt \"微软雅黑\";border-radius: 4px;background-color: rgba(60,60,60,100);color: rgb(255,255,255);}"
    );
    rootLayout->addWidget(mainWidget);

    auto *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    auto *headerLayout = new QHBoxLayout;
    btnBack = new QToolButton(mainWidget);
    btnBack->setIcon(QIcon(":/main/pic/main/main.png"));
    btnBack->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnBack->setText("主界面");
    btnBack->setIconSize(QSize(32, 32));
    btnBack->setStyleSheet("color: white; background-color: rgba(255,255,255,0);");

    leCity = new QLineEdit(mainWidget);
    leCity->setPlaceholderText("城市");
    leCity->setFixedWidth(200);
    leCity->setStyleSheet("font: 14pt \"Microsoft YaHei UI\";background-color: rgb(255,255,255);border-radius: 4px;padding: 1px 5px;color: black;");

    btnSearch = new QPushButton(mainWidget);
    btnSearch->setIcon(QIcon(":/weather/pic/weather/search.png"));
    btnSearch->setStyleSheet("background-color: rgba(157,133,255,0);");

    lblDate = new QLabel("2022/08/15 星期四", mainWidget);
    lblDate->setStyleSheet("font: 20pt \"Arial\";background-color: rgba(255,255,255,0);");
    lblDate->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(btnBack);
    headerLayout->addSpacing(10);
    headerLayout->addWidget(leCity);
    headerLayout->addWidget(btnSearch);
    headerLayout->addStretch();
    headerLayout->addWidget(lblDate);

    mainLayout->addLayout(headerLayout);

    auto *contentLayout = new QHBoxLayout;
    contentLayout->setSpacing(10);
    mainLayout->addLayout(contentLayout, 1);

    auto *leftPanel = new QWidget(mainWidget);
    leftPanel->setFixedWidth(340);
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);

    auto *todayPanel = new QWidget(leftPanel);
    todayPanel->setStyleSheet("background-color: rgba(157,133,255,0);border-radius: 15px;");
    auto *todayLayout = new QHBoxLayout(todayPanel);
    todayLayout->setContentsMargins(10, 10, 10, 10);
    todayLayout->setSpacing(8);

    lblTypeIcon = new QLabel(todayPanel);
    lblTypeIcon->setFixedSize(110, 110);
    lblTypeIcon->setStyleSheet("background-color: rgba(255,255,255,0);");
    lblTypeIcon->setScaledContents(true);

    auto *todayTextLayout = new QVBoxLayout;
    auto *tempRow = new QHBoxLayout;
    lblTemp = new QLabel("32°", todayPanel);
    lblTemp->setStyleSheet("color: rgb(255,255,255);font: 50pt \"Arial\";background-color: rgba(0,255,255,0);");
    lblTemp->setAlignment(Qt::AlignCenter);

    lblCity = new QLabel("北京", todayPanel);
    lblCity->setStyleSheet("font: 12pt \"Microsoft YaHei UI\";background-color: rgba(255,255,255,0);");
    lblCity->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    tempRow->addWidget(lblTemp);
    tempRow->addStretch();
    tempRow->addWidget(lblCity);

    auto *typeRow = new QHBoxLayout;
    lblType = new QLabel("晴转多云", todayPanel);
    lblType->setStyleSheet("background-color: rgba(255,255,255,0);font: 12pt \"微软雅黑\";");
    lblLowHigh = new QLabel("19°~31°", todayPanel);
    lblLowHigh->setStyleSheet("background-color: rgba(255,255,255,0);font: 12pt \"微软雅黑\";");
    typeRow->addWidget(lblType);
    typeRow->addSpacing(10);
    typeRow->addWidget(lblLowHigh);
    typeRow->addStretch();

    todayTextLayout->addLayout(tempRow);
    todayTextLayout->addLayout(typeRow);
    todayLayout->addWidget(lblTypeIcon);
    todayLayout->addLayout(todayTextLayout);

    lblGanMao = new QLabel("感冒指数：", leftPanel);
    lblGanMao->setStyleSheet("background-color: rgba(60,60,60,0);padding-left: 5px;padding-right: 5px;");
    lblGanMao->setWordWrap(true);

    auto *indexPanel = new QWidget(leftPanel);
    indexPanel->setStyleSheet("background-color: rgb(157,133,255);border-radius: 15px;");
    auto *indexLayout = new QGridLayout(indexPanel);
    indexLayout->setContentsMargins(40, 25, 40, 25);
    indexLayout->setHorizontalSpacing(50);
    indexLayout->setVerticalSpacing(20);

    auto *windIcon = new QLabel(indexPanel);
    windIcon->setFixedSize(40, 40);
    windIcon->setPixmap(QPixmap(":/weather/pic/weather/wind.png"));
    windIcon->setScaledContents(true);
    windIcon->setStyleSheet("background-color: rgba(255,255,255,0);");

    lblWindFx = new QLabel("西北风", indexPanel);
    lblWindFx->setStyleSheet("background-color: rgba(255,255,255,0);font: 10pt \"微软雅黑\";");
    lblWindFx->setAlignment(Qt::AlignCenter);
    lblWindFl = new QLabel("3级", indexPanel);
    lblWindFl->setStyleSheet("background-color: rgba(255,255,255,0);font: 10pt \"微软雅黑\";");
    lblWindFl->setAlignment(Qt::AlignCenter);

    auto *windLayout = new QVBoxLayout;
    windLayout->addWidget(lblWindFx);
    windLayout->addWidget(lblWindFl);

    auto *pmIcon = new QLabel(indexPanel);
    pmIcon->setFixedSize(40, 40);
    pmIcon->setPixmap(QPixmap(":/weather/pic/weather/pm25.png"));
    pmIcon->setScaledContents(true);
    pmIcon->setStyleSheet("background-color: rgba(255,255,255,0);");
    auto *pmTitle = new QLabel("PM2.5", indexPanel);
    pmTitle->setStyleSheet("background-color: rgba(255,255,255,0);font: 10pt \"微软雅黑\";");
    pmTitle->setAlignment(Qt::AlignCenter);
    lblPM25 = new QLabel("10", indexPanel);
    lblPM25->setStyleSheet("background-color: rgba(255,255,255,0);font: 10pt \"微软雅黑\";");
    lblPM25->setAlignment(Qt::AlignCenter);
    auto *pmLayout = new QVBoxLayout;
    pmLayout->addWidget(pmTitle);
    pmLayout->addWidget(lblPM25);

    auto *shiDuIcon = new QLabel(indexPanel);
    shiDuIcon->setFixedSize(40, 40);
    shiDuIcon->setPixmap(QPixmap(":/weather/pic/weather/humidity.png"));
    shiDuIcon->setScaledContents(true);
    shiDuIcon->setStyleSheet("background-color: rgba(255,255,255,0);");
    auto *shiDuTitle = new QLabel("湿度", indexPanel);
    shiDuTitle->setStyleSheet("background-color: rgba(255,255,255,0);font: 10pt \"微软雅黑\";");
    shiDuTitle->setAlignment(Qt::AlignCenter);
    lblShiDu = new QLabel("60%", indexPanel);
    lblShiDu->setStyleSheet("background-color: rgba(255,255,255,0);font: 10pt \"微软雅黑\";");
    lblShiDu->setAlignment(Qt::AlignCenter);
    auto *shiDuLayout = new QVBoxLayout;
    shiDuLayout->addWidget(shiDuTitle);
    shiDuLayout->addWidget(lblShiDu);

    auto *qualityIcon = new QLabel(indexPanel);
    qualityIcon->setFixedSize(40, 40);
    qualityIcon->setPixmap(QPixmap(":/weather/pic/weather/aqi.png"));
    qualityIcon->setScaledContents(true);
    qualityIcon->setStyleSheet("background-color: rgba(255,255,255,0);");
    auto *qualityTitle = new QLabel("空气质量", indexPanel);
    qualityTitle->setStyleSheet("background-color: rgba(255,255,255,0);font: 10pt \"微软雅黑\";");
    qualityTitle->setAlignment(Qt::AlignCenter);
    lblQuality = new QLabel("优", indexPanel);
    lblQuality->setStyleSheet("background-color: rgba(255,255,255,0);font: 10pt \"微软雅黑\";");
    lblQuality->setAlignment(Qt::AlignCenter);
    auto *qualityLayout = new QVBoxLayout;
    qualityLayout->addWidget(qualityTitle);
    qualityLayout->addWidget(lblQuality);

    auto *windCell = new QWidget(indexPanel);
    auto *windCellLayout = new QHBoxLayout(windCell);
    windCellLayout->setContentsMargins(0, 0, 0, 0);
    windCellLayout->addWidget(windIcon);
    windCellLayout->addLayout(windLayout);

    auto *pmCell = new QWidget(indexPanel);
    auto *pmCellLayout = new QHBoxLayout(pmCell);
    pmCellLayout->setContentsMargins(0, 0, 0, 0);
    pmCellLayout->addWidget(pmIcon);
    pmCellLayout->addLayout(pmLayout);

    auto *shiDuCell = new QWidget(indexPanel);
    auto *shiDuCellLayout = new QHBoxLayout(shiDuCell);
    shiDuCellLayout->setContentsMargins(0, 0, 0, 0);
    shiDuCellLayout->addWidget(shiDuIcon);
    shiDuCellLayout->addLayout(shiDuLayout);

    auto *qualityCell = new QWidget(indexPanel);
    auto *qualityCellLayout = new QHBoxLayout(qualityCell);
    qualityCellLayout->setContentsMargins(0, 0, 0, 0);
    qualityCellLayout->addWidget(qualityIcon);
    qualityCellLayout->addLayout(qualityLayout);

    indexLayout->addWidget(windCell, 0, 0);
    indexLayout->addWidget(pmCell, 0, 1);
    indexLayout->addWidget(shiDuCell, 1, 0);
    indexLayout->addWidget(qualityCell, 1, 1);

    leftLayout->addWidget(todayPanel);
    leftLayout->addWidget(lblGanMao);
    leftLayout->addWidget(indexPanel);
    leftLayout->addStretch();

    auto *rightPanel = new QWidget(mainWidget);
    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(4);

    auto *weekGrid = new QGridLayout;
    weekGrid->setContentsMargins(0, 0, 0, 0);
    weekGrid->setHorizontalSpacing(6);
    weekGrid->setVerticalSpacing(0);

    for (int i = 0; i < 6; ++i) {
        auto *weekLabel = new QLabel("周", rightPanel);
        auto *dateLabel = new QLabel("00/00", rightPanel);
        auto *typeLabel = new QLabel("多云", rightPanel);
        auto *typeIcon = new QLabel(rightPanel);
        auto *aqiLabel = new QLabel("优", rightPanel);
        auto *fxLabel = new QLabel("东风", rightPanel);
        auto *flLabel = new QLabel("2级", rightPanel);

        weekLabel->setAlignment(Qt::AlignCenter);
        dateLabel->setAlignment(Qt::AlignCenter);
        typeLabel->setAlignment(Qt::AlignCenter);
        typeIcon->setAlignment(Qt::AlignCenter);
        typeIcon->setFixedHeight(40);
        typeIcon->setScaledContents(true);
        aqiLabel->setAlignment(Qt::AlignCenter);
        fxLabel->setAlignment(Qt::AlignCenter);
        flLabel->setAlignment(Qt::AlignCenter);

        mWeekList << weekLabel;
        mDateList << dateLabel;
        mTypeList << typeLabel;
        mTypeIconList << typeIcon;
        mAqiList << aqiLabel;
        mFxList << fxLabel;
        mFlList << flLabel;

        weekGrid->addWidget(weekLabel, 0, i);
        weekGrid->addWidget(dateLabel, 1, i);
        weekGrid->addWidget(typeIcon, 2, i);
        weekGrid->addWidget(typeLabel, 3, i);
    }

    auto *aqiGrid = new QGridLayout;
    aqiGrid->setContentsMargins(0, 0, 0, 0);
    aqiGrid->setHorizontalSpacing(6);
    for (int i = 0; i < 6; ++i) {
        aqiGrid->addWidget(mAqiList[i], 0, i);
    }

    auto *fxGrid = new QGridLayout;
    fxGrid->setContentsMargins(0, 0, 0, 0);
    fxGrid->setHorizontalSpacing(6);
    for (int i = 0; i < 6; ++i) {
        fxGrid->addWidget(mFxList[i], 0, i);
    }

    auto *flGrid = new QGridLayout;
    flGrid->setContentsMargins(0, 0, 0, 0);
    flGrid->setHorizontalSpacing(6);
    for (int i = 0; i < 6; ++i) {
        flGrid->addWidget(mFlList[i], 0, i);
    }

    auto *curveWidget = new QWidget(rightPanel);
    auto *curveLayout = new QVBoxLayout(curveWidget);
    curveLayout->setContentsMargins(0, 0, 0, 0);
    curveLayout->setSpacing(0);
    lblHighCurve = new QLabel(curveWidget);
    lblHighCurve->setMinimumHeight(80);
    lblHighCurve->setStyleSheet("border-bottom-left-radius: 0px;border-bottom-right-radius: 0px;");
    lblLowCurve = new QLabel(curveWidget);
    lblLowCurve->setMinimumHeight(80);
    lblLowCurve->setStyleSheet("border-top-left-radius: 0px;border-top-right-radius: 0px;");
    curveLayout->addWidget(lblHighCurve);
    curveLayout->addWidget(lblLowCurve);

    rightLayout->addLayout(weekGrid);
    rightLayout->addWidget(curveWidget);
    rightLayout->addLayout(aqiGrid);
    rightLayout->addLayout(fxGrid);
    rightLayout->addLayout(flGrid);
    rightLayout->addStretch();

    contentLayout->addWidget(leftPanel);
    contentLayout->addWidget(rightPanel, 1);

    connect(btnBack, &QToolButton::clicked, this, &WeatherWindow::backRequested);
    connect(btnSearch, &QPushButton::clicked, this, &WeatherWindow::onSearchClicked);
}

void WeatherWindow::getWeatherInfo(QString cityName)
{
    QString cityCode = WeatherTool::getCityCode(cityName);
    if (cityCode.isEmpty()) {
        QMessageBox::warning(this, "天气", "请检查输入是否正确！", QMessageBox::Ok);
        return;
    }

    QString base = "http://t.weather.itboy.net/api/weather/city/";
    QUrl url(base + cityCode);
    mNetAccessManager->get(QNetworkRequest(url));
}

void WeatherWindow::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &err);
    if (err.error != QJsonParseError::NoError) {
        return;
    }

    QJsonObject rootObj = doc.object();
    QString message = rootObj.value("message").toString();
    if (!message.contains("success")) {
        QMessageBox::warning(this, "天气", "请求数据失败！", QMessageBox::Ok);
        return;
    }

    mToday.date = rootObj.value("date").toString();
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();

    QJsonObject objData = rootObj.value("data").toObject();

    QJsonObject objYestody = objData.value("yesterday").toObject();
    mDay[0].week = objYestody.value("week").toString();
    mDay[0].date = objYestody.value("ymd").toString();
    mDay[0].type = objYestody.value("type").toString();

    QString s;
    s = objYestody.value("high").toString().split(" ").at(1);
    s = s.left(s.length() - 1);
    mDay[0].high = s.toInt();
    s = objYestody.value("low").toString().split(" ").at(1);
    s = s.left(s.length() - 1);
    mDay[0].low = s.toInt();
    mDay[0].fx = objYestody.value("fx").toString();
    mDay[0].fl = objYestody.value("fl").toString();
    mDay[0].aqi = objYestody.value("aqi").toDouble();

    QJsonArray forecastArr = objData.value("forecast").toArray();
    for (int i = 0; i < 5; i++) {
        QJsonObject objForcast = forecastArr[i].toObject();
        mDay[i + 1].week = objForcast.value("week").toString();
        mDay[i + 1].date = objForcast.value("ymd").toString();
        mDay[i + 1].type = objForcast.value("type").toString();

        QString s;
        s = objForcast.value("high").toString().split(" ").at(1);
        s = s.left(s.length() - 1);
        mDay[i + 1].high = s.toInt();
        s = objForcast.value("low").toString().split(" ").at(1);
        s = s.left(s.length() - 1);
        mDay[i + 1].low = s.toInt();
        mDay[i + 1].fx = objForcast.value("fx").toString();
        mDay[i + 1].fl = objForcast.value("fl").toString();
        mDay[i + 1].aqi = objForcast.value("aqi").toDouble();
    }

    mToday.ganmao = objData.value("ganmao").toString();

    double wendu = objData.value("wendu").toString().toDouble();
    mToday.wendu = qRound(wendu);

    mToday.shidu = objData.value("shidu").toString();
    mToday.pm25 = objData.value("pm25").toDouble();
    mToday.quality = objData.value("quality").toString();

    mToday.type = mDay[1].type;
    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;
    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    updateUI();
    lblHighCurve->update();
    lblLowCurve->update();
}

void WeatherWindow::updateUI()
{
    lblDate->setText(QDateTime::fromString(mToday.date, "yyyyMMdd").toString("yyyy/MM/dd") + " " + mDay[1].week);
    lblCity->setText(mToday.city);
    lblTypeIcon->setPixmap(QPixmap(mTypeMap.value(mToday.type, ":/weather/pic/weather/type/undefined.png")));
    lblTemp->setText(QString::number(mToday.wendu) + "°");
    lblType->setText(mToday.type);
    lblLowHigh->setText(QString::number(mToday.low) + "~" + QString::number(mToday.high) + "°C");
    lblGanMao->setText("感冒指数：" + mToday.ganmao);
    lblWindFx->setText(mToday.fx);
    lblWindFl->setText(mToday.fl);
    lblPM25->setText(QString::number(mToday.pm25));
    lblShiDu->setText(mToday.shidu);
    lblQuality->setText(mToday.quality);

    for (int i = 0; i < 6; i++) {
        mWeekList[i]->setText("周" + mDay[i].week.right(1));
        mWeekList[0]->setText("昨天");
        mWeekList[1]->setText("今天");
        mWeekList[2]->setText("明天");
        QStringList ymdList = mDay[i].date.split("-");
        if (ymdList.size() >= 3) {
            mDateList[i]->setText(ymdList[1] + "/" + ymdList[2]);
        }

        mTypeIconList[i]->setPixmap(QPixmap(mTypeMap.value(mDay[i].type, ":/weather/pic/weather/type/undefined.png")));
        mTypeList[i]->setText(mDay[i].type);

        if (mDay[i].aqi >= 0 && mDay[i].aqi <= 50) {
            mAqiList[i]->setText("优");
            mAqiList[i]->setStyleSheet("background-color: rgb(121, 184, 0);");
        } else if (mDay[i].aqi > 50 && mDay[i].aqi <= 100) {
            mAqiList[i]->setText("良");
            mAqiList[i]->setStyleSheet("background-color: rgb(255, 187, 23);");
        } else if (mDay[i].aqi > 100 && mDay[i].aqi <= 150) {
            mAqiList[i]->setText("轻度");
            mAqiList[i]->setStyleSheet("background-color: rgb(255, 87, 97);");
        } else if (mDay[i].aqi > 150 && mDay[i].aqi <= 200) {
            mAqiList[i]->setText("中度");
            mAqiList[i]->setStyleSheet("background-color: rgb(235, 17, 27);");
        } else if (mDay[i].aqi > 200 && mDay[i].aqi <= 300) {
            mAqiList[i]->setText("重度");
            mAqiList[i]->setStyleSheet("background-color: rgb(170, 0, 0);");
        } else {
            mAqiList[i]->setText("严重");
            mAqiList[i]->setStyleSheet("background-color: rgb(110, 0, 0);");
        }

        mFxList[i]->setText(mDay[i].fx);
        mFlList[i]->setText(mDay[i].fl);
    }
}

void WeatherWindow::onReplied(QNetworkReply *reply)
{
    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (reply->error() != QNetworkReply::NoError || status_code != 200) {
        QMessageBox::warning(this, "天气", "请求数据失败！", QMessageBox::Ok);
    } else {
        QByteArray byteArray = reply->readAll();
        parseJson(byteArray);
    }

    reply->deleteLater();
}

void WeatherWindow::onSearchClicked()
{
    QString cityName = leCity->text();
    getWeatherInfo(cityName);
}

bool WeatherWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == lblHighCurve && event->type() == QEvent::Paint) {
        paintHighCurve();
    }
    if (watched == lblLowCurve && event->type() == QEvent::Paint) {
        paintLowCurve();
    }
    return QWidget::eventFilter(watched, event);
}

void WeatherWindow::paintHighCurve()
{
    QPainter painter(lblHighCurve);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int pointX[6] = {0};
    for (int i = 0; i < 6; i++) {
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width() / 2;
    }

    int tempSum = 0;
    for (int i = 0; i < 6; i++) {
        tempSum += mDay[i].high;
    }

    int tempAverage = tempSum / 6;
    int pointY[6] = {0};
    int yCenter = lblHighCurve->height() / 2;
    for (int i = 0; i < 6; i++) {
        pointY[i] = yCenter - ((mDay[i].high - tempAverage) * INCREMENT);
    }

    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(255, 170, 0));
    painter.save();
    painter.setPen(pen);
    painter.setBrush(QColor(255, 170, 0));

    for (int i = 0; i < 6; i++) {
        painter.drawEllipse(QPoint(pointX[i], pointY[i]), POINT_RADIUS, POINT_RADIUS);
        painter.drawText(QPoint(pointX[i] - TEXT_OFFSET_X, pointY[i] - TEXT_OFFSET_Y), QString::number(mDay[i].high) + "°");
    }

    for (int i = 0; i < 5; i++) {
        if (i == 0) {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        } else {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i], pointY[i], pointX[i + 1], pointY[i + 1]);
    }

    painter.restore();
}

void WeatherWindow::paintLowCurve()
{
    QPainter painter(lblLowCurve);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int pointX[6] = {0};
    for (int i = 0; i < 6; i++) {
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width() / 2;
    }

    int tempSum = 0;
    for (int i = 0; i < 6; i++) {
        tempSum += mDay[i].low;
    }

    int tempAverage = tempSum / 6;
    int pointY[6] = {0};
    int yCenter = lblLowCurve->height() / 2;
    for (int i = 0; i < 6; i++) {
        pointY[i] = yCenter - ((mDay[i].low - tempAverage) * INCREMENT);
    }

    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(255, 170, 0));
    painter.save();
    painter.setPen(pen);
    painter.setBrush(QColor(255, 170, 0));

    for (int i = 0; i < 6; i++) {
        painter.drawEllipse(QPoint(pointX[i], pointY[i]), POINT_RADIUS, POINT_RADIUS);
        painter.drawText(QPoint(pointX[i] - TEXT_OFFSET_X, pointY[i] - TEXT_OFFSET_Y), QString::number(mDay[i].low) + "°");
    }

    for (int i = 0; i < 5; i++) {
        if (i == 0) {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        } else {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i], pointY[i], pointX[i + 1], pointY[i + 1]);
    }

    painter.restore();
}
