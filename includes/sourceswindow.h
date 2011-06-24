#ifndef SOURCESWINDOW_H
#define SOURCESWINDOW_H

#include <QDialog>

namespace Ui {
    class sourcesWindow;
}

class sourcesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit sourcesWindow(QWidget *parent = 0);
    ~sourcesWindow();

private:
    Ui::sourcesWindow *ui;
};

#endif // SOURCESWINDOW_H
