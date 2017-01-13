#ifndef LANGUAGE_LOADER_H
#define LANGUAGE_LOADER_H

#include <QMap>
#include <QString>


class LanguageLoader
{
	public:
		LanguageLoader(QString path);
		QMap<QString, QString> getAllLanguages() const;

	private:
		QString m_path;
};

#endif // LANGUAGE_LOADER_H
