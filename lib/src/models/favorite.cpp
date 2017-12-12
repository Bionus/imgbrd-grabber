#include "models/favorite.h"
#include <QDir>
#include "functions.h"


Favorite::Favorite(const QString &name)
	: Favorite(name, 50, QDateTime::currentDateTime(), QString())
{}
Favorite::Favorite(const QString &name, int note, const QDateTime &lastViewed, const QString &imagePath)
	: Favorite(name, note, lastViewed, 0, QDateTime::currentDateTime(), imagePath)
{}
Favorite::Favorite(const QString &name, int note, const QDateTime &lastViewed, int monitoringInterval, const QDateTime &lastMonitoring, const QString &imagePath)
	: m_name(name), m_note(note), m_lastViewed(lastViewed), m_monitoringInterval(monitoringInterval), m_lastMonitoring(lastMonitoring), m_imagePath(imagePath)
{}

void Favorite::setImagePath(const QString &imagePath)
{ m_imagePath = imagePath; }
void Favorite::setLastViewed(const QDateTime &lastViewed)
{ m_lastViewed = lastViewed; }
void Favorite::setNote(int note)
{ m_note = note; }
void Favorite::setMonitoringInterval(int seconds)
{ m_monitoringInterval = seconds; }
void Favorite::setLastMonitoring(const QDateTime &lastMonitoring)
{ m_lastMonitoring = lastMonitoring; }

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
int Favorite::getMonitoringInterval() const
{ return m_monitoringInterval; }
QDateTime Favorite::getLastMonitoring() const
{ return m_lastMonitoring; }
int Favorite::getSecondsToNextMonitoring() const
{ return QDateTime::currentDateTimeUtc().secsTo(m_lastMonitoring.addSecs(m_monitoringInterval)); }
QString Favorite::getImagePath() const
{ return m_imagePath; }

bool Favorite::setImage(const QPixmap &img)
{
	if (!QDir(savePath("thumbs")).exists())
		QDir(savePath()).mkdir("thumbs");

	m_imagePath = savePath("thumbs/" + getName(true) + ".png");
	return img
			.scaled(QSize(150, 150), Qt::KeepAspectRatio, Qt::SmoothTransformation)
			.save(m_imagePath, "PNG");
}
QPixmap Favorite::getImage() const
{
	QPixmap img(m_imagePath);
	if (img.width() > 150 || img.height() > 150)
	{
		img = img.scaled(QSize(150, 150), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		img.save(savePath("thumbs/" + getName(true) + ".png"), "PNG");
	}
	return img;
}

QString Favorite::toString() const
{
	return getName() + "|" + QString::number(getNote()) + "|" + getLastViewed().toString(Qt::ISODate) + "|" + QString::number(getMonitoringInterval()) + "|" + getLastMonitoring().toString(Qt::ISODate);
}
Favorite Favorite::fromString(const QString &path, const QString &text)
{
	QStringList xp = text.split("|");

	QString tag = xp.takeFirst();
	int note = xp.isEmpty() ? 50 : xp.takeFirst().toInt();
	QDateTime lastViewed = xp.isEmpty() ? QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)) : QDateTime::fromString(xp.takeFirst(), Qt::ISODate);
	int monitoringInterval = xp.isEmpty() ? 0 : xp.takeFirst().toInt();
	QDateTime lastMonitoring = xp.isEmpty() ? QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)) : QDateTime::fromString(xp.takeFirst(), Qt::ISODate);

	QString thumbPath = path + "/thumbs/" + (QString(tag).remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|')) + ".png";
	if (!QFile::exists(thumbPath))
		thumbPath = ":/images/noimage.png";

	return Favorite(tag, note, lastViewed, monitoringInterval, lastMonitoring, thumbPath);
}

bool Favorite::sortByNote(const Favorite &s1, const Favorite &s2)
{ return s1.getNote() < s2.getNote(); }
bool Favorite::sortByName(const Favorite &s1, const Favorite &s2)
{ return s1.getName().toLower() < s2.getName().toLower(); }
bool Favorite::sortByLastviewed(const Favorite &s1, const Favorite &s2)
{ return s1.getLastViewed() < s2.getLastViewed(); }


bool operator==(const Favorite& lhs, const Favorite& rhs)
{ return lhs.getName().toLower() == rhs.getName().toLower(); }
bool operator!=(const Favorite& lhs, const Favorite& rhs)
{ return !(lhs == rhs); }
