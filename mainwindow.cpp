#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include <QSlider>
#include <random>
#include <stdlib.h>
#include <QPropertyAnimation>
#include <QLabel>
#include <QFont>

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
    ui->mes->setIcon(QIcon(":/icons/forward3.png"));
    ui->menys->setIcon(QIcon(":/icons/backward2.png"));
    ui->sequencial->setIcon(QIcon(":/icons/shuffle.png"));
    ui->bucle->setIcon(QIcon(":/icons/loop.png"));
    ui->afegirMusica->setIcon(QIcon(":/icons/plus.png"));
    QPixmap pixmap(":/icons/volume-medium.png");
    ui->volum->setPixmap(pixmap);

    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(ui->play, &QPushButton::clicked, this, &MainWindow::playAudio);
    connect(ui->afegirMusica, &QPushButton::clicked, this, &MainWindow::addAudioFiles);
    connect(ui->next, &QPushButton::clicked, this, &MainWindow::nextAudio);
    connect(ui->previous, &QPushButton::clicked, this, &MainWindow::previousAudio);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::audioAcabat);

    connect(ui->volumeSlider, &QSlider::sliderMoved, this, &MainWindow::volumen);
    connect(ui->mes, &QPushButton::clicked, this, &MainWindow::endavantAudio);
    connect(ui->menys, &QPushButton::clicked, this, &MainWindow::enrrereAudio);
    connect(ui->sequencial, &QPushButton::clicked, this, &MainWindow::sequencial);
    connect(ui->bucle, &QPushButton::clicked, this, &MainWindow::bucle);

    ui->volumeSlider->setMaximumWidth(200);
    ui->volumeSlider->setMinimumWidth(50);

    ui->positionSlider->setRange(0, 0);
    ui->positionSlider->setValue(0);
    connect(ui->positionSlider, &QSlider::sliderMoved, this, &MainWindow::setMPPosition);
    ui->canco->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QString playlistFolder = QCoreApplication::applicationDirPath() + "/../../M7_Spotify/playlist";
    QDir playlistDir(playlistFolder);
    QStringList playlist = playlistDir.entryList(QStringList() << "*.mp3" << "*.wav", QDir::Files);
    for (int i = 0; i < playlist.size(); i++) {
        QString filePath = playlistDir.filePath(playlist[i]);
        audioFiles.append(filePath);
        QFileInfo fileInfo(filePath);
        ui->playlistWidget->addItem(fileInfo.fileName());
    }

    //Status bar Labels

    leftLabel = new QLabel("HEHEHE una prova ta mare");
    //leftLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    leftLabel->setFont(QFont("Sans Serif", 11));

    rightLabel = new QLabel(QString::number(playlist.size()) + " cançons afegides");
    //rightLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    rightLabel->setFont(QFont("Sans Serif", 11));


    layout->addWidget(leftLabel);
    layout->addStretch(1);
    layout->addWidget(rightLabel);

    widget->setLayout(layout);

    loadStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::positionChanged(qint64 position)
{
    ui->positionSlider->setValue(position / 1000);
}

void MainWindow::durationChanged(qint64 duration)
{
    ui->positionSlider->setRange(0, duration / 1000);
}

void MainWindow::playAudio()
{
    if (!audioFiles.isEmpty()) {

        if(primer ==""){
            QString currentFile = audioFiles[currentAudioIndex];
            QUrl url = QUrl::fromLocalFile(currentFile);
            player->setSource(url);
            QFileInfo fileInfo(currentFile);
            QString fileName = fileInfo.fileName();
            QString fileNameWithoutExtension = fileName.left(fileName.length() - 4);
            ui->canco->setText(fileNameWithoutExtension);
            player->play();
            ui->play->setIcon(QIcon(":/icons/pause2.png"));
            primer = "cargat";
        }else{
            if (!isPlaying) {
                player->pause();
                ui->play->setIcon(QIcon(":/icons/play3.png"));
            } else {
                player->play();
                ui->play->setIcon(QIcon(":/icons/pause2.png"));

            }
            isPlaying = !isPlaying;
        }
        ui->playlistWidget->setCurrentRow(currentAudioIndex);
    }
}


void MainWindow::on_stop_clicked()
{
    player->stop();
    isPlaying = true;
    ui->play->setIcon(QIcon(":/icons/play3.png"));
}

void MainWindow::volumen(double value)
{
    audioOutput->setVolume(value/100);

    //QAudioOutput::setVolume(value);
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
    if(bucle2){
            primer = "";
            playAudio();
    }else{
        if(sequencial2){
            int randomIndex = rand() % audioFiles.count();
            while (randomIndex == currentAudioIndex || usedNumbers.contains(randomIndex)) {
                  randomIndex = rand() % audioFiles.count();
            }
            currentAudioIndex = randomIndex;
            usedNumbers.insert(currentAudioIndex);
            if (usedNumbers.count() == audioFiles.count()) {
                 usedNumbers.clear();
            }
        }else{
            currentAudioIndex = (currentAudioIndex + 1) % audioFiles.count();

        }
        QString currentFile = audioFiles[currentAudioIndex];
        QUrl url = QUrl::fromLocalFile(currentFile);
        player->setSource(url);
        QFileInfo fileInfo(currentFile);
        QString fileName = fileInfo.fileName();
        QString fileNameWithoutExtension = fileName.left(fileName.length() - 4);
        ui->canco->setText(fileNameWithoutExtension);
        player->play();
        ui->play->setIcon(QIcon(":/icons/pause2.png"));
        ui->playlistWidget->setCurrentRow(currentAudioIndex);
    }
}

