#ifndef FAVORITE_H
#define FAVORITE_H

#include <QString>
#include <QDateTime>
#include <QPixmap>


class Favorite
{
	public:
		Favorite(int id, QString name);
		void setNote(int);
		void setLastViewed(QDateTime);
		void setImagePath(QString);
		bool setImage(QPixmap);
		int getId() const;
		QString getName(bool clean = false) const;
		int getNote() const;
		QDateTime getLastViewed() const;
		QString getImagePath() const;
		QPixmap getImage() const;

	private:
		int id;
		QString name;
		int note;
		QDateTime lastViewed;
		QString imagePath;
};

bool operator==(const Favorite& lhs, const Favorite& rhs);
bool operator!=(const Favorite& lhs, const Favorite& rhs);

#endif // FAVORITE_H
