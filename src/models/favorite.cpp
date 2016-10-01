#include "favorite.h"
#include "functions.h"
#include <QDir>
#include <QDebug>

Favorite::Favorite(QString name)
	: m_name(name), m_note(0), m_lastViewed(QDateTime::currentDateTime()), m_imagePath(QString())
{}
Favorite::Favorite(QString name, int note, QDateTime lastViewed, QString imagePath)
	: m_name(name), m_note(note), m_lastViewed(lastViewed), m_imagePath(imagePath)
{}

void Favorite::setImagePath(QString val)
{ m_imagePath = val; }
void Favorite::setLastViewed(QDateTime val)
{ m_lastViewed = val; }
void Favorite::setNote(int val)
{ m_note = val; }

QString Favorite::getName(bool clean) const
{
	if (clean)
		return QString(m_name).remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
	return m_name;
}
int Favorite::getNote() const
{ return m_note; }
QDateTime Favorite::getLastViewed() const
{ return m_lastViewed; }
QString Favorite::getImagePath() const
{ return m_imagePath; }

bool Favorite::setImage(QPixmap& img)
{
	if (!QDir(savePath("thumbs")).exists())
		QDir(savePath()).mkdir("thumbs");

	m_imagePath = savePath("thumbs/" + getName(true) + ".png");
	return img
			.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation)
			.save(m_imagePath, "PNG");
}
QPixmap Favorite::getImage() const
{
	QPixmap img(m_imagePath);
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
