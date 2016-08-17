#ifndef FILENAME_H
#define FILENAME_H

#include "image.h"


class Filename
{
    public:
        Filename(QString format);
        QStringList	path(Image &img, QString pth = "", int counter = 0, bool complex = true, bool maxlength = true, bool shouldFixFilename = true, bool getFull = false);
        QString getFormat();

        QString expandConditionals(QString text, QStringList tokens, QStringList tags, int depth = 0);
        QList<QPair<QString,QString>> getReplace(QString setting, QMap<QString,QStringList> details, QSettings *settings);

	private:
		QString m_format;
};

#endif // FILENAME_H
