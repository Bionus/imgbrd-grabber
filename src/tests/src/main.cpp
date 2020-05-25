#define CATCH_CONFIG_RUNNER

#ifdef HEADLESS
	#include <QCoreApplication>
#else
	#include <QGuiApplication>
#endif
#include "functions.h"
#include "vendor/catch/single_include/catch2/catch.hpp"


int main(int argc, char* argv[])
{
	#ifdef HEADLESS
		QCoreApplication app(argc, argv);
	#else
		QGuiApplication app(argc, argv);
	#endif

	// Used for networking and finding test resource files
	setTestModeEnabled(true);

	return Catch::Session().run(argc, argv);
}
