#ifndef SIMPLE_PRINTER_H
#define SIMPLE_PRINTER_H

#include <QList>
#include <QSharedPointer>
#include <QString>
#include "printer.h"


class Image;
class Profile;
class Tag;

class SimplePrinter : public Printer
{
	public:
		explicit SimplePrinter(QString tagsFormat);

		void print(int val) const override;
		void print(const QString &val) const override;

		void print(const Image &val) const override;
		void print(const QList<QSharedPointer<Image>> &images) const override;
		void print(const Tag &tag) const override;
		void print(const QList<Tag> &tags) const override;

	private:
		QString m_tagsFormat;
};

#endif // SIMPLE_PRINTER_H
