#include "favorite.h"
#include "functions.h"
#include <QDir>


Favorite::Favorite(QString name, int note, QDateTime lastViewed, QString imagePath)
	: _name(name), _note(note), _lastViewed(lastViewed), _imagePath(imagePath)
{}

void Favorite::setImagePath(QString val)
{ _imagePath = val; }
void Favorite::setLastViewed(QDateTime val)
{ _lastViewed = val; }
void Favorite::setNote(int val)
{ _note = val; }

QString Favorite::getName(bool clean) const
{
	if (clean)
		return QString(_name).remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
	return _name;
}
int Favorite::getNote() const
{ return _note; }
QDateTime Favorite::getLastViewed() const
{ return _lastViewed; }
QString Favorite::getImagePath() const
{ return _imagePath; }

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
	QPixmap img(_imagePath);
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
