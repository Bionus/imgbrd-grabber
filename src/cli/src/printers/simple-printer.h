#ifndef SIMPLE_PRINTER_H
#define SIMPLE_PRINTER_H

#include <QList>
#include <QSharedPointer>
#include <QString>
#include "printer.h"


class Image;
class Profile;
class QTextStream;
class Site;
class Tag;

class SimplePrinter : public Printer
{
	public:
		explicit SimplePrinter(QTextStream *stream, QString tagsFormat);

		void print(int val) const override;
		void print(const QString &val) const override;

		void print(const Image &image) const override;
		void print(const QList<QSharedPointer<Image>> &images) const override;
		void print(const Tag &tag, Site *site) const override;
		void print(const QList<Tag> &tags, Site *site) const override;

	private:
		QTextStream *m_stream;
		QString m_tagsFormat;
};

#endif // SIMPLE_PRINTER_H
