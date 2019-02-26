#include "filename/ast-filename.h"
#include "filename/ast/filename-node-root.h"
#include "filename/filename-parser.h"
#include "filename/filename-resolution-visitor.h"


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
