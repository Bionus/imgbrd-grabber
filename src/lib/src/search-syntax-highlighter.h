#ifndef SEARCH_SYNTAX_HIGHLIGHTER_H
#define SEARCH_SYNTAX_HIGHLIGHTER_H

#include <QRegularExpression>
#include <QString>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>


class Profile;
class QTextDocument;

class SearchSyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

	public:
		explicit SearchSyntaxHighlighter(bool full, QTextDocument *parent = nullptr, Profile *profile = nullptr);

	protected:
		void highlightBlock(const QString &text) override;

	protected slots:
		void updateFavorites();
		void updateKeptForLater();

	private:
		Profile *m_profile;
		struct HighlightingRule
		{
			QRegularExpression pattern;
			QTextCharFormat format;
		};
		QVector<HighlightingRule> highlightingRules;
		HighlightingRule *m_favoritesRule;
		HighlightingRule *m_kflRule;
};

#endif // SEARCH_SYNTAX_HIGHLIGHTER_H
