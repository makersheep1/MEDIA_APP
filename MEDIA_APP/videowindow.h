#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QWidget>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QMediaPlayer;
class QAudioOutput;
class QToolButton;
class QSlider;
class QListWidget;
class QVideoWidget;
class QLabel;
QT_END_NAMESPACE
/* 媒体信息结构体 */
struct VideoObjectInfo {
    /* 用于保存视频文件名 */
    QString fileName;
    /* 用于保存视频文件路径 */
    QString filePath;
};
class VideoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWindow(QWidget *parent = nullptr);

signals:
    void backRequested();

private:
    void playIndex(int index);
    void scanVideoFiles();
    QMediaPlayer *player;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QAudioOutput *audioOutput;
#endif
    QStringList mediaFiles;
    int currentIndex;
    QListWidget *listWidget;
    QSlider *positionSlider;
    QLabel *currentTimeLabel;
    QLabel *durationLabel;
    QToolButton *playButton;
    QSlider *volumeSlider;
    QVideoWidget *videoWidget;
    /* 媒体信息存储 */
    QVector<VideoObjectInfo> videoObjectInfo;
};

#endif // VIDEOWINDOW_H
