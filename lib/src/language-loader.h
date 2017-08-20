#ifndef LANGUAGE_LOADER_H
#define LANGUAGE_LOADER_H

#include <QMap>
#include <QString>


class LanguageLoader
{
	public:
		explicit LanguageLoader(QString path);
		QMap<QString, QString> getAllLanguages() const;

	private:
		QString m_path;
};

#endif // LANGUAGE_LOADER_H
