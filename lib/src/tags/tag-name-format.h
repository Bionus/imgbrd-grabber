#ifndef TAG_NAME_FORMAT_H
#define TAG_NAME_FORMAT_H

#include <QMetaType>
#include <QStringList>


class TagNameFormat
{
	public:
		enum CaseFormat
		{
			Lower, // some_tag
			UpperFirst, // Some_tag
			Upper, // Some_Tag
			Caps, // SOME_TAG
		};

		TagNameFormat() = default;
		TagNameFormat(CaseFormat caseFormat, QString wordSeparator);
		static TagNameFormat &Normalized();
		CaseFormat caseFormat() const;
		QString wordSeparator() const;
		QString formatted(const QStringList &words) const;

	protected:
		QString formatted(const QString &word, int index) const;

	private:
		CaseFormat m_caseFormat = CaseFormat::Lower;
		QString m_wordSeparator;
};

bool operator==(const TagNameFormat &a, const TagNameFormat &b);

Q_DECLARE_METATYPE(TagNameFormat)

#endif // TAG_NAME_H
