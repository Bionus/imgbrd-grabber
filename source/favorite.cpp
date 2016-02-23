#include "favorite.h"
#include "functions.h"
#include <QDir>


Favorite::Favorite(QString name)
	: name(name)
{}

void Favorite::setImagePath(QString val)
{ imagePath = val; }
void Favorite::setLastViewed(QDateTime val)
{ lastViewed = val; }
void Favorite::setNote(int val)
{ note = val; }

QString Favorite::getName(bool clean) const
{
	if (clean)
		return QString(name).remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
	return name;
}
int Favorite::getNote() const
{ return note; }
QDateTime Favorite::getLastViewed() const
{ return lastViewed; }
QString Favorite::getImagePath() const
{ return imagePath; }

bool Favorite::setImage(QPixmap& img)
{
	if (!QDir(savePath("thumbs")).exists())
		QDir(savePath()).mkdir("thumbs");

	return img
			.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation)
			.save(savePath("thumbs/" + getName(true) + ".png"), "PNG");
}
QPixmap Favorite::getImage() const
{
	QPixmap img(imagePath);
	if (img.width() > 150 || img.height() > 150)
	{
		img = img.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		img.save(savePath("thumbs/" + getName(true) + ".png"), "PNG");
	}
	return img;
}


bool operator==(const Favorite& lhs, const Favorite& rhs)
{ return lhs.getName().toLower() == rhs.getName().toLower(); }
bool operator!=(const Favorite& lhs, const Favorite& rhs)
{ return !(lhs == rhs); }
