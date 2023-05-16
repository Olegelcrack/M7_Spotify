#ifndef WINDOWHELP_H
#define WINDOWHELP_H

#include <QMainWindow>

namespace Ui {
class windowhelp;
}

class windowhelp : public QMainWindow
{
    Q_OBJECT

public:
    explicit windowhelp(QWidget *parent = nullptr);
    ~windowhelp();

private:
    Ui::windowhelp *ui;
};

#endif // WINDOWHELP_H
