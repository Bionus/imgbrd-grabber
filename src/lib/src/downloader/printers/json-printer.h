#ifndef JSON_PRINTER_H
#define JSON_PRINTER_H

#include <QList>
#include <QSharedPointer>
#include "printer.h"


class Image;
class Profile;
class QJsonArray;
class QJsonObject;
class Tag;

class JsonPrinter : public Printer
{
	public:
		explicit JsonPrinter(Profile *profile);

		void print(int val) const override;
		void print(const QString &val) const override;

		void print(const Image &val) const override;
		void print(const QList<QSharedPointer<Image>> &images) const override;
		void print(const Tag &tag) const override;
		void print(const QList<Tag> &tags) const override;

	protected:
		void printArray(const QJsonArray &array) const;
		void printObject(const QJsonObject &object) const;
		QJsonObject serializeImage(const Image &image) const;

	private:
		Profile *m_profile;
};

#endif // JSON_PRINTER_H
