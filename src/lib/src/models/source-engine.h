#ifndef SOURCE_ENGINE_H
#define SOURCE_ENGINE_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>


class Api;
class Auth;
class SearchFormat;
class TagNameFormat;

/**
 * Represents a source engine that can be used for multiple sites.
 *
 * One engine can support multiple sites, and each site will share the same logic, with only the settings and base URL
 * changing. This is very useful for when an engine is used in many places. For many proprietary sites, each site will
 * have its own source.
 *
 * @see https://www.bionus.org/imgbrd-grabber/docs/sites/source.html
 */
class SourceEngine : public QObject
{
	Q_OBJECT

	public:
		explicit SourceEngine(QObject *parent = nullptr);

		virtual bool isValid() const = 0;
		Api *getApi(const QString &name) const;

		// Getters
		virtual const QString &getName() const = 0;
		virtual const QList<Api*> &getApis() const = 0;
		virtual const QMap<QString, Auth*> &getAuths() const = 0;
		virtual const QStringList &getAdditionalTokens() const = 0;
		virtual const TagNameFormat &getTagNameFormat() const = 0;
		virtual const SearchFormat &getSearchFormat() const = 0;

	signals:
		void changed();
};

#endif // SOURCE_ENGINE_H
