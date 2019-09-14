#include "filename/ast-filename.h"
#include "filename/ast/filename-node-root.h"
#include "filename/filename-parser.h"
#include "filename/filename-resolution-visitor.h"
#if DEBUG
	#include "filename/filename-print-visitor.h"
	#include "logger.h"
#endif


AstFilename::AstFilename(const QString &str)
	: m_parser(str)
{}

void AstFilename::parse()
{
	auto ast = m_parser.parseRoot();
	if (m_parser.error().isEmpty()) {
		m_ast = ast;

		FilenameResolutionVisitor resolutionVisitor;
		m_tokens = resolutionVisitor.run(*m_ast);

		#if DEBUG
			FilenamePrintVisitor printVisitor;
			QString printedAst = printVisitor.run(*m_ast);
			log(QString("Parsed filename '%1' into '%2'").arg(m_parser.str(), printedAst), Logger::Debug);
		#endif
	}

	m_parsed = true;
}

const QString &AstFilename::error()
{
	if (!m_parsed) {
		parse();
	}

	return m_parser.error();
}

FilenameNodeRoot *AstFilename::ast()
{
	if (!m_parsed) {
		parse();
	}

	return m_ast;
}

const QSet<QString> &AstFilename::tokens()
{
	if (!m_parsed) {
		parse();
	}

	return m_tokens;
}
