#include "models/favorite.h"
#include <QDir>
#include <QJsonArray>
#include "functions.h"


Favorite::Favorite(const QString &name)
	: Favorite(name, 50, QDateTime::currentDateTime(), QString())
{}
Favorite::Favorite(const QString &name, int note, const QDateTime &lastViewed, const QString &imagePath)
	: Favorite(name, note, lastViewed, QList<Monitor>(), imagePath)
{}
Favorite::Favorite(const QString &name, int note, const QDateTime &lastViewed, const QList<Monitor> &monitors, const QString &imagePath)
	: m_name(name), m_note(note), m_lastViewed(lastViewed), m_monitors(monitors), m_imagePath(imagePath)
{}

void Favorite::setImagePath(const QString &imagePath)
{ m_imagePath = imagePath; }
void Favorite::setLastViewed(const QDateTime &lastViewed)
{ m_lastViewed = lastViewed; }
void Favorite::setNote(int note)
{ m_note = note; }

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
QList<Monitor> &Favorite::getMonitors()
{ return m_monitors; }

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
	return getName() + "|" + QString::number(getNote()) + "|" + getLastViewed().toString(Qt::ISODate);
}
Favorite Favorite::fromString(const QString &path, const QString &text)
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

void Favorite::toJson(QJsonObject &json) const
{
	json["tag"] = getName();
	json["note"] = getNote();
	json["lastViewed"] = getLastViewed().toString(Qt::ISODate);

	QJsonArray monitorsJson;
	for (const Monitor &monitor : m_monitors)
	{
		QJsonObject obj;
		monitor.toJson(obj);
		monitorsJson.append(obj);
	}
	json["monitors"] = monitorsJson;
}
Favorite Favorite::fromJson(const QString &path, const QJsonObject &json, const QMap<QString, Site *> &sites)
{
	QString tag = json["tag"].toString();
	int note = json["note"].toInt();
	QDateTime lastViewed = QDateTime::fromString(json["lastViewed"].toString(), Qt::ISODate);

	QString thumbPath = path + "/thumbs/" + (QString(tag).remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|')) + ".png";
	if (!QFile::exists(thumbPath))
		thumbPath = ":/images/noimage.png";

	QList<Monitor> monitors;
	QJsonArray monitorsJson = json["monitors"].toArray();
	for (auto monitorJson : monitorsJson)
	{ monitors.append(Monitor::fromJson(monitorJson.toObject(), sites)); }

	return Favorite(tag, note, lastViewed, monitors, thumbPath);
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
