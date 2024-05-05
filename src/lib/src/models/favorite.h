#ifndef FAVORITE_H
#define FAVORITE_H

#include <QDateTime>
#include <QList>
#include <QString>
#include "monitoring/monitor.h"


class QPixmap;
class Site;

class Favorite
{
	public:
		explicit Favorite(QString name);
		Favorite(QString name, int note, QDateTime lastViewed, QMap<QString, QVariantMap> lastImages = {}, QString imagePath = "", QStringList postFiltering = {}, QList<Site*> sites = {});
		Favorite(QString name, int note, QDateTime lastViewed, QMap<QString, QVariantMap> lastImages, QList<Monitor> monitors, QString imagePath = "", QStringList postFiltering = {}, QList<Site*> sites = {});

		// Getters and setters
		void setNote(int note);
		void setLastViewed(const QDateTime &lastViewed);
		void setLastImage(const QString &site, const QVariantMap &lastImage);
		void setImagePath(const QString &imagePath);
		void setPostFiltering(const QStringList &postFiltering);
		void setSites(const QList<Site*> &sites);
		int getNote() const;
		QDateTime getLastViewed() const;
		QMap<QString, QVariantMap> getLastImages() const { return m_lastImages; };
		QString getImagePath() const;
		QList<Monitor> &getMonitors();
		QStringList getPostFiltering() const;
		QList<Site*> getSites() const;

		/**
		 * @brief Return the name of this favorite.
		 * @param clean Remove special characters from the name.
		 * @return The name of this favorite, maybe cleaned.
		 */
		QString getName(bool clean = false) const;

		bool setImage(const QPixmap &img);
		QPixmap getImage() const;

		QString toString() const;
		static Favorite fromString(const QString &path, const QString &text);
		void toJson(QJsonObject &json) const;
		static Favorite fromJson(const QString &path, const QJsonObject &json, Profile *profile);

		static bool sortByNote(const Favorite &s1, const Favorite &s2);
		static bool sortByName(const Favorite &s1, const Favorite &s2);
		static bool sortByLastViewed(const Favorite &s1, const Favorite &s2);

	private:
		QString m_name;
		int m_note;
		QDateTime m_lastViewed;
		QMap<QString, QVariantMap> m_lastImages;
		QList<Monitor> m_monitors;
		QString m_imagePath;
		QStringList m_postFiltering;
		QList<Site*> m_sites;
};

bool operator==(const Favorite &lhs, const Favorite &rhs);
bool operator!=(const Favorite &lhs, const Favorite &rhs);

#endif // FAVORITE_H
