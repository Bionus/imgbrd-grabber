#ifndef IMAGE_FACTORY_H
#define IMAGE_FACTORY_H

#include <functional>
#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QVariantMap>


class Image;
class Page;
class Profile;
class Site;

typedef std::function<void (const QString &val, QVariantMap &data)> vTransformToken;

class ImageFactory
{
	public:
		static QSharedPointer<Image> build(Site *site, QMap<QString, QString> details, Profile *profile, Page *parent = nullptr);
		static QSharedPointer<Image> build(Site *site, QMap<QString, QString> details, QVariantMap data, Profile *profile, Page *parent = nullptr);

	private:
		static vTransformToken parseString(const QString &key);
		static vTransformToken parseInt(const QString &key);
		static vTransformToken parseBool(const QString &key);

		static void parseCreatedAt(const QString &val, QVariantMap &data);
		static void parseDate(const QString &val, QVariantMap &data);
		static void parseRating(const QString &val, QVariantMap &data);
		static vTransformToken parseTypedTags(const QString &type);
		static void parseTags(const QString &val, QVariantMap &data);
};

#endif // IMAGE_FACTORY_H
