#ifndef CONDITIONAL_FILENAME_H
#define CONDITIONAL_FILENAME_H

#include <QMap>
#include <QString>
#include "filename/filename.h"


struct FilenameNodeCondition;
class QSettings;
class Token;

class ConditionalFilename
{
	public:
		ConditionalFilename(QString condition, const QString &filename, QString path);
		bool matches(const QMap<QString, Token> &tokens, QSettings *settings) const;

		QString condition;
		Filename filename;
		QString path;

	private:
		FilenameNodeCondition *m_ast = nullptr;
};

#endif // CONDITIONAL_FILENAME_H
