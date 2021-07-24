#include "search-syntax-highlighter.h"
#include <QColor>
#include <QRegularExpression>


SearchSyntaxHighlighter::SearchSyntaxHighlighter(bool full, QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	favoritesFormat.setForeground(QColor("#ffc0cb"));
	keptForLaterFormat.setForeground(QColor("#000000"));

	HighlightingRule rule;

	// Or format "~tag"
	orFormat.setForeground(Qt::green);
	rule.pattern = QRegularExpression("(?: |^)~([^ ]+)");
	rule.format = orFormat;
	highlightingRules.append(rule);

	// Exclusion format "-tag"
	excludeFormat.setForeground(Qt::red);
	rule.pattern = QRegularExpression("(?: |^)-([^ ]+)");
	rule.format = excludeFormat;
	highlightingRules.append(rule);

	if (!full) {
		// Meta other format "unknown_meta:value"
		metaOtherFormat.setForeground(QColor("#ff0000"));
		rule.pattern = QRegularExpression("(?: |^)([^:]+):([^: ][^ ]*)?(?: |$)");
		rule.format = metaOtherFormat;
		highlightingRules.append(rule);
	} else {
		// MD5 format "qdrg15sdfgs1d2f1gs3dfg"
		md5Format.setForeground(QColor("#800080"));
		rule.pattern = QRegularExpression("(?: |^)([0-9A-F]{32})", QRegularExpression::CaseInsensitiveOption);
		rule.format = md5Format;
		highlightingRules.append(rule);

		// URL format "http://..."
		urlFormat.setForeground(Qt::blue);
		rule.pattern = QRegularExpression("(?: |^)(https?://[^\\s/$.?#].[^\\s]*)(?: |$)");
		rule.format = urlFormat;
		highlightingRules.append(rule);
	}

	// Meta format "meta:value"
	metaFormat.setForeground(QColor("#a52a2a"));
	rule.pattern = QRegularExpression("(?: |^)(user|fav|md5|pool|rating|source|status|approver|unlocked|sub|id|width|height|aspect_ratio|score|mpixels|filesize|filetype|date|gentags|arttags|chartags|copytags|status|status|approver|order|parent|sort|grabber):([^: ][^ ]*)?(?: |$)", QRegularExpression::CaseInsensitiveOption);
	rule.format = metaFormat;
	highlightingRules.append(rule);
}

void SearchSyntaxHighlighter::highlightBlock(const QString &text)
{
	for (const HighlightingRule &rule : highlightingRules) {
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
		while (matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			setFormat(match.capturedStart(), match.capturedLength(), rule.format);
		}
	}
}
