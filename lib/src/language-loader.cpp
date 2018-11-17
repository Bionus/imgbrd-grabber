#include "language-loader.h"
#include <QCoreApplication>
#include <QDir>
#include <QLocale>
#include <QSettings>
#include "logger.h"


LanguageLoader::LanguageLoader(QString path)
	: m_path(std::move(path))
{}

QMap<QString, QString> LanguageLoader::getAllLanguages() const
{
	QSettings fullLanguages(m_path + "languages.ini", QSettings::IniFormat);
	fullLanguages.setIniCodec("UTF-8");

	QStringList languageFiles = QDir(m_path).entryList(QStringList() << QStringLiteral("*.qm"), QDir::Files);
	QMap<QString, QString> languages;
	for (const QString &languageFile : languageFiles)
	{
		const QString lang = languageFile.left(languageFile.length() - 3);
		const QString fullLang = fullLanguages.value(lang, lang).toString();
		languages[lang] = fullLang;
	}

	if (!languages.contains("English"))
	{
		languages[""] = "English";
	}

	return languages;
}

void LanguageLoader::install(QCoreApplication *app)
{
	app->installTranslator(&m_translator);
	app->installTranslator(&m_qtTranslator);
}

void LanguageLoader::uninstall(QCoreApplication *app)
{
	app->removeTranslator(&m_translator);
	app->removeTranslator(&m_qtTranslator);
}

void LanguageLoader::setLanguage(const QString &lang)
{
	log(QStringLiteral("Setting language to '%1'...").arg(lang), Logger::Info);

	QLocale::setDefault(QLocale(lang));

	m_translator.load(m_path + lang + ".qm");
	m_qtTranslator.load(m_path + "qt/" + lang + ".qm");
}
