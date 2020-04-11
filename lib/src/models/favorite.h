#ifndef FAVORITE_H
#define FAVORITE_H

#include <QDateTime>
#include <QList>
#include <QPixmap>
#include <QString>
#include "models/monitor.h"


class Site;

class Favorite
{
	public:
		explicit Favorite(QString name);
		Favorite(QString name, int note, QDateTime lastViewed, QString imagePath = "", QStringList postFiltering = QStringList());
		Favorite(QString name, int note, QDateTime lastViewed, QList<Monitor> monitors, QString imagePath = "", QStringList postFiltering = QStringList());

		// Getters and setters
		void setNote(int note);
		void setLastViewed(const QDateTime &lastViewed);
		void setImagePath(const QString &imagePath);
		void setPostFiltering(const QStringList &postFiltering);
		int getNote() const;
		QDateTime getLastViewed() const;
		QString getImagePath() const;
		QList<Monitor> &getMonitors();
		QStringList getPostFiltering() const;

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
		QList<Monitor> m_monitors;
		QString m_imagePath;
		QStringList m_postFiltering;
};

bool operator==(const Favorite &lhs, const Favorite &rhs);
bool operator!=(const Favorite &lhs, const Favorite &rhs);

#endif // FAVORITE_H