void MainWindow::previousAudio()
{
    if (audioFiles.isEmpty()) {
        return;
    }
    if(bucle2){
            primer = "";
            playAudio();
    }else{
        if(sequencial2){
            qint64 newPos = player->position();
            if (newPos > 2000){
                newPos = 0;
                player->setPosition(newPos);
            }else{
                int randomIndex = rand() % audioFiles.count();
                while (randomIndex == currentAudioIndex || usedNumbers.contains(randomIndex)) {
                      randomIndex = rand() % audioFiles.count();
                }
                currentAudioIndex = randomIndex;
                usedNumbers.insert(currentAudioIndex);
                if (usedNumbers.count() == audioFiles.count()) {
                     usedNumbers.clear();
                }
            }
        }else{
            qint64 newPos = player->position();
            if (newPos > 2000){
                newPos = 0;
                player->setPosition(newPos);
            }else{

                currentAudioIndex--;
                if (currentAudioIndex < 0) {
                    currentAudioIndex = 0;
                }

            }
        }
        QString currentFile = audioFiles[currentAudioIndex];
        QUrl url = QUrl::fromLocalFile(currentFile);
        player->setSource(url);
        QFileInfo fileInfo(currentFile);
        QString fileName = fileInfo.fileName();
        QString fileNameWithoutExtension = fileName.left(fileName.length() - 4);
        ui->canco->setText(fileNameWithoutExtension);
        player->play();
        ui->play->setIcon(QIcon(":/icons/pause2.png"));
        ui->playlistWidget->setCurrentRow(currentAudioIndex);
    }
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
        layout->removeWidget(rightLabel);
        rightLabel->setText(QString::number(files.size()) + " cançons afegides");
        layout->addWidget(rightLabel);
        loadStatusBar();
    }
}

void MainWindow::audioAcabat(QMediaPlayer::MediaStatus state)
{
    if (state == QMediaPlayer::EndOfMedia) {
        nextAudio();
    }
}

void MainWindow::endavantAudio()
{
    qint64 newPos = player->position() + 5000;

    if (newPos >= player->duration()) {
            nextAudio();
        } else {
            player->setPosition(newPos);
        }

}

void MainWindow::enrrereAudio()
{
    qint64 newPos = player->position() - 5000;
    if (newPos < 0) newPos = 0;
    player->setPosition(newPos);
}

void MainWindow::sequencial() {
    sequencial2 = !sequencial2;
    if (sequencial2) {
        ui->actionAleatori->setChecked(true);
        ui->sequencial->setStyleSheet("background-color: #00FF00");
    } else {
        ui->actionAleatori->setChecked(false);
        ui->sequencial->setStyleSheet("");
    }
}

void MainWindow::bucle() {

    bucle2 = !bucle2;
    if (bucle2) {
        ui->actionBucle->setChecked(true);
         ui->bucle->setStyleSheet("background-color: #00FF00");
       } else {
        ui->actionBucle->setChecked(false);
         ui->bucle->setStyleSheet("");

      }

}


void MainWindow::on_playlistWidget_currentRowChanged(int currentRow)
{
    if (borrat && currentRow < 0) {
        currentRow += 1;
    }else if(borrat && (currentRow+1) > audioFiles.size()){
        currentRow = audioFiles.size() -1;
    }else if((borrat && currentRow > 0 && (currentRow+1) < audioFiles.size()) || (borrat && currentRow+1 == audioFiles.size() && ultim == false)){
        currentRow -=1;
    }
    borrat = false;

    if(start && audioFiles.size() > 0){

        currentAudioIndex = currentRow;
        //qDebug() << audioFiles[currentAudioIndex];
        QString currentFile = audioFiles[currentAudioIndex];
        QUrl url = QUrl::fromLocalFile(currentFile);
        player->setSource(url);
        QFileInfo fileInfo(currentFile);
        QString fileName = fileInfo.fileName();
        QString fileNameWithoutExtension = fileName.left(fileName.length() - 4);
        ui->canco->setText(fileNameWithoutExtension);
        isPlaying = false;
        player->play();
        primer = "cargat";
        ui->play->setIcon(QIcon(":/icons/pause2.png"));
        if(currentRow+1 == audioFiles.size()){
            ultim = true;
        }else{
            ultim = false;
        }

     }else{
        start = true;
        player->stop();
        ui->canco->clear();
        ui->play->setIcon(QIcon(":/icons/play3.png"));
        currentAudioIndex = currentRow;
    }

}


void MainWindow::on_playlistWidget_itemClicked(QListWidgetItem *item)
{
    on_playlistWidget_currentRowChanged(currentAudioIndex);

}

void MainWindow::on_remove_clicked()
{
    if(audioFiles.size() > 0){
        num_files = audioFiles.size();
        audioFiles.remove(currentAudioIndex);
        borrat = true;
        ui->playlistWidget->takeItem(currentAudioIndex);
        layout->removeWidget(rightLabel);
        rightLabel->setText(QString::number(audioFiles.size()) + " cançons afegides");
        layout->addWidget(rightLabel);
        loadStatusBar();
    }else{

    }

}

void MainWindow::on_actionBucle_triggered(){
    bucle();
}

void MainWindow::on_actionAleatori_triggered(){
    sequencial();
}

void MainWindow::on_actionSortir_triggered(){
    QApplication::exit();
}

void MainWindow::on_actionInsertar_Can_triggered(){
    addAudioFiles();
}
void MainWindow::loadStatusBar()
{
    //ui->statusbar->removeWidget(widget);
    ui->statusbar->addWidget(widget, 1);
}



