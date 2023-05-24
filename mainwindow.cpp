#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "windowhelp.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include <QSlider>
#include <QTimer>
#include <random>
#include <stdlib.h>
#include <QPropertyAnimation>
#include <QLabel>
#include <QFont>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    player = new QMediaPlayer();
    audioOutput = new QAudioOutput();
    player->setAudioOutput(audioOutput);

    //  Shortcuts per el teclat

    QShortcut *scSpace = new QShortcut(QKeySequence(Qt::Key_Space), this);
    QShortcut *scForward = new QShortcut(QKeySequence(Qt::Key_Right), this);
    QShortcut *scBackward = new QShortcut(QKeySequence(Qt::Key_Left), this);
    QShortcut *scNext = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Right), this);
    QShortcut *scPrevious = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Left), this);
    QShortcut *scVolumeDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
    QShortcut *scVolumeUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
    QShortcut *scRemove = new QShortcut(QKeySequence(Qt::Key_Delete), this);

    //  Connects els cuals creem per definir que fa cada botó, shortcut o algo que faci el player
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::updatePositionLabel);
    connect(ui->play, &QPushButton::clicked, this, &MainWindow::playAudio);
    connect(ui->afegirMusica, &QPushButton::clicked, this, &MainWindow::addAudioFiles);
    connect(ui->next, &QPushButton::clicked, this, &MainWindow::nextAudio);
    connect(ui->previous, &QPushButton::clicked, this, &MainWindow::previousAudio);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::audioAcabat);

    connect(ui->volumeSlider, &QSlider::valueChanged, this, &MainWindow::volumen);
    connect(ui->mes, &QPushButton::clicked, this, &MainWindow::endavantAudio);
    connect(ui->menys, &QPushButton::clicked, this, &MainWindow::enrrereAudio);
    connect(ui->sequencial, &QPushButton::clicked, this, &MainWindow::sequencial);
    connect(ui->bucle, &QPushButton::clicked, this, &MainWindow::bucle);
    connect(ui->remove, &QPushButton::clicked, this, &MainWindow::remove);
    connect(scSpace, &QShortcut::activated, this, &MainWindow::playAudio);
    connect(scForward, &QShortcut::activated, this, &MainWindow::endavantAudio);
    connect(scBackward, &QShortcut::activated, this, &MainWindow::enrrereAudio);
    connect(scNext, &QShortcut::activated, this, &MainWindow::nextAudio);
    connect(scPrevious, &QShortcut::activated, this, &MainWindow::previousAudio);
    QSlider *vol = ui->volumeSlider;
    connect(scVolumeDown, &QShortcut::activated, this, [vol]() {
        int currentValue = vol->value();
        vol->setValue(currentValue - 10);
    });

    connect(scVolumeUp, &QShortcut::activated, this, [vol]() {
        int currentValue = vol->value();
        vol->setValue(currentValue + 10);
    });
    connect(scRemove, &QShortcut::activated, this, &MainWindow::remove);


    ui->volumeSlider->setMaximumWidth(200);
    ui->volumeSlider->setMinimumWidth(50);

    ui->positionSlider->setRange(0, 0);
    ui->positionSlider->setValue(0);
    connect(ui->positionSlider, &QSlider::sliderMoved, this, &MainWindow::setMPPosition);
    ui->canco->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    playlistFilePath = QCoreApplication::applicationDirPath() + "/playlist.txt";
    loadPlaylist();

    //Status bar Labels
    if(audioFiles.size() > 0){
        leftLabel = new QLabel("");
    }else{
        leftLabel = new QLabel("No hi ha cap cançó afegida");
    }

    leftLabel->setFont(QFont("Sans Serif", 11));

    rightLabel = new QLabel(QString::number(audioFiles.size()) + " cançons afegides");
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

//Funció que utilitzem per canviar la posició de la cançó amb el slider
void MainWindow::positionChanged(qint64 position)
{
    ui->positionSlider->setValue(position / 1000);
}

void MainWindow::durationChanged(qint64 duration)
{
    ui->positionSlider->setRange(0, duration / 1000);
}

