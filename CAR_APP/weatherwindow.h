#ifndef WEATHERWINDOW_H
#define WEATHERWINDOW_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLabel>
#include <QList>
#include <QMap>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include "weatherdata.h"

// 温度曲线相关的宏
#define INCREMENT     3   // 温度每升高/降低1度，y轴坐标的增量
#define POINT_RADIUS  3   // 曲线描点的大小
#define TEXT_OFFSET_X 12  // 温度文本相对于点的偏移
#define TEXT_OFFSET_Y 10  // 温度文本相对于点的偏移

class WeatherWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherWindow(QWidget *parent = nullptr);
    ~WeatherWindow();

signals:
    void backRequested();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintHighCurve();
    void paintLowCurve();

private slots:
    void onReplied(QNetworkReply* reply);
    void onSearchClicked();

private:
    void setupUI();
    void getWeatherInfo(QString cityName);
    void parseJson(QByteArray& byteArray);
    void updateUI();

    QNetworkAccessManager* mNetAccessManager;
    Today mToday;
    Day mDay[6];

    // 主容器
    QWidget* mainWidget;

    // 顶部控件
    QLabel* lblDate;
    QLabel* lblCity;
    QLineEdit* leCity;
    QPushButton* btnSearch;
    QToolButton* btnBack;

    // 当前天气控件
    QLabel* lblTypeIcon;
    QLabel* lblTemp;
    QLabel* lblType;
    QLabel* lblLowHigh;
    QLabel* lblGanMao;
    QLabel* lblWindFx;
    QLabel* lblWindFl;
    QLabel* lblPM25;
    QLabel* lblShiDu;
    QLabel* lblQuality;

    // 星期和日期
    QList<QLabel*> mWeekList;
    QList<QLabel*> mDateList;

    // 天气和天气图标
    QList<QLabel*> mTypeList;
    QList<QLabel*> mTypeIconList;

    // 天气指数
    QList<QLabel*> mAqiList;

    // 风向和风力
    QList<QLabel*> mFxList;
    QList<QLabel*> mFlList;

    // 温度曲线
    QLabel* lblHighCurve;
    QLabel* lblLowCurve;

    // 天气类型图标映射
    QMap<QString, QString> mTypeMap;
};

#endif // WEATHERWINDOW_H
