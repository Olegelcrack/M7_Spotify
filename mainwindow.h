#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSlider>
#include <QListWidget>
#include <QPushButton>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void volumen(int value);
    void on_stop_clicked();
    void setMPPosition(int position);

    void addAudioFiles();
    void previousAudio();
    void nextAudio();

    void playAudio();
    void audioAcabat(QMediaPlayer::MediaStatus state);
    void endavantAudio();
    void enrrereAudio();
    void sequencial();
    void bucle();
    //void on_positionSlider_sliderMoved(int position);

    void on_playlistWidget_currentRowChanged(int currentRow);

    void on_playlistWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QListWidget *playlistWidget;
    QList<QString> audioFiles;
    QUrl url;
    QPushButton *afegirMusica;
    int currentAudioIndex = 0;
    bool isPlaying = false;
    QString primer = "";
    bool sequencial2 = false;
    bool bucle2 = false;
    QSet<int> usedNumbers;
    bool start = false;

};
#endif // MAINWINDOW_H
