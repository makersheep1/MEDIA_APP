#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "musicwindow.h"
#include "videowindow.h"
#include "weatherwindow.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setGeometry(0, 0, 800, 480);
    this->setMinimumSize(800, 480);
    this->setMaximumSize(800, 480);
    this->statusBar()->hide();
    stacked = new QStackedWidget(ui->centralwidget);
    auto *rootLayout = new QVBoxLayout(ui->centralwidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(stacked);

    createHomePage();

    musicPage = new MusicWindow(this);
    // 步骤1：启用自动背景填充（QWidget子类必加，否则背景色不绘制）
    musicPage->setAttribute(Qt::WA_StyledBackground, true);
    // 步骤2：精准设置MusicWindow本身黑色背景，子控件/布局无继承
    musicPage->setStyleSheet("background-color: #25242A;");

    videoPage = new VideoWindow(this);
    videoPage->setAttribute(Qt::WA_StyledBackground, true);
    videoPage->setStyleSheet("background-color: #25242A;");

    weatherPage = new WeatherWindow(this);
    weatherPage->setAttribute(Qt::WA_StyledBackground, true);

    stacked->addWidget(homePage);
    stacked->addWidget(musicPage);
    stacked->addWidget(videoPage);
    stacked->addWidget(weatherPage);
    stacked->setCurrentWidget(homePage);

    connect(musicButton, &QToolButton::clicked, this, [this]() {
        stacked->setCurrentWidget(musicPage);
    });
    connect(videoButton, &QToolButton::clicked, this, [this]() {
        stacked->setCurrentWidget(videoPage);
    });
    connect(weatherButton, &QToolButton::clicked, this, [this]() {
        stacked->setCurrentWidget(weatherPage);
    });
    connect(musicPage, &MusicWindow::backRequested, this, &MainWindow::showHome);
    connect(videoPage, &VideoWindow::backRequested, this, &MainWindow::showHome);
    connect(weatherPage, &WeatherWindow::backRequested, this, &MainWindow::showHome);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createHomePage()
{
    homePage = new QWidget(this);
    homePage->setStyleSheet("QWidget { background-color: #25242A; }");

    //musicButton->setStyleSheet("border: none;");
    musicButton = new QToolButton(homePage);
    musicButton->setText("音乐播放器");
    musicButton->setStyleSheet("color: white; background-color: #25242A; border: none;");
    musicButton->setIcon(QIcon(":/main/pic/main/music.png"));
    musicButton->setIconSize(QSize(180, 180));
    musicButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    musicButton->setMinimumSize(220, 240);


    videoButton = new QToolButton(homePage);
    videoButton->setText("视频播放器");
    videoButton->setStyleSheet("color: white; background-color: #25242A; border: none;");
    videoButton->setIcon(QIcon(":/main/pic/main/video.png"));
    videoButton->setIconSize(QSize(180, 180));
    videoButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    videoButton->setMinimumSize(220, 240);

    weatherButton = new QToolButton(homePage);
    weatherButton->setText("天气预报");
    weatherButton->setStyleSheet("color: white; background-color: #25242A; border: none;");
    weatherButton->setIcon(QIcon(":/main/pic/main/weather.png"));
    weatherButton->setIconSize(QSize(180, 180));
    weatherButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    weatherButton->setMinimumSize(220, 240);

    //videoButton->setStyleSheet("border: none;");

    auto *rowLayout = new QHBoxLayout;
    rowLayout->addStretch();
    rowLayout->addWidget(musicButton);
    rowLayout->addSpacing(40);
    rowLayout->addWidget(videoButton);
    rowLayout->addSpacing(40);
    rowLayout->addWidget(weatherButton);
    rowLayout->addStretch();

    auto *pageLayout = new QVBoxLayout(homePage);
    pageLayout->addStretch();
    pageLayout->addLayout(rowLayout);
    pageLayout->addStretch();
}

void MainWindow::showHome()
{
    stacked->setCurrentWidget(homePage);
}
