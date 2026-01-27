#include "musicwindow.h"

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
#include <QCoreApplication>
#include <QStyle>



#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioOutput>
#endif

MusicWindow::MusicWindow(QWidget *parent)
    : QWidget(parent)
    , player(new QMediaPlayer(this))
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    , audioOutput(new QAudioOutput(this))
#endif
    , currentIndex(-1)
    , listWidget(new QListWidget(this))
    , playButton(new QToolButton(this))
    , volumeSlider(new QSlider(Qt::Horizontal, this))
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    player->setAudioOutput(audioOutput);
#endif
#if 1
    mediaPlaylist = new QMediaPlaylist(this);
    scanSongs();
    //qDebug()<<"musicwindow"<<endl;
    this->setObjectName("MusicWindow");
    //this->setStyleSheet("QWidget{ background-color: black; }");


    auto *backButton = new QToolButton(this);
    backButton->setText("主界面");
    backButton->setIcon(QIcon(":/music/pic/music/main.png"));
    backButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    backButton->setIconSize(QSize(36, 36));

    auto *titleLabel = new QLabel("音乐播放器", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white; font-size: 22px;");

    auto *headerLayout = new QHBoxLayout;
    headerLayout->addWidget(backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    //headerLayout->setStyleSheet("MusicWindow { background-color: black; }");

    listWidget->setMinimumWidth(360);
    listWidget->setStyleSheet(
        "QListWidget { border: none; }"  // 消除列表框默认边框（关键）
        "QListWidget::item { color: white; }"  // 列表项文字设为白色
    );

    artworkLabel = new QLabel(this);
    artworkLabel->setMinimumSize(320, 320);
    artworkLabel->setAlignment(Qt::AlignCenter);
    //artworkLabel->setStyleSheet("background-color: #ffffff; border-radius: 130px;" );
    QImage Image;
    Image.load(":/music/pic/music/cd.png");
    QPixmap pixmap = QPixmap::fromImage(Image);
    int with = 320;
    int height = 320;
    QPixmap fitpixmap =
            pixmap.scaled(with, height,
                          Qt::IgnoreAspectRatio,
                          Qt::SmoothTransformation);
    artworkLabel->setPixmap(fitpixmap);


    positionSlider = new QSlider(Qt::Horizontal, this);
    positionSlider->setRange(0, 0);

    currentTimeLabel = new QLabel("00:00", this);
    durationLabel = new QLabel("00:00", this);

    auto *prevButton = new QToolButton(this);
    prevButton->setIcon(QIcon(":/music/pic/music/prev.png"));//
    prevButton->setIconSize(QSize(36, 36));

    const QIcon playIcon(":/music/pic/music/play.png");
    const QIcon pauseIcon (":/music/pic/music/stop.png");

    playButton->setIcon(playIcon);
    playButton->setIconSize(QSize(42, 42));

    auto *nextButton = new QToolButton(this);
    nextButton->setIcon(QIcon(":/music/pic/music/next.png"));
    nextButton->setIconSize(QSize(36, 36));

//    auto *stopButton = new QToolButton(this);
//    stopButton->setIcon(QIcon(":/music/pic/music/stop.png"));
//    stopButton->setIconSize(QSize(32, 32));

    auto *addButton = new QToolButton(this);
    addButton->setIcon(QIcon(":/music/pic/music/addfiles.png"));
    addButton->setIconSize(QSize(32, 32));

    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(60);

    auto *volumeIcon = new QLabel(this);
    volumeIcon->setPixmap(QIcon(":/music/pic/music/volume.png").pixmap(24, 24));

    auto *controlsLayout = new QHBoxLayout;
    controlsLayout->addStretch();
    controlsLayout->addWidget(prevButton);
    controlsLayout->addWidget(playButton);
    controlsLayout->addWidget(nextButton);
    //controlsLayout->addWidget(stopButton);
    controlsLayout->addSpacing(30);
    controlsLayout->addWidget(volumeIcon);
    controlsLayout->addWidget(volumeSlider);
    controlsLayout->addSpacing(10);
    controlsLayout->addWidget(addButton);
    controlsLayout->addStretch();

    auto *centerLayout = new QHBoxLayout;
    centerLayout->addWidget(listWidget, 3);
    centerLayout->addStretch();
    centerLayout->addWidget(artworkLabel, 2);

    auto *progressLayout = new QHBoxLayout;
    progressLayout->addWidget(currentTimeLabel);
    progressLayout->addWidget(positionSlider);
    progressLayout->addWidget(durationLabel);

    auto *pageLayout = new QVBoxLayout(this);
    pageLayout->addLayout(headerLayout);
    pageLayout->addSpacing(12);
    pageLayout->addLayout(centerLayout);
    pageLayout->addSpacing(10);
    pageLayout->addLayout(progressLayout);
    pageLayout->addSpacing(8);
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
//#else
    connect(backButton, &QToolButton::clicked, this, &MusicWindow::backRequested);
    connect(addButton, &QToolButton::clicked, this, [this]() {
        QFileDialog dialog(this, "选择音频文件"); // 创建对话框实例
        dialog.setFileMode(QFileDialog::ExistingFiles); // 设置为「多文件选择」模式（对应原静态方法）
        dialog.setNameFilter("Music Files (*.mp3 *.wav *.flac *.aac)"); // 设置文件过滤器（与原代码一致）
        dialog.setStyleSheet("background-color: white; color: black;");
        if (dialog.exec() == QDialog::Accepted) {
            const QStringList files = dialog.selectedFiles();
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
    /*上一首*/
    connect(prevButton, &QToolButton::clicked, this, [this]() {
        if (mediaFiles.isEmpty()) {
            return;
        }
        int nextIndex = currentIndex <= 0 ? mediaFiles.size() - 1 : currentIndex - 1;
        playIndex(nextIndex);
    });
    /*下一首*/
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
    /*音乐播放暂停*/
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
    /*音乐播放暂停图标切换*/
    connect(player, &QMediaPlayer::stateChanged, this, [updatePlayIcon]() {
        updatePlayIcon();
    });
#endif
    /*播放进度条*/
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
    /*音量调节*/
    connect(volumeSlider, &QSlider::valueChanged, this, [this](int value) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        audioOutput->setVolume(value / 100.0);
#else
        player->setVolume(value);
#endif
    });
#endif
}

void MusicWindow::playIndex(int index)
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

void MusicWindow::scanSongs()
{
    QDir dir(QCoreApplication::applicationDirPath()
             + "/Music");
    QDir dirbsolutePath(dir.absolutePath());
    qDebug() << "【方法1】最终绝对路径 = " << dirbsolutePath.absolutePath();
    /* 如果目录存在 */
    if (dirbsolutePath.exists()) {
        /* 定义过滤器 */
        QStringList filter;
        /* 包含所有.mp3后缀的文件 */
        filter << "*.mp3";
        /* 获取该目录下的所有文件 */
        QFileInfoList files =
                dirbsolutePath.entryInfoList(filter, QDir::Files);

        /* 遍历 */
        for (int i = 0; i < files.count(); i++) {
            MusicObjectInfo info;
            /* 使用utf-8编码 */
            QString fileName = QString::fromUtf8(files.at(i)
                                                 .fileName()
                                                 .replace(".mp3", "")
                                                 .toUtf8()
                                                 .data());
            info.fileName = fileName + "\n"
                    + fileName.split("-").at(1);
            info.filePath = QString::fromUtf8(files.at(i)
                                              .filePath()
                                              .toUtf8()
                                              .data());
            /* 媒体列表添加歌曲 */
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
                mediaFiles.append(info.filePath); // 有效则存入mediaFiles
                musicObjectInfo.append(info);
                listWidget->addItem(info.fileName); // 列表展示文件名
            }
            else
            {
                qDebug()<<"mediaFiles is empty"
                        << endl;
            }
        }
    }
}