//Funció per pausar/reproduir la cançó
void MainWindow::playAudio()
{
    if (!audioFiles.isEmpty()) {

        if(primer ==false){ //La primera cançó que reproduim fara aquestà condició
            start = true;
            QString currentFile = audioFiles[currentAudioIndex];
            QUrl url = QUrl::fromLocalFile(currentFile);
            player->setSource(url);
            QFileInfo fileInfo(currentFile);
            QString fileName = fileInfo.fileName();
            QString fileNameWithoutExtension = fileName.left(fileName.length() - 4);
            ui->canco->setText(fileNameWithoutExtension);
            player->play();
            ui->play->setIcon(QIcon(":/icons/pause2.png"));
            primer = true;
            leftLabel->setText("Reproduint Cançó");
            isPlaying = true;
        }else{
            if (isPlaying) { //Comprobem que s'estigui reproduint, si s'està reproduint i fem click es posarà en pausa i sinó es reproduirà
                player->pause();
                ui->play->setIcon(QIcon(":/icons/play3.png"));
                leftLabel->setText("Cançó Pausada");
            } else {
                player->play();
                ui->play->setIcon(QIcon(":/icons/pause2.png"));
                leftLabel->setText("Reproduint Cançó");

            }
            isPlaying = !isPlaying; //Posem el boolean de isPlaying al contrari del que estaba
        }
        Notificacio();
        ui->playlistWidget->setCurrentRow(currentAudioIndex);
    }
}

//Funció per parar el reproductor
void MainWindow::on_stop_clicked()
{
    player->stop();
    isPlaying = false;
    ui->play->setIcon(QIcon(":/icons/play3.png"));
}

//Funció per definir el volumen del reproductor
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

//Aquesta funció es la que utilitzem per a que aparegui els minuts i segons actuals de la cançó
void MainWindow::updatePositionLabel(qint64 position)
{
    int minutes = position / 60000;
    int seconds = (position % 60000) / 1000;
    QString positionString = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
    ui->duracio->setText(positionString);

    qint64 duraciototal = player->duration();
    int minutstotals = duraciototal / 60000;
    int segonstotals = (duraciototal % 60000) / 1000;
    QString duraciototalstring = QString("%1:%2").arg(minutstotals, 2, 10, QChar('0')).arg(segonstotals, 2, 10, QChar('0'));
    ui->duraciototal->setText(duraciototalstring);

}

//Funció que utilitzem per pasar de cançó
void MainWindow::nextAudio()
{
    if (audioFiles.isEmpty()) { //Si no hi han cançons no fem res
        return;
    }
    EstatCanco();
    if(bucle2){ //Comprobem si esta el bucle activat
            primer = false;
            playAudio(); //Si està activat es reproduirà la mateixa cançó tot el rato
            leftLabel->setText(textActual.append(" - Repetint Cançó"));
    }else{
        if(sequencial2){ //Comprobem si esta el sequencial activat
            int randomIndex = rand() % audioFiles.count(); //Posem una cançó random mirant que no hagi sortit abans ja creant un randomIndex
            while (randomIndex == currentAudioIndex || usedNumbers.contains(randomIndex)) {
                  randomIndex = rand() % audioFiles.count();
            }
            currentAudioIndex = randomIndex;
            usedNumbers.insert(currentAudioIndex); //Anem afegint les cançons que van sortit
            if (usedNumbers.count() == audioFiles.count()) { //Quan ja han sortit totes, netejem el usedNumbers i ja poden tornar a sortir totes
                 usedNumbers.clear();
            }
            leftLabel->setText(textActual.append(" - Cançó Aleatoria"));
        }else{

            if(continua2 == false && currentAudioIndex+1 == audioFiles.count()){ //Comprobem que el continua no estigui activat, si està activat reproduim la seguent
                leftLabel->setText(textActual.append(" - Repetint Cançó")); //Si el continua no està activat la última cançó serà la última i no pararà de reproduir-se
            }else{
                currentAudioIndex = (currentAudioIndex + 1) % audioFiles.count();
                leftLabel->setText(textActual.append(" - Següent Cançó"));
            }



        }
        QString currentFile = audioFiles[currentAudioIndex];
        QUrl url = QUrl::fromLocalFile(currentFile);
        player->setSource(url);
        QFileInfo fileInfo(currentFile);
        QString fileName = fileInfo.fileName();
        QString fileNameWithoutExtension = fileName.left(fileName.length() - 4);
        ui->canco->setText(fileNameWithoutExtension);
        player->play(); //Reproduim la cançó depenent del currentAudioIndex que haguem obtingut en les condicions
        ui->play->setIcon(QIcon(":/icons/pause2.png"));
        ui->playlistWidget->setCurrentRow(currentAudioIndex);


        Notificacio();
    }
}

