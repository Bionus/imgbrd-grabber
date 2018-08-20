#ifndef LANGUAGE_LOADER_H
#define LANGUAGE_LOADER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QTranslator>


class QCoreApplication;

class LanguageLoader : public QObject
{
	Q_OBJECT

	public:
		explicit LanguageLoader(QString path);
		QMap<QString, QString> getAllLanguages() const;
		void install(QCoreApplication *app);
		void uninstall(QCoreApplication *app);

	public slots:
		void setLanguage(const QString &lang);

	private:
		QString m_path;
		QTranslator m_translator;
		QTranslator m_qtTranslator;
};

#endif // LANGUAGE_LOADER_H
