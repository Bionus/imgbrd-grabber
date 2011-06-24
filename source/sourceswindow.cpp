#include "sourceswindow.h"
#include "ui_sourceswindow.h"

sourcesWindow::sourcesWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sourcesWindow)
{
    ui->setupUi(this);
}

sourcesWindow::~sourcesWindow()
{
    delete ui;
}
