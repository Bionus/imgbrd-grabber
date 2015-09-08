#include <QtTest>
#include <QString>

#include "../source/tag.cpp"

class TestTag : public QObject
{
	Q_OBJECT

	public:
		TestTag();

	private Q_SLOTS:
		void testText();
		void testCount();
		void testRelated();
		void testShortType();
		void testStylishedFavorite();
		void testStylishedNotFavorite();
		void testType();
		void testTypedTextGeneral();
		void testTypedTextArtist();
};

TestTag::TestTag()
{ }

void TestTag::testText()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.text(), QString("tag_text"));
}
void TestTag::testCount()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.count(), 123);
}
void TestTag::testType()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.type(), QString("artist"));
}
void TestTag::testShortType()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.shortType(), 1);
}
void TestTag::testStylishedFavorite()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.stylished(QStringList() << "tag_text"), QString("<span style=\"color:pink\">tag_text</span>"));
}
void TestTag::testStylishedNotFavorite()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.stylished(QStringList() << "tag_other"), QString("tag_text"));
}
void TestTag::testRelated()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.related(), QStringList() << "related1" << "related2" << "related3");
}
void TestTag::testTypedTextArtist()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.typedText(), QString("artist:tag_text"));
}
void TestTag::testTypedTextGeneral()
{
	Tag tag("tag_text", "general", 123, QStringList() << "related1" << "related2" << "related3");
	QCOMPARE(tag.typedText(), QString("tag_text"));
}

QTEST_APPLESS_MAIN(TestTag)

#include "tst_tag.moc"
