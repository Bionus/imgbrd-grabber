#include "favorite.h"
#include <QDir>
#include <QDebug>


// TODO: remove this
#include <QApplication>
#include <QStandardPaths>
QString _savePath(QString file = "", bool exists = false)
{
	#ifdef TEST
		Q_UNUSED(exists);
		return QDir::toNativeSeparators(QDir::currentPath()+"/tests/resources/"+file);
	#else
		QString check = exists ? file : "settings.ini";
		if (QFile(QDir::toNativeSeparators(qApp->applicationDirPath()+"/"+check)).exists())
		{ return QDir::toNativeSeparators(qApp->applicationDirPath()+"/"+file); }
		if (QFile(QDir::toNativeSeparators(QDir::currentPath()+"/"+check)).exists())
		{ return QDir::toNativeSeparators(QDir::currentPath()+"/"+file); }
		if (QFile(QDir::toNativeSeparators(QDir::homePath()+"/Grabber/"+check)).exists())
		{ return QDir::toNativeSeparators(QDir::homePath()+"/Grabber/"+file); }
		#ifdef __linux__
			if (QFile(QDir::toNativeSeparators(QDir::homePath()+"/.Grabber/"+check)).exists())
			{ return QDir::toNativeSeparators(QDir::homePath()+"/.Grabber/"+file); }
		#endif
		return QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/"+file);
	#endif
}


Favorite::Favorite(QString name)
	: Favorite(name, 50, QDateTime::currentDateTime(), QString())
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
	if (!QDir(_savePath("thumbs")).exists())
		QDir(_savePath()).mkdir("thumbs");

	m_imagePath = _savePath("thumbs/" + getName(true) + ".png");
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
		img.save(_savePath("thumbs/" + getName(true) + ".png"), "PNG");
	}
	return img;
}

QString Favorite::toString() const
{
	return getName() + "|" + QString::number(getNote()) + "|" + getLastViewed().toString(Qt::ISODate);
}
Favorite Favorite::fromString(QString path, QString text)
{
	QStringList xp = text.split("|");

	QString tag = xp.takeFirst();
	int note = xp.isEmpty() ? 50 : xp.takeFirst().toInt();
	QDateTime lastViewed = xp.isEmpty() ? QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)) : QDateTime::fromString(xp.takeFirst(), Qt::ISODate);

	QString thumbPath = path + "/thumbs/" + (QString(tag).remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|')) + ".png";
	if (!QFile::exists(thumbPath))
		thumbPath = ":/images/noimage.png";

	return Favorite(tag, note, lastViewed, thumbPath);
}


bool operator==(const Favorite& lhs, const Favorite& rhs)
{ return lhs.getName().toLower() == rhs.getName().toLower(); }
bool operator!=(const Favorite& lhs, const Favorite& rhs)
{ return !(lhs == rhs); }
