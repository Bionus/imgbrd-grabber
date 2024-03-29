#ifndef PRINTER_H
#define PRINTER_H

#include <QList>
#include <QSharedPointer>


class Image;
class Site;
class Tag;

class Printer
{
	public:
		virtual void print(int val) const = 0;
		virtual void print(const QString &val) const = 0;

		virtual void print(const Image &image) const = 0;
		virtual void print(const QList<QSharedPointer<Image>> &images) const = 0;
		virtual void print(const Tag &tag, Site *site) const = 0;
		virtual void print(const QList<Tag> &tags, Site *site) const = 0;
};

#endif // PRINTER_H
