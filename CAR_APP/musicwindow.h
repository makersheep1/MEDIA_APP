#ifndef MUSICWINDOW_H
#define MUSICWINDOW_H

#include <QWidget>
#include <QStringList>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QSpacerItem>
#include <QDebug>
QT_BEGIN_NAMESPACE
class QMediaPlayer;
class QAudioOutput;
class QToolButton;
class QSlider;
class QListWidget;
class QLabel;
QT_END_NAMESPACE
/* 媒体信息结构体 */
struct MusicObjectInfo {
    /* 用于保存歌曲文件名 */
    QString fileName;
    /* 用于保存歌曲文件路径 */
    QString filePath;
};
class MusicWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MusicWindow(QWidget *parent = nullptr);

signals:
    void backRequested();

private:
    void playIndex(int index);
    void scanSongs();

    QMediaPlayer *player;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QAudioOutput *audioOutput;
#endif
    QStringList mediaFiles;
    int currentIndex;
    QListWidget *listWidget;
    QLabel *artworkLabel;
    QSlider *positionSlider;
    QLabel *currentTimeLabel;
    QLabel *durationLabel;
    QToolButton *playButton;
    QSlider *volumeSlider;
    /* 媒体列表 */
    QMediaPlaylist *mediaPlaylist;
    /* 媒体信息存储 */
    QVector<MusicObjectInfo> musicObjectInfo;
};

#endif // MUSICWINDOW_H
