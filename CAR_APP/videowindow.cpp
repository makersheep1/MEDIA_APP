#include "videowindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QMediaPlayer>
#include <QSlider>
#include <QSize>
#include <QTime>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QVideoWidget>
#include <QCoreApplication>
#include <QStyle>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioOutput>
#endif

VideoWindow::VideoWindow(QWidget *parent)
    : QWidget(parent)
    , player(new QMediaPlayer(this))
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    , audioOutput(new QAudioOutput(this))
#endif
    , currentIndex(-1)
    , listWidget(new QListWidget(this))
    , playButton(new QToolButton(this))
    , volumeSlider(new QSlider(Qt::Horizontal, this))
    , videoWidget(new QVideoWidget(this))
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    player->setAudioOutput(audioOutput);
#endif
    player->setVideoOutput(videoWidget);
    scanVideoFiles();
    setObjectName("VideoWindow");
    setStyleSheet("#VideoWindow { background-color: #1e1f24; } QLabel { color: #ffffff; } QListWidget { background-color: #2a2b32; color: #ffffff; border: none; } QListWidget::item:selected { background-color: #3a3b44; } QToolButton { color: #ffffff; } QSlider::groove:horizontal { height: 4px; background: #3a3b44; border-radius: 2px; } QSlider::handle:horizontal { width: 12px; margin: -4px 0; background: #ffffff; border-radius: 6px; }");

    auto *backButton = new QToolButton(this);
    backButton->setText("主界面");
    backButton->setIcon(QIcon(":/media/pic/video/quit.png"));
    backButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    backButton->setIconSize(QSize(36, 36));

    auto *titleLabel = new QLabel("视频播放器", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white; font-size: 22px;");

    auto *headerLayout = new QHBoxLayout;
    headerLayout->addWidget(backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    videoWidget->setMinimumHeight(320);

    listWidget->setMinimumWidth(260);
    listWidget->setStyleSheet(
        "QListWidget { border: none; }"  // 消除列表框默认边框（关键）
        "QListWidget::item { color: white; }"  // 列表项文字设为白色
    );

    positionSlider = new QSlider(Qt::Horizontal, this);
    positionSlider->setRange(0, 0);

    currentTimeLabel = new QLabel("00:00", this);
    durationLabel = new QLabel("00:00", this);

    auto *prevButton = new QToolButton(this);
    prevButton->setIcon(QIcon(":/media/pic/video/prev.png"));
    prevButton->setIconSize(QSize(36, 36));

    const QIcon playIcon(":/media/pic/video/play.png");
    const QIcon pauseIcon(":/media/pic/video/stop.png");

    playButton->setIcon(playIcon);
    playButton->setIconSize(QSize(42, 42));

    auto *nextButton = new QToolButton(this);
    nextButton->setIcon(QIcon(":/media/pic/video/next.png"));
    nextButton->setIconSize(QSize(36, 36));

//    auto *stopButton = new QToolButton(this);
//    stopButton->setIcon(QIcon(":/media/pic/video/stop.png"));
//    stopButton->setIconSize(QSize(32, 32));

    auto *addButton = new QToolButton(this);
    addButton->setIcon(QIcon(":/media/pic/video/addfile.png"));
    addButton->setIconSize(QSize(32, 32));

    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(60);

    auto *volumeIcon = new QLabel(this);
    volumeIcon->setPixmap(QIcon(":/media/pic/video/volume.png").pixmap(24, 24));

    auto *controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget(prevButton);
    controlsLayout->addWidget(playButton);
    controlsLayout->addWidget(nextButton);
    //controlsLayout->addWidget(stopButton);
    controlsLayout->addSpacing(14);
    controlsLayout->addWidget(currentTimeLabel);
    controlsLayout->addWidget(positionSlider, 1);
    controlsLayout->addWidget(durationLabel);
    controlsLayout->addSpacing(14);
    controlsLayout->addWidget(volumeIcon);
    controlsLayout->addWidget(volumeSlider);
    controlsLayout->addSpacing(10);
    controlsLayout->addWidget(addButton);

    auto *contentLayout = new QHBoxLayout;
    contentLayout->addWidget(videoWidget, 3);
    contentLayout->addWidget(listWidget, 1);

    auto *pageLayout = new QVBoxLayout(this);
    pageLayout->addLayout(headerLayout);
    pageLayout->addSpacing(8);
    pageLayout->addLayout(contentLayout);
    pageLayout->addSpacing(6);
    pageLayout->addLayout(controlsLayout);

    auto updatePlayIcon = [this, playIcon, pauseIcon]() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (player->playbackState() == QMediaPlayer::PlayingState) {
            playButton->setIcon(pauseIcon);
        } else {
            playButton->setIcon(playIcon);
        }
#else
        if (player->state() == QMediaPlayer::PlayingState) {
            playButton->setIcon(pauseIcon);
        } else {
            playButton->setIcon(playIcon);
        }
#endif
    };

    connect(backButton, &QToolButton::clicked, this, &VideoWindow::backRequested);
    connect(addButton, &QToolButton::clicked, this, [this]() {
        QFileDialog dialog(this, "选择视频文件"); // 创建对话框实例
        dialog.setFileMode(QFileDialog::ExistingFiles); // 设置为「多文件选择」模式（对应原静态方法）
        dialog.setNameFilter("Video Files (*.mp4 *.avi *.mkv *.mov)"); // 设置文件过滤器（与原代码一致）
        // 核心：设置对话框白色背景，覆盖继承的黑色样式
        dialog.setStyleSheet("background-color: white; color: black;");
        // 弹出对话框，判断是否选择了文件
        if (dialog.exec() == QDialog::Accepted) {
            const QStringList files = dialog.selectedFiles(); // 获取选中的文件路径列表
            // 后续原逻辑不变（遍历、缓存、列表展示...）
            for (const QString &file : files) {
                mediaFiles.append(file);
                listWidget->addItem(QFileInfo(file).fileName());
            }
            if (currentIndex < 0 && !mediaFiles.isEmpty()) {
                playIndex(0);
            }
        }
    });
    connect(listWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        int index = listWidget->row(item);
        playIndex(index);
    });
    connect(prevButton, &QToolButton::clicked, this, [this]() {
        if (mediaFiles.isEmpty()) {
            return;
        }
        int nextIndex = currentIndex <= 0 ? mediaFiles.size() - 1 : currentIndex - 1;
        playIndex(nextIndex);
    });
    connect(nextButton, &QToolButton::clicked, this, [this]() {
        if (mediaFiles.isEmpty()) {
            return;
        }
        int nextIndex = currentIndex >= mediaFiles.size() - 1 ? 0 : currentIndex + 1;
        playIndex(nextIndex);
    });
