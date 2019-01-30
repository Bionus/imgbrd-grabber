#ifndef AST_FILENAME_H
#define AST_FILENAME_H

#include <QSet>
#include <QString>
#include "filename/filename-parser.h"


struct FilenameNodeRoot;

class AstFilename
{
	public:
		explicit AstFilename(const QString &str);
		const QString &error();
		FilenameNodeRoot *ast();
		const QSet<QString> &tokens();

	protected:
		void parse();

	private:
		FilenameParser m_parser;
		bool m_parsed = false;

		FilenameNodeRoot *m_ast = nullptr;
		QSet<QString> m_tokens;
};

#endif // AST_FILENAME_H
