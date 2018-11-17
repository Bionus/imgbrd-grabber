#include "danbooru-downloader-importer.h"
#include <QFileInfo>
#include <QMap>
#include <QRegularExpression>
#include <QSettings>


DanbooruDownloaderImporter::DanbooruDownloaderImporter()
	: m_firefoxProfilePath(QString())
{
	QSettings cfg(QSettings::IniFormat, QSettings::UserScope, "Mozilla", "Firefox");
	const QString path = QFileInfo(cfg.fileName()).absolutePath() + "/Firefox";
	if (QFile::exists(path + "/profiles.ini"))
	{
		QSettings profiles(path + "/profiles.ini", QSettings::IniFormat);
		m_firefoxProfilePath = path + "/" + profiles.value("Profile0/Path").toString();
	}
}

bool DanbooruDownloaderImporter::isInstalled() const
{
	return !m_firefoxProfilePath.isEmpty() && QFile::exists(m_firefoxProfilePath + "/extensions/danbooru_downloader@cuberocks.net.xpi");
}

void DanbooruDownloaderImporter::import(QSettings *dest) const
{
	QFile prefs(m_firefoxProfilePath + "/prefs.js");
	if (prefs.exists() && prefs.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	const QString source = prefs.readAll();
	QRegularExpression rx("user_pref\\(\"danbooru.downloader.([^\"]+)\", ([^\\)]+)\\);");
	QMap<QString, QString> firefox, assoc;
	assoc["blacklist"] = "blacklistedtags";
	assoc["generalTagsSeparator"] = "separator";
	assoc["multipleArtistsAll"] = "artist_useall";
	assoc["multipleArtistsDefault"] = "artist_value";
	assoc["multipleArtistsSeparator"] = "artist_sep";
	assoc["multipleCharactersAll"] = "character_useall";
	assoc["multipleCharactersDefault"] = "character_value";
	assoc["multipleCharactersSeparator"] = "character_sep";
	assoc["multipleCopyrightsAll"] = "copyright_useall";
	assoc["multipleCopyrightsDefault"] = "copyright_value";
	assoc["multipleCopyrightsSeparator"] = "copyright_sep";
	assoc["noArtist"] = "artist_empty";
	assoc["noCharacter"] = "character_empty";
	assoc["noCopyright"] = "copyright_empty";
	assoc["targetFolder"] = "path";
	assoc["targetName"] = "filename";

	auto matches = rx.globalMatch(source);
	while (matches.hasNext())
	{
		auto match = matches.next();
		QString value = match.captured(2);
		if (value.startsWith('"')) { value = value.right(value.length() - 1); }
		if (value.endsWith('"')) { value = value.left(value.length() - 1); }
		firefox[match.captured(1)] = value;
	}

	dest->beginGroup("Save");
	if (firefox.contains("useBlacklist"))
	{ dest->setValue("downloadblacklist", firefox["useBlacklist"] != "true"); }
	for (auto it = firefox.constBegin(); it != firefox.constEnd(); ++it)
	{
		if (assoc.contains(it.key()))
		{
			QString v(it.value());
			v.replace("\\\\", "\\");
			dest->setValue(assoc[it.key()], v);
		}
	}
	dest->endGroup();
	prefs.close();
}