//    connect(stopButton, &QToolButton::clicked, this, [this, updatePlayIcon]() {
//        player->stop();
//        updatePlayIcon();
//    });
    connect(playButton, &QToolButton::clicked, this, [this]() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (player->playbackState() == QMediaPlayer::PlayingState) {
            player->pause();
        } else if (!mediaFiles.isEmpty()) {
            if (currentIndex < 0) {
                playIndex(0);
            } else {
                player->play();
            }
        }
#else
        if (player->state() == QMediaPlayer::PlayingState) {
            player->pause();
        } else if (!mediaFiles.isEmpty()) {
            if (currentIndex < 0) {
                playIndex(0);
            } else {
                player->play();
            }
        }
#endif
    });

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(player, &QMediaPlayer::playbackStateChanged, this, [updatePlayIcon]() {
        updatePlayIcon();
    });
#else
    connect(player, &QMediaPlayer::stateChanged, this, [updatePlayIcon]() {
        updatePlayIcon();
    });
#endif
    connect(positionSlider, &QSlider::sliderMoved, this, [this](int value) {
        player->setPosition(value);
    });
    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 position) {
        if (!positionSlider->isSliderDown()) {
            positionSlider->setValue(static_cast<int>(position));
        }
        currentTimeLabel->setText(QTime(0, 0).addMSecs(static_cast<int>(position)).toString("mm:ss"));
    });
    connect(player, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        positionSlider->setRange(0, static_cast<int>(duration));
        durationLabel->setText(QTime(0, 0).addMSecs(static_cast<int>(duration)).toString("mm:ss"));
    });
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia && !mediaFiles.isEmpty()) {
            int nextIndex = currentIndex >= mediaFiles.size() - 1 ? 0 : currentIndex + 1;
            playIndex(nextIndex);
        }
    });
    connect(volumeSlider, &QSlider::valueChanged, this, [this](int value) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        audioOutput->setVolume(value / 100.0);
#else
        player->setVolume(value);
#endif
    });
}

void VideoWindow::playIndex(int index)
{
    if (index < 0 || index >= mediaFiles.size()) {
        return;
    }
    currentIndex = index;
    listWidget->setCurrentRow(index);
    const QUrl url = QUrl::fromLocalFile(mediaFiles.at(index));
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    player->setSource(url);
#else
    player->setMedia(url);
#endif
    player->play();
}

void VideoWindow::scanVideoFiles()
{
    QDir dir(QCoreApplication::applicationDirPath()
             + "/Video");
    QDir dirbsolutePath(dir.absolutePath());
    /* 如果目录存在 */
    if (dirbsolutePath.exists()) {
        /* 定义过滤器 */
        QStringList filter;
        /* 包含所有xx后缀的文件 */
        filter << "*.mp4" << "*.mkv" << "*.wmv" << "*.avi";
        /* 获取该目录下的所有文件 */
        QFileInfoList files =
                dirbsolutePath.entryInfoList(filter, QDir::Files);
        /* 遍历 */
        for (int i = 0; i < files.count(); i++) {
            VideoObjectInfo info;
            /* 使用utf-8编码 */
            info.fileName = QString::fromUtf8(files.at(i)
                                              .fileName()
                                              .toUtf8()
                                              .data());
            info.filePath = QString::fromUtf8(files.at(i)
                                              .filePath()
                                              .toUtf8()
                                              .data());
            // 1. 初始化QMediaPlayer（临时用于校验，无需设置父部件）
            QMediaPlayer mediaPlayer;
            // 2. 构造待校验的媒体文件URL（本地文件路径转QUrl）
            QUrl mediaUrl = QUrl::fromLocalFile(info.filePath);
            // 3. 设置待校验的媒体文件到QMediaPlayer
            mediaPlayer.setMedia(QMediaContent(mediaUrl));
            //qDebug() <<mediaPlayer.mediaStatus()<<endl;
            // 4. 核心：通过mediaStatus判断文件是否有效
            if (mediaPlayer.mediaStatus() == QMediaPlayer::LoadingMedia)
            {
                /* 添加到媒体播放列表 */
                mediaFiles.append(info.filePath);
                /* 添加到容器数组里储存 */
                videoObjectInfo.append(info);
                /* 添加视频名字至列表 */
                listWidget->addItem(info.fileName);
            } else {
                qDebug()<<"mediaFiles is empty"
                        << endl;
            }
        }
    }
}
