#include "tags/tag-name-format.h"
#include <QStringList>
#include <utility>


TagNameFormat::TagNameFormat(CaseFormat caseFormat, QString wordSeparator)
	: m_caseFormat(caseFormat), m_wordSeparator(std::move(wordSeparator))
{}

TagNameFormat &TagNameFormat::Normalized()
{
	static TagNameFormat normalized(CaseFormat::Lower, "_");
	return normalized;
}


TagNameFormat::CaseFormat TagNameFormat::caseFormat() const
{
	return m_caseFormat;
}

QString TagNameFormat::wordSeparator() const
{
	return m_wordSeparator;
}


QString TagNameFormat::formatted(const QStringList &words) const
{
	QString ret;

	// Early return for empty inputs
	if (words.isEmpty()) {
		return ret;
	}

	// Allocate the proper size for the return string
	int totalLength = m_wordSeparator.length() * (words.length() - 1);
	for (const QString &word : words) {
		totalLength += word.length();
	}
	ret.reserve(totalLength);

	// Format every word
	for (int i = 0; i < words.length(); ++i) {
		if (i > 0) {
			ret.append(m_wordSeparator);
		}
		ret.append(formatted(words[i], i));
	}

	return ret;
}

QString TagNameFormat::formatted(const QString &word, int index) const
{
	switch (m_caseFormat)
	{
		case TagNameFormat::Lower:
			return word.toLower();

		case TagNameFormat::Upper:
		case TagNameFormat::UpperFirst:
		{
			auto res = word.toLower();
			if (index == 0 || m_caseFormat == TagNameFormat::Upper) {
				res[0] = res[0].toUpper();
			}
			return res;
		}

		case TagNameFormat::Caps:
			return word.toUpper();
	}

	return word;
}

bool operator==(const TagNameFormat &a, const TagNameFormat &b)
{
	return a.caseFormat() == b.caseFormat() && a.wordSeparator() == b.wordSeparator();
}
