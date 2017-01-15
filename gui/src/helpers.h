#ifndef HELPERS_H
#define HELPERS_H

#include <QWidget>
#include <QString>
#include <QLayout>


void error(QWidget *parent, QString message);
void showInGraphicalShell(const QString &path);
void clearLayout(QLayout *layout);

#endif // HELPERS_H
