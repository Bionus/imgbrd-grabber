#ifndef FAVORITE_H
#define FAVORITE_H

#include <QString>
#include <QDateTime>
#include <QPixmap>


class Favorite
{
	public:
		Favorite(QString name, int note, QDateTime lastViewed, QString imagePath = "");
		void setNote(int);
		void setLastViewed(QDateTime);
		void setImagePath(QString);
		bool setImage(QPixmap&);
		QString getName(bool clean = false) const;
		int getNote() const;
		QDateTime getLastViewed() const;
		QString getImagePath() const;
		QPixmap getImage() const;

	private:
		QString _name;
		int _note;
		QDateTime _lastViewed;
		QString _imagePath;
};

bool operator==(const Favorite& lhs, const Favorite& rhs);
bool operator!=(const Favorite& lhs, const Favorite& rhs);

#endif // FAVORITE_H