//Funció que utilitzem per anar a l'anterior Cançó
void MainWindow::previousAudio()
{
    if (audioFiles.isEmpty()) { //Si no hi han cançons no fem res
        return;
    }
    EstatCanco();
    if(bucle2){ //Comprobem si esta el bucle activat
            primer = false;
            playAudio(); //Si està activat es reproduirà la mateixa cançó tot el rato
            leftLabel->setText(textActual.append(" - Repetint Cançó"));
    }else{
        if(sequencial2){ //Comprobem si esta el sequencial activat
            qint64 newPos = player->position();
            if (newPos > 2000){ //Comprobem si la cançó porta més de dos segons reproduint-se, si es aixi començem de nou, sino anem a l'anterior cançó
                newPos = 0;
                player->setPosition(newPos);
                leftLabel->setText(textActual.append(" - Començant cançó de nou"));
            }else{ //Posem una cançó random mirant que no hagi sortit abans ja
                int randomIndex = rand() % audioFiles.count(); //Creem un randomIndex per obtenir una cançó random
                while (randomIndex == currentAudioIndex || usedNumbers.contains(randomIndex)) {
                      randomIndex = rand() % audioFiles.count();
                }
                currentAudioIndex = randomIndex;
                usedNumbers.insert(currentAudioIndex); //Anem afegint les cançons que van sortit
                if (usedNumbers.count() == audioFiles.count()) { //Quan ja han sortit totes, netejem el usedNumbers i ja poden tornar a sortir totes
                     usedNumbers.clear();
                }
                leftLabel->setText(textActual.append(" - Cançó Aleatoria"));
            }
        }else{ //En cas de no ser ni sequencial ni bucle anem a l'anterior cançó i ja està
            qint64 newPos = player->position();
            if (newPos > 2000){
                newPos = 0;
                player->setPosition(newPos);
                leftLabel->setText(textActual.append(" - Començant cançó de nou"));
            }else{

                currentAudioIndex--;

                if (currentAudioIndex < 0) { //En cas de ser la primera cançó hi vols tirar enrrere no pots i començes de nou amb la primera
                    currentAudioIndex = 0;
                    leftLabel->setText(textActual.append(" - Començant cançó de nou"));
                }else{
                    leftLabel->setText(textActual.append(" - Cançó Anterior"));
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
        player->play(); //Reproduim la cançó depenent del currentAudioIndex que haguem obtingut en les condicions
        ui->play->setIcon(QIcon(":/icons/pause2.png"));
        ui->playlistWidget->setCurrentRow(currentAudioIndex);

        Notificacio();
    }
}

//Funció que utilitzem per afegir cançons al reproductor
void MainWindow::addAudioFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Audio Files"), QDir::homePath(), tr("Audio Files (*.mp3 *.mp4)")); //Obrim el explorador d'arxius
    EstatCanco();
    if (!files.isEmpty()) { //Comprobem si s'ha seleccionat algun arxiu
        foreach (QString filePath, files) { //Afegim tots els arxius que s'han seleccionat amb un bucle
            if (!audioFiles.contains(filePath)) {
                audioFiles.append(filePath);
                QFileInfo fileInfo(filePath);
                ui->playlistWidget->addItem(fileInfo.fileName());
            }
        }
        layout->removeWidget(rightLabel);
        rightLabel->setText(QString::number(audioFiles.size()) + " cançons afegides");
        layout->addWidget(rightLabel);
        loadStatusBar(); //Actualitzem la statusBar
        leftLabel->setText("Cançó Afegida");
        savePlaylist();
    }else{
        leftLabel->setText(textActual.append(" - Cap Cançó Afegida")); //En cas de no afegir cap cançó apareixerà això al statusBar
    }


    Notificacio();
}

//Funció que es crida en pic ha acabat un audio i comença automaticament pel seguent cridant a la fucnió nextAudio()
void MainWindow::audioAcabat(QMediaPlayer::MediaStatus state)
{
    if (state == QMediaPlayer::EndOfMedia) {
        nextAudio();
    }
}

void MainWindow::endavantAudio()
{
    qint64 newPos = player->position() + 5000; //Li sumem 5 segons a la posició actual del reproductor

    if (newPos >= player->duration()) {
            nextAudio(); //Si la posició nova és més gran que el audio pasem a la següent cançó
        } else {
            player->setPosition(newPos);
            EstatCanco();
            leftLabel->setText(textActual.append(" - Adelantant Cançó 5 segons"));
            Notificacio();
        }


}

//Funció que cridem per tirar enrrere l'audio 5 segons
void MainWindow::enrrereAudio()
{
    qint64 newPos = player->position() - 5000; //Li restem 5 segons a la posició actual del reproductor
    if (newPos < 0) newPos = 0; //Si es -0 fem que la cançó començi pel principi
    player->setPosition(newPos);
    EstatCanco();
    leftLabel->setText(textActual.append(" - Endarrerint Cançó 5 segons"));
    Notificacio();
}

//Funció que cridem per fer el sequencial i continua
void MainWindow::sequencial() {
    EstatCanco();
    if (sequencial2 == false && continua2==false) {//Comprobem que no hi hagi cap dels dos activats amb dos booleans
        sequencial2 = true; //Si els dos estan desactivats activem el sequencial
        ui->actionAleatori->setChecked(true);
        ui->sequencial->setStyleSheet("background-color: #00FF00");
        leftLabel->setText(textActual.append(" - Aleatori Activat"));

    } else {
        if(continua2==false){ //Si el sequencial està activat i tornem a fer click activem el continua
            sequencial2 = false; //Posem el sequencial a false
            ui->sequencial->setIcon(QIcon(":/icons/sync_alt.png"));
            continua2 = true; //I el continua a true
            ui->sequencial->setStyleSheet("background-color: #00FFFF");
            leftLabel->setText(textActual.append(" - Continu Activat"));
        }else{
            continua2 = false; //Si el continua esta activat el seguent cop que clickem el desactivarem i començarem la condició pel principi un altre cop
            ui->sequencial->setIcon(QIcon(":/icons/shuffle.png"));
            ui->actionAleatori->setChecked(false);
            ui->sequencial->setStyleSheet("");
            leftLabel->setText(textActual.append(" - Sequencial Activat"));
        }

    }
   Notificacio();


    //QTimer::singleShot(2000, this, SLOT(changeLeftLabelText()));

}

//Funció que utilitzem pel bucle
void MainWindow::bucle() {

    bucle2 = !bucle2;
    EstatCanco();
    if (bucle2) { //Comprobem si es true o false
        ui->actionBucle->setChecked(true); //El posem checked i el pintem de color
         ui->bucle->setStyleSheet("background-color: #00FF00");
         leftLabel->setText(textActual.append(" - Bucle Activat"));
       } else {
        ui->actionBucle->setChecked(false); //Li treiem el checked i el color
         ui->bucle->setStyleSheet("");
         leftLabel->setText(textActual.append(" - Bucle Desactivat"));

      }
    Notificacio();
    //QTimer::singleShot(2000, this, SLOT(changeLeftLabelText()));

}



//Funció la cual reprodueix una cançó de la llista quan fem clic a sobre
void MainWindow::on_playlistWidget_currentRowChanged(int currentRow)
{
    if (borrat && currentRow < 0) { //Condicions en cas de eliminar la primera cançó, la última o la antepenultima, per evitar que elimini quina no és
        currentRow += 1;
    }else if(borrat && (currentRow+1) > audioFiles.size()){
        currentRow = audioFiles.size() -1;
    }else if((borrat && currentRow > 0 && (currentRow+1) < audioFiles.size()) || (borrat && currentRow+1 == audioFiles.size() && ultim == false)){
        currentRow -=1;
    }
    borrat = false;

    if(start && audioFiles.size() > 0){ //Per evitar que començi just al iniciar el programa creem boolean start i comprobem que hi hagin cançons afegides

        currentAudioIndex = currentRow;
        QString currentFile = audioFiles[currentAudioIndex];
        QUrl url = QUrl::fromLocalFile(currentFile);
        player->setSource(url); //carguem la cançó seleccionada
        QFileInfo fileInfo(currentFile);
        QString fileName = fileInfo.fileName();
        QString fileNameWithoutExtension = fileName.left(fileName.length() - 4);
        ui->canco->setText(fileNameWithoutExtension);
        isPlaying = true;
        player->play(); //Reproduim la cançó
        primer = true;
        ui->play->setIcon(QIcon(":/icons/pause2.png"));
        if(currentRow+1 == audioFiles.size()){ //Condició per saber si el que s'ha seleccionat és l'últim o no
            ultim = true;
        }else{
            ultim = false;
        }

     }else{
        isPlaying = false;
        start = true;
        player->stop();
        ui->canco->clear();
        ui->play->setIcon(QIcon(":/icons/play3.png"));
        currentAudioIndex = currentRow;

    }

}

//Funció que crida a la funció de currentRowChanged, la cual selecciona una cançó de la llista i la reprodueix
void MainWindow::on_playlistWidget_itemClicked()
{
    on_playlistWidget_currentRowChanged(currentAudioIndex);
    changeLeftLabelText();

}

//Funció per eliminar una cançó
void MainWindow::remove()
{
    if(audioFiles.size() > 0){ //Comprobem que hi hagin cançons introduides
        num_files = audioFiles.size();
        audioFiles.remove(currentAudioIndex); //Borrem la cançó del QList AudioFiles
        borrat = true;
        ui->playlistWidget->takeItem(currentAudioIndex); //El borrem de la llista que mostrem
        layout->removeWidget(rightLabel);
        rightLabel->setText(QString::number(audioFiles.size()) + " cançons afegides"); //Actualitzem el statusBar amb les cançons que queden
        layout->addWidget(rightLabel);
        loadStatusBar();
        EstatCanco();
        leftLabel->setText(textActual.append(" - Cançó Eliminada")); //Afegim informació al statusBar de que la cançó s'ha eliminat
        Notificacio();
        savePlaylist(); //Guardem les cançons que queden al txt
    }else{

    }

}

//Funcions per a la barra d'eines
void MainWindow::on_actionBucle_triggered(){
    bucle();
}

void MainWindow::on_actionAleatori_triggered(){
    sequencial2 = false;
    continua2 = false;
    sequencial();
}

void MainWindow::on_actionSortir_triggered(){
    QApplication::exit();
}

void MainWindow::on_actionInsertar_Can_triggered(){
    addAudioFiles();
}

void MainWindow::on_actionContinua_triggered(){

    sequencial2 = true;
    sequencial();
}

void MainWindow::on_actionAjuda_triggered(){
    windowhelp *helpWindow = new windowhelp(this);
    helpWindow->show();
}

//Cridem aquesta funció per afegir el statusBar
void MainWindow::loadStatusBar()
{
    //ui->statusbar->removeWidget(widget);
    ui->statusbar->addWidget(widget, 1);
}

//En aquesta funció canviem el text del label de l'esquerra
void MainWindow::changeLeftLabelText()
{
    if(audioFiles.size() > 0){
        if(player->source().isEmpty()){  //comprobem si hi ha una cançó seleccionada
                newText = "";
        }else{
            if(!isPlaying){ //comproblem si està pausada o s'està reproduint

                newText = "Cançó Pausada";
            }else{

                newText = "Reproduint Cançó";

            }
        }
    }else{
        newText = "No hi ha cap cançó afegida";
    }
    leftLabel->setText(newText);
    loadStatusBar();
}

//Funció per obtindre l'estat de la cançó

void MainWindow::EstatCanco(){
    if(audioFiles.size() > 0){

        if(player->source().isEmpty()){ //comprobem si hi ha una cançó seleccionada

                textActual = "";
        }else{
            if(!isPlaying){ //comproblem si està pausada o s'està reproduint

                textActual = "Cançó Pausada";
            }else{
                textActual = "Reproduint Cançó";

            }
        }

    }else{
        textActual = "No hi ha cap cançó afegida";
    }

}

//Funció la cual cridem per afegir contingut al label de l'esquerra i el borrem al cap de dos segons
void MainWindow::Notificacio()
{


    if (sequencialTimer) { //En cas d'existir timer el parem, i l'eliminem per poder crear un nou (Això esta fet per a que si algu bugejar el statusbar no podrà
        sequencialTimer->stop();
        delete sequencialTimer;
        sequencialTimer = nullptr;
    } else {

    }
    sequencialTimer = new QTimer(this); //creem un timer
    connect(sequencialTimer, SIGNAL(timeout()), this, SLOT(changeLeftLabelText())); //Li asignem a quina funció ha de cridar
    sequencialTimer->start(2000);
}


//Funció per guardar la playlist al fitxer .txt
void MainWindow::savePlaylist()
{
    QFile file(playlistFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        for (const QString& filePath : audioFiles) {
            stream << filePath << "\n";
        }
        file.close();
    } else {
        // Error al abrir el archivo
    }
}

//Funció per cargar les cançons guardades al .txt
void MainWindow::loadPlaylist()
{
    QFile file(playlistFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString filePath = stream.readLine();
            if (!filePath.isEmpty() && !audioFiles.contains(filePath)) { //Comprobem si hi han cançons al fitxer i les afegim al QList audioFiles
                audioFiles.append(filePath);
                QFileInfo fileInfo(filePath);
                ui->playlistWidget->addItem(fileInfo.fileName()); //Afegim la cançó a la llista que mostrem
            }
        }
        file.close();

    } else {
        // Error al abrir el archivo
    }
}
