#include "models/favorite.h"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QPixmap>
#include <QSize>
#include <utility>
#include "functions.h"
#include "models/profile.h"


Favorite::Favorite(QString name)
	: Favorite(std::move(name), 50, QDateTime::currentDateTime(), QString())
{}
Favorite::Favorite(QString name, int note, QDateTime lastViewed, QString imagePath, QStringList postFiltering, QList<Site*> sites)
	: Favorite(std::move(name), note, std::move(lastViewed), QList<Monitor>(), std::move(imagePath), std::move(postFiltering), std::move(sites))
{}
Favorite::Favorite(QString name, int note, QDateTime lastViewed, QList<Monitor> monitors, QString imagePath, QStringList postFiltering, QList<Site*> sites)
	: m_name(std::move(name)), m_note(note), m_lastViewed(std::move(lastViewed)), m_monitors(std::move(monitors)), m_imagePath(std::move(imagePath)), m_postFiltering(std::move(postFiltering)), m_sites(std::move(sites))
{}

void Favorite::setImagePath(const QString &imagePath)
{ m_imagePath = imagePath; }
void Favorite::setLastViewed(const QDateTime &lastViewed)
{ m_lastViewed = lastViewed; }
void Favorite::setNote(int note)
{ m_note = note; }
void Favorite::setPostFiltering(const QStringList &postFiltering)
{ m_postFiltering = postFiltering; }
void Favorite::setSites(const QList<Site*> &sites)
{ m_sites = sites; }

QString Favorite::getName(bool clean) const
{
	if (clean) {
		return QString(m_name).remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
	}
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
QStringList Favorite::getPostFiltering() const
{ return m_postFiltering; }
QList<Site*> Favorite::getSites() const
{ return m_sites; }

bool Favorite::setImage(const QPixmap &img)
{
	if (!QDir(savePath("thumbs")).exists()) {
		QDir(savePath()).mkdir("thumbs");
	}

	m_imagePath = savePath("thumbs/" + getName(true) + ".png");
	return img
		.scaled(QSize(150, 150), Qt::KeepAspectRatio, Qt::SmoothTransformation)
		.save(m_imagePath, "PNG");
}
QPixmap Favorite::getImage() const
{
	QPixmap img(m_imagePath);
	if (img.width() > 150 || img.height() > 150) {
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

	const QString tag = xp.takeFirst();
	const int note = xp.isEmpty() ? 50 : xp.takeFirst().toInt();
	const QDateTime lastViewed = xp.isEmpty()
		? QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0))
		: QDateTime::fromString(xp.takeFirst(), Qt::ISODate);

	QString thumbPath = path + "/thumbs/" + (QString(tag).remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|')) + ".png";
	if (!QFile::exists(thumbPath)) {
		thumbPath = ":/images/noimage.png";
	}

	return Favorite(tag, note, lastViewed, thumbPath);
}

void Favorite::toJson(QJsonObject &json) const
{
	json["tag"] = getName();
	json["note"] = getNote();
	json["lastViewed"] = getLastViewed().toString(Qt::ISODate);

	if (!m_postFiltering.isEmpty() && (m_postFiltering.count() > 1 || !m_postFiltering[0].isEmpty())) {
		json["postFiltering"] = QJsonArray::fromStringList(m_postFiltering);
	}

	if (!m_monitors.isEmpty()) {
		QJsonArray monitorsJson;
		for (const Monitor &monitor : m_monitors) {
			QJsonObject obj;
			monitor.toJson(obj);
			monitorsJson.append(obj);
		}
		json["monitors"] = monitorsJson;
	}
}
Favorite Favorite::fromJson(const QString &path, const QJsonObject &json, Profile *profile)
{
	const QString tag = json["tag"].toString();
	const int note = json["note"].toInt();
	const QDateTime lastViewed = QDateTime::fromString(json["lastViewed"].toString(), Qt::ISODate);

	QString thumbPath = path + "/thumbs/" + (QString(tag).remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|')) + ".png";
	if (!QFile::exists(thumbPath)) {
		thumbPath = ":/images/noimage.png";
	}

	// Monitors
	QList<Monitor> monitors;
	if (json.contains("monitors")) {
		QJsonArray monitorsJson = json["monitors"].toArray();
		for (auto monitorJson : monitorsJson) {
			monitors.append(Monitor::fromJson(monitorJson.toObject(), profile));
		}
	}

	// Post-filtering
	QStringList postFiltering;
	if (json.contains("postFiltering")) {
		QJsonArray jsonPostFiltering = json["postFiltering"].toArray();
		postFiltering.reserve(jsonPostFiltering.count());
		for (auto filter : jsonPostFiltering) {
			postFiltering.append(filter.toString());
		}
	}

	return Favorite(tag, note, lastViewed, monitors, thumbPath, postFiltering);
}

bool Favorite::sortByNote(const Favorite &s1, const Favorite &s2)
{ return s1.getNote() < s2.getNote(); }
bool Favorite::sortByName(const Favorite &s1, const Favorite &s2)
{ return s1.getName().toLower() < s2.getName().toLower(); }
bool Favorite::sortByLastViewed(const Favorite &s1, const Favorite &s2)
{ return s1.getLastViewed() < s2.getLastViewed(); }


bool operator==(const Favorite &lhs, const Favorite &rhs)
{ return lhs.getName().toLower() == rhs.getName().toLower(); }
bool operator!=(const Favorite &lhs, const Favorite &rhs)
{ return !(lhs == rhs); }
