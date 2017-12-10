#ifndef FAVORITE_H
#define FAVORITE_H

#include <QDateTime>
#include <QPixmap>
#include <QString>


class Favorite
{
	public:
		explicit Favorite(const QString &name);
		Favorite(const QString &name, int note, const QDateTime &lastViewed, const QString &imagePath = "");

		// Getters and setters
		void setNote(int note);
		void setLastViewed(const QDateTime &lastViewed);
		void setImagePath(const QString &imagePath);
		int			getNote()		const;
		QDateTime	getLastViewed()	const;
		QString		getImagePath()	const;

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

		static bool sortByNote(const Favorite &s1, const Favorite &s2);
		static bool sortByName(const Favorite &s1, const Favorite &s2);
		static bool sortByLastviewed(const Favorite &s1, const Favorite &s2);

	private:
		QString		m_name;
		int			m_note;
		QDateTime	m_lastViewed;
		QString		m_imagePath;
};

bool operator==(const Favorite& lhs, const Favorite& rhs);
bool operator!=(const Favorite& lhs, const Favorite& rhs);

#endif // FAVORITE_H
