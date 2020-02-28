#ifndef IMAGE_FACTORY_H
#define IMAGE_FACTORY_H

#include <functional>
#include <QMap>
#include <QString>
#include <QVariantMap>


class Image;
class Page;
class Profile;
class Site;

typedef std::function<void(const QString &val, QMap<QString, QVariant> &data)> vTransformToken;

class ImageFactory
{
	public:
		static Image *build(Site *site, QMap<QString, QString> details, QMap<QString, QVariant> data, Profile *profile, Page *parent = nullptr);

	private:
		static void parseCreatedAt(const QString &val, QMap<QString, QVariant> &data);
		static void parseDate(const QString &val, QMap<QString, QVariant> &data);
		static void parseRating(const QString &val, QMap<QString, QVariant> &data);
		static vTransformToken parseTypedTags(const QString &type);
		static void parseTags(const QString &val, QMap<QString, QVariant> &data);
};

#endif // IMAGE_FACTORY_H
