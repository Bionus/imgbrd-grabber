#include "tags/tag-name-format.h"


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
	QStringList res;
	res.reserve(words.count());
	for (int i = 0; i < words.length(); ++i)
		res.append(formatted(words[i], i));

	return res.join(m_wordSeparator);
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
			if (index == 0 || m_caseFormat == TagNameFormat::Upper)
				res[0] = res[0].toUpper();
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
