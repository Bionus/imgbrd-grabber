#ifndef FAVORITE_H
#define FAVORITE_H

#include <QDateTime>
#include <QJsonObject>
#include <QPixmap>
#include <QString>
#include "models/monitor.h"


class Site;

class Favorite
{
	public:
		explicit Favorite(const QString &name);
		Favorite(const QString &name, int note, const QDateTime &lastViewed, const QString &imagePath = "");
		Favorite(const QString &name, int note, const QDateTime &lastViewed, const QList<Monitor> &monitors, const QString &imagePath = "");

		// Getters and setters
		void setNote(int note);
		void setLastViewed(const QDateTime &lastViewed);
		void setImagePath(const QString &imagePath);
		int			getNote()		const;
		QDateTime	getLastViewed()	const;
		QString		getImagePath()	const;
		QList<Monitor> &getMonitors();

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
		static Favorite fromJson(const QString &path, const QJsonObject &json, const QMap<QString, Site*> &sites);

		static bool sortByNote(const Favorite &s1, const Favorite &s2);
		static bool sortByName(const Favorite &s1, const Favorite &s2);
		static bool sortByLastviewed(const Favorite &s1, const Favorite &s2);

	private:
		QString		m_name;
		int			m_note;
		QDateTime	m_lastViewed;
		QList<Monitor> m_monitors;
		QString		m_imagePath;
};

bool operator==(const Favorite& lhs, const Favorite& rhs);
bool operator!=(const Favorite& lhs, const Favorite& rhs);

#endif // FAVORITE_H
