#include "auth-field-test.h"
#include <QSettings>
#include <QtTest>
#include "auth/auth-const-field.h"
#include "auth/auth-field.h"
#include "auth/auth-hash-field.h"
#include "mixed-settings.h"


MixedSettings *makeSettings(QString key, QString value)
{
	QSettings *settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);
	settings->setValue(key, value);

	return new MixedSettings(QList<QSettings*>() << settings);
}


void AuthFieldTest::testBasic()
{
	AuthField field("key", AuthField::FieldType::Username);

	QCOMPARE(field.key(), QString("key"));
	QCOMPARE(field.type(), AuthField::FieldType::Username);

	MixedSettings *settings = makeSettings("auth/pseudo", "user");
	QCOMPARE(field.value(settings), QString("user"));
	settings->deleteLater();
}

void AuthFieldTest::testConst()
{
	AuthConstField field("key", "val");

	QCOMPARE(field.key(), QString("key"));
	QCOMPARE(field.type(), AuthField::FieldType::Const);

	MixedSettings *settings = new MixedSettings(QList<QSettings*>());
	QCOMPARE(field.value(settings), QString("val"));
	settings->deleteLater();
}

void AuthFieldTest::testHash()
{
	AuthHashField field("key", QCryptographicHash::Algorithm::Md5, "test-%username%");

	QCOMPARE(field.key(), QString("key"));
	QCOMPARE(field.type(), AuthField::FieldType::Hash);
	QCOMPARE(field.salt(), QString("test-%username%"));

	MixedSettings *settings = makeSettings("auth/pseudo", "user");
	QCOMPARE(field.value(settings), QString("42b27efc1480b4fe6d7eaa5eec47424d")); // md5("test-user")
	settings->deleteLater();
}


QTEST_MAIN(AuthFieldTest)
