#include <QApplication>
#include <QTabBar>
#include <QWidget>
#include "catch.h"
#include "ui/qclosabletabwidget.h"


TEST_CASE("QClosableTabWidget")
{
	QClosableTabWidget tabWidget(nullptr);
	tabWidget.resize(700, 300);
	for (int i = 0; i < 3; ++i) {
		tabWidget.addTab(new QWidget(&tabWidget), QStringLiteral("A long tab title %1").arg(i));
	}

	tabWidget.show();
	QApplication::processEvents();

	auto *tabBar = tabWidget.findChild<QTabBar*>();
	REQUIRE(tabBar != nullptr);
	REQUIRE(tabBar->elideMode() == Qt::ElideRight);
	REQUIRE(tabBar->usesScrollButtons());

	const int wideTabWidth = tabBar->tabRect(0).width();
	REQUIRE(wideTabWidth <= 200);

	tabWidget.resize(300, 300);
	QApplication::processEvents();
	const int narrowTabWidth = tabBar->tabRect(0).width();

	REQUIRE(narrowTabWidth >= 72);
	REQUIRE(narrowTabWidth < wideTabWidth);
}
