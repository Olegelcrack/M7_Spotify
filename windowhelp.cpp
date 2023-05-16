#include "windowhelp.h"
#include "ui_windowhelp.h"

windowhelp::windowhelp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::windowhelp)
{
    ui->setupUi(this);
}

windowhelp::~windowhelp()
{
    delete ui;
}
