#ifndef FILENAME_RESOLUTION_VISITOR_H
#define FILENAME_RESOLUTION_VISITOR_H

#include <QSet>
#include <QString>
#include "filename/ast/filename-visitor-base.h"


class FilenameResolutionVisitor : public FilenameVisitorBase
{
	public:
		QSet<QString> run(const FilenameNodeRoot &node);

		void visit(const FilenameNodeConditionToken &node) override;
		void visit(const FilenameNodeVariable &node) override;

	private:
		QSet<QString> m_results;
};

#endif // FILENAME_RESOLUTION_VISITOR_H
