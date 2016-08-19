#ifndef FILENAME_H
#define FILENAME_H

#include "image.h"


class Filename
{
    public:
        Filename(QString format);
        QStringList	path(Image &img, QSettings *settings, QString pth = "", int counter = 0, bool complex = true, bool maxlength = true, bool shouldFixFilename = true, bool getFull = false);
        QString getFormat();

        QList<QMap<QString, QPair<QString, QString>>> getReplaces(QString filename, Image &img, QSettings *settings, QMap<QString,QStringList> custom);
        QString getPairValue(QPair<QString, QString> pair);
        QString expandConditionals(QString text, QStringList tokens, QStringList tags, QMap<QString, QPair<QString, QString>> replaces, int depth = 0);
        QList<QPair<QString,QString>> getReplace(QString setting, QMap<QString,QStringList> details, QSettings *settings);
        QMap<QString, QStringList> makeDetails(Image &img, QSettings *settings);

	private:
		QString m_format;
};

#endif // FILENAME_H
