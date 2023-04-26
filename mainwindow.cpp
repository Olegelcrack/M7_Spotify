#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include <QDebug>
#include <QSlider>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    player = new QMediaPlayer();
    audioOutput = new QAudioOutput();
    player->setAudioOutput(audioOutput);
    ui->play->setIcon(QIcon(":/icons/play3.png"));
    ui->next->setIcon(QIcon(":/icons/next2.png"));
    ui->previous->setIcon(QIcon(":/icons/previous2.png"));
    ui->stop->setIcon(QIcon(":/icons/stop2.png"));
    QPixmap pixmap(":/icons/volume-medium.png");
    ui->volum->setPixmap(pixmap);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(ui->play, &QPushButton::clicked, this, &MainWindow::playAudio);
    connect(ui->afegirMusica, &QPushButton::clicked, this, &MainWindow::addAudioFiles);
    connect(ui->next, &QPushButton::clicked, this, &MainWindow::nextAudio);
    connect(ui->previous, &QPushButton::clicked, this, &MainWindow::previousAudio);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::audioAcabat);

    positionSlider = new QSlider(Qt::Horizontal, this);
    positionSlider->setRange(0, 0); // inicialización con valor cero
    positionSlider->setValue(0);
    connect(positionSlider, &QSlider::sliderMoved, this, &MainWindow::setMPPosition);
    ui->statusbar->addPermanentWidget(positionSlider);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::positionChanged(qint64 position)
{
    positionSlider->setValue(position / 1000);
}

void MainWindow::durationChanged(qint64 duration)

{
    positionSlider->setRange(0, duration / 1000);
}

void MainWindow::playAudio()
{
    if (!audioFiles.isEmpty()) {

        if(primer ==""){
            QString currentFile = audioFiles[currentAudioIndex];
            QUrl url = QUrl::fromLocalFile(currentFile);
            player->setSource(url);
            ui->statusbar->showMessage(currentFile);
            player->play();
            ui->play->setIcon(QIcon(":/icons/pause2.png"));
            isPlaying = !isPlaying;
            primer = "cargat";
        }else{
            if (isPlaying) {
                player->pause();
                ui->play->setIcon(QIcon(":/icons/play3.png"));
            } else { // Establece la posición de reproducción almacenada
                player->play();
                ui->play->setIcon(QIcon(":/icons/pause2.png"));

            }
            isPlaying = !isPlaying;
        }
    }
}


void MainWindow::on_stop_clicked()
{
    player->stop();
}

void MainWindow::on_volumeSlider_valueChanged(int value)
{
    audioOutput->setVolume(value);
    if(value == 0){
        QPixmap pixmap(":/icons/volume-mute.png");
        ui->volum->setPixmap(pixmap);
    }else if(value < 25){
        QPixmap pixmap(":/icons/volume-low.png");
        ui->volum->setPixmap(pixmap);
    }else if(value > 25 && value < 75){
        QPixmap pixmap(":/icons/volume-medium.png");
        ui->volum->setPixmap(pixmap);
    }else if(value > 75){
        QPixmap pixmap(":/icons/volume-high.png");
        ui->volum->setPixmap(pixmap);
    }
}

void MainWindow::setMPPosition(int position)
{
    player->setPosition(position * 1000);
}


void MainWindow::nextAudio()
{
    if (audioFiles.isEmpty()) {
        return;
    }
    currentAudioIndex = (currentAudioIndex + 1) % audioFiles.count();
    QString currentFile = audioFiles[currentAudioIndex];
    QUrl url = QUrl::fromLocalFile(currentFile);
    player->setSource(url);
    ui->statusbar->showMessage(currentFile);
    player->play();
}

void MainWindow::previousAudio()
{
    if (audioFiles.isEmpty()) {
        return;
    }
    currentAudioIndex--;
    if (currentAudioIndex < 0) {
        currentAudioIndex = audioFiles.count() - 1;
    }
    QString currentFile = audioFiles[currentAudioIndex];
    QUrl url = QUrl::fromLocalFile(currentFile);
    player->setSource(url);
    ui->statusbar->showMessage(currentFile);
    player->play();
}


void MainWindow::addAudioFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Audio Files"), QDir::homePath(), tr("Audio Files (*.mp3 *.mp4)"));
    if (!files.isEmpty()) {
        foreach (QString filePath, files) {
            if (!audioFiles.contains(filePath)) {
                audioFiles.append(filePath);
                QFileInfo fileInfo(filePath);
                ui->playlistWidget->addItem(fileInfo.fileName());
            }
        }
    }
}

void MainWindow::audioAcabat(QMediaPlayer::MediaStatus state)
{
    if (state == QMediaPlayer::EndOfMedia) {
        nextAudio();
    }
}


