#ifndef HELPERS_H
#define HELPERS_H

#include <QString>


class QLayout;
class QWidget;


void error(QWidget *parent, const QString &message);
void showInGraphicalShell(const QString &path);
void clearLayout(QLayout *layout);

#endif // HELPERS_H
