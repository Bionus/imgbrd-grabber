#ifndef LANGUAGE_LOADER_H
#define LANGUAGE_LOADER_H

#include <QLocale>
#include <QMap>
#include <QObject>
#include <QString>
#include <QTranslator>


class QCoreApplication;

class LanguageLoader : public QObject
{
	Q_OBJECT

	public:
		explicit LanguageLoader(QString path);
		QMap<QString, QString> getAllLanguages() const;
		bool install(QCoreApplication *app);
		bool uninstall(QCoreApplication *app);

	public slots:
		bool setLanguage(const QString &lang);

	protected:
		QLocale localeFromString(const QString &lang);

	private:
		QString m_path;
		QTranslator m_translator;
		QTranslator m_qtTranslator;
};

#endif // LANGUAGE_LOADER_H
