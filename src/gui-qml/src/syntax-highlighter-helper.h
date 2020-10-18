#ifndef SYNTAX_HIGHLIGHTER_HELPER_H
#define SYNTAX_HIGHLIGHTER_HELPER_H

#include <QObject>
#include <QQuickTextDocument>
#include "search-syntax-highlighter.h"


class SyntaxHighlighterHelper: public QObject
{
    Q_OBJECT

	Q_PROPERTY(QQuickTextDocument *quickDocument READ quickDocument WRITE setQuickDocument NOTIFY quickDocumentChanged)

    public:
        SyntaxHighlighterHelper(QObject *parent = nullptr) : QObject(parent), m_quickDocument(nullptr) {}
        QQuickTextDocument *quickDocument() const { return m_quickDocument; }
        void setQuickDocument(QQuickTextDocument *quickDocument) { m_quickDocument = quickDocument; if (m_quickDocument) { new SearchSyntaxHighlighter(m_quickDocument->textDocument()); } }

    signals:
        void quickDocumentChanged();

    private:
        QQuickTextDocument *m_quickDocument;
};

#endif // SYNTAX_HIGHLIGHTER_HELPER_H
