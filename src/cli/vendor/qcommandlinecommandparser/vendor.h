#ifndef VENDOR_H
#define VENDOR_H

#include <QCommandLineOption>
#include <QString>


enum MessageType { UsageMessage, ErrorMessage };

void showParserMessage(const QString &message, MessageType type);
QString wrapText(const QString &names, int optionNameMaxWidth, const QString &description);
QString optionName(const QCommandLineOption &option);

#endif // VENDOR_H
