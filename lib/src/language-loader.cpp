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

bool LanguageLoader::install(QCoreApplication *app)
{
	const bool general = app->installTranslator(&m_translator);
	const bool qt = app->installTranslator(&m_qtTranslator);
	return general && qt;
}

bool LanguageLoader::uninstall(QCoreApplication *app)
{
	const bool general = app->removeTranslator(&m_translator);
	const bool qt = app->removeTranslator(&m_qtTranslator);
	return general && qt;
}

bool LanguageLoader::setLanguage(const QString &lang)
{
	log(QStringLiteral("Setting language to '%1'...").arg(lang), Logger::Info);

	QLocale::setDefault(QLocale(lang));

	const bool general = m_translator.load(m_path + lang + ".qm");
	const bool qt = m_qtTranslator.load(m_path + "qt/" + lang + ".qm");
	return general && qt;
}
