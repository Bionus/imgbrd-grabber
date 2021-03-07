#include "json-printer.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QSharedPointer>
#include <QTextStream>
#include "logger.h"
#include "models/image.h"
#include "models/profile.h"
#include "tags/tag.h"


JsonPrinter::JsonPrinter(Profile *profile)
	: m_profile(profile)
{}


void JsonPrinter::print(int val) const
{
	print(QString::number(val));
}

void JsonPrinter::print(const QString &val) const
{
	QTextStream(stdout) << qPrintable(val);
}


void JsonPrinter::print(const Image &image) const
{
	const QJsonObject jsonImage = serializeImage(image);
	printObject(jsonImage);
}

void JsonPrinter::print(const QList<QSharedPointer<Image>> &images) const
{
	QJsonArray jsonArray;
	for (const QSharedPointer<Image> &image : images) {
		jsonArray.append(serializeImage(*image.data()));
	}
	printArray(jsonArray);
}

void JsonPrinter::print(const Tag &tag) const
{
	QJsonObject jsonTag;
	tag.write(jsonTag);
	printObject(jsonTag);
}

void JsonPrinter::print(const QList<Tag> &tags) const
{
	QJsonArray jsonArray;
	for (const Tag &tag : tags) {
		QJsonObject jsonObj;
		tag.write(jsonObj);
		jsonArray.append(jsonObj);
	}
	printArray(jsonArray);
}


void JsonPrinter::printArray(const QJsonArray &array) const
{
	QJsonDocument jsonDoc;
	jsonDoc.setArray(array);

	const QByteArray jsonResult = jsonDoc.toJson(QJsonDocument::Indented);
	QTextStream(stdout) << qPrintable(jsonResult);
}

void JsonPrinter::printObject(const QJsonObject &object) const
{
	QJsonDocument jsonDoc;
	jsonDoc.setObject(object);

	const QByteArray jsonResult = jsonDoc.toJson(QJsonDocument::Indented);
	QTextStream(stdout) << qPrintable(jsonResult);
}


QJsonObject JsonPrinter::serializeImage(const Image &image) const
{
	static const QStringList ignoreKeys = {"all", "allo", "allos", "all_namespaces", };

	QJsonObject jsObject;

	const auto tokens = image.tokens(m_profile);
	for (auto& key : tokens.keys()) {
		typedef QVariant::Type Type;
		if (ignoreKeys.contains(key)) {
			continue;
		}
		if (key.contains("search_")) {
			continue;
		}

		const QVariant& qvalue = tokens.value(key).value();
		auto type = qvalue.type();

		if (type == QVariant::Type::StringList) {
			QStringList l = qvalue.toStringList();
			if (l.isEmpty()) {
				continue;
			}
			jsObject.insert(key, QJsonArray::fromStringList(l));
		} else if (type == QVariant::Type::String) {
			QString s = qvalue.toString();
			if (s.isEmpty()) {
				continue;
			}
			jsObject.insert(key, s);
		} else if (type == Type::Url || type == Type::ULongLong || type == Type::LongLong) {
			jsObject.insert(key, qvalue.toString());
		} else if (type == Type::Int) {
			jsObject.insert(key, qvalue.value<int>());
		} else if (type == Type::Bool) {
			jsObject.insert(key, qvalue.value<bool>());
		} else if (type == Type::DateTime) {
			jsObject.insert(key, static_cast<int>(qvalue.value<QDateTime>().toTime_t()));
		} else {
			log(QStringLiteral("using generic QVariant::toString for key: %1").arg(key), Logger::Warning);
			jsObject.insert(key, qvalue.toString());
		}
	}
	jsObject.insert("isVideo", image.isVideo());
	jsObject.insert("isGallery", image.isGallery());
	jsObject.insert("isAnimated", image.isAnimated());
	return jsObject;
}
