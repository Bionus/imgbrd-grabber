#include <QSettings>
#include "mixed-settings.h"
#include "catch.h"


TEST_CASE("MixedSettings")
{
	auto child = new QSettings("tests/resources/tmp/child.ini", QSettings::IniFormat);
	auto parent = new QSettings("tests/resources/tmp/parent.ini", QSettings::IniFormat);

	child->clear();
	parent->clear();

	SECTION("EmptySettings")
	{
		MixedSettings settings((QList<QSettings*>()));

		// Those calls shouldn't do anything, but shouldn't throw either
		settings.setValue("test", "val");
		REQUIRE(settings.value("test", "none").toString() == QString("none"));
		REQUIRE(settings.childKeys() == QStringList());
		settings.beginGroup("group");
		REQUIRE(settings.childKeys() == QStringList());
		settings.endGroup();
		settings.sync();
	}

	SECTION("ValueFirstValid")
	{
		MixedSettings settings(QList<QSettings*>() << child << parent);

		child->setValue("test", "child");
		parent->setValue("test", "parent");
		QVariant v1 = child->value("test");
		QVariant v2 = child->value("test");

		REQUIRE(settings.value("test").toString() == QString("child"));
	}

	SECTION("ValueDefault")
	{
		MixedSettings settings(QList<QSettings*>() << child << parent);

		REQUIRE(settings.value("test", "default").toString() == QString("default"));
	}

	SECTION("SetValueResetToParent")
	{
		MixedSettings settings(QList<QSettings*>() << child << parent);

		child->setValue("test", "child");
		parent->setValue("test", "parent");

		settings.setValue("test", "parent", "default");
		settings.sync();

		REQUIRE(child->value("test", "none").toString() == QString("none"));
		REQUIRE(parent->value("test", "none").toString() == QString("parent"));
	}

	SECTION("SetValueResetToDefault")
	{
		MixedSettings settings(QList<QSettings*>() << child << parent);

		child->setValue("test", "child");

		settings.setValue("test", "default", "default");
		settings.sync();

		REQUIRE(child->value("test", "none").toString() == QString("none"));
		REQUIRE(parent->value("test", "none").toString() == QString("none"));
	}

	SECTION("SetValueOverrideParent")
	{
		MixedSettings settings(QList<QSettings*>() << child << parent);

		parent->setValue("test", "parent");

		settings.setValue("test", "child", "default");
		settings.sync();

		REQUIRE(child->value("test", "none").toString() == QString("child"));
		REQUIRE(parent->value("test", "none").toString() == QString("parent"));
	}

	SECTION("SetValueOverrideDefault")
	{
		MixedSettings settings(QList<QSettings*>() << child << parent);

		settings.setValue("test", "child", "default");
		settings.sync();

		REQUIRE(child->value("test", "none").toString() == QString("child"));
		REQUIRE(parent->value("test", "none").toString() == QString("none"));
	}

	SECTION("ChildKeys")
	{
		MixedSettings settings(QList<QSettings*>() << child << parent);

		child->setValue("child", "value");
		parent->setValue("parent", "value");

		REQUIRE(settings.childKeys() == QStringList() << "child" << "parent");
	}

	SECTION("ChildKeysInGroup")
	{
		MixedSettings settings(QList<QSettings*>() << child << parent);

		child->setValue("other", "value");
		child->setValue("Group/child", "value");
		parent->setValue("Group/parent", "value");

		settings.beginGroup("Group");
		REQUIRE(settings.childKeys() == QStringList() << "child" << "parent");
		settings.endGroup();
	}
}
