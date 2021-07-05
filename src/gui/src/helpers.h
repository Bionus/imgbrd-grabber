#ifndef HELPERS_H
#define HELPERS_H

#include <QString>


class QDialog;
class QLayout;
class QSettings;
class QWidget;


void error(QWidget *parent, const QString &message);
void showInGraphicalShell(const QString &path);
void clearLayout(QLayout *layout);
void setupDialogShortcuts(QDialog *dialog, QSettings *settings);

#endif // HELPERS_H
