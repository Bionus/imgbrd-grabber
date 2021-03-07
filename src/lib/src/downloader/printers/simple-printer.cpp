#include "simple-printer.h"
#include <QList>
#include <QSharedPointer>
#include <QString>
#include <iostream>
#include <utility>
#include "logger.h"
#include "models/image.h"
#include "models/profile.h"
#include "tags/tag.h"


SimplePrinter::SimplePrinter(QString tagsFormat)
	: m_tagsFormat(std::move(tagsFormat))
{}


void SimplePrinter::print(int val) const
{
	std::cout << val << std::endl;
}

void SimplePrinter::print(const QString &val) const
{
	std::cout << val.toStdString() << std::endl;
}


void SimplePrinter::print(const Image &image) const
{
	print(image.url().toString());
}

void SimplePrinter::print(const QList<QSharedPointer<Image>> &images) const
{
	for (const QSharedPointer<Image> &image : images) {
		print(*image.data());
	}
}

void SimplePrinter::print(const Tag &tag) const
{
	QString ret = m_tagsFormat;
	ret.replace("\\t", "\t");
	ret.replace("\\n", "\n");
	ret.replace("\\r", "\r");
	ret.replace("%tag", tag.text());
	ret.replace("%count", QString::number(tag.count()));
	ret.replace("%type", tag.type().name());
	ret.replace("%stype", QString::number(tag.type().number()));
	print(ret);
}

void SimplePrinter::print(const QList<Tag> &tags) const
{
	for (const Tag &tag : tags) {
		print(tag);
	}
}
