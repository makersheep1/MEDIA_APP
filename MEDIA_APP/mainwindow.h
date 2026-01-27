#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QStackedWidget;
class QWidget;
class QToolButton;
QT_END_NAMESPACE

class MusicWindow;
class VideoWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createHomePage();
    void showHome();

    Ui::MainWindow *ui;
    QStackedWidget *stacked;
    QWidget *homePage;
    MusicWindow *musicPage;
    VideoWindow *videoPage;
    QToolButton *musicButton;
    QToolButton *videoButton;
};
#endif // MAINWINDOW_H
