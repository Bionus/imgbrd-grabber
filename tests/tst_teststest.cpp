#include <QtTest>
#include <QString>

class TestsTest : public QObject
{
		Q_OBJECT

	public:
		TestsTest();

	private Q_SLOTS:
		void testCase1();
};

TestsTest::TestsTest()
{
}

void TestsTest::testCase1()
{
	Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

	QCOMPARE(tag.text(), "tag_text");
}

QTEST_APPLESS_MAIN(TestsTest)

#include "tst_teststest.moc"
