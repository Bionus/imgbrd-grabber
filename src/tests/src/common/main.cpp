#define CATCH_CONFIG_RUNNER

#ifdef HEADLESS
	#include <QCoreApplication>
#else
	#include <QApplication>
#endif
#include "functions.h"
#include "vendor/catch/single_include/catch2/catch.hpp"


int main(int argc, char* argv[])
{
	#ifdef HEADLESS
		QCoreApplication app(argc, argv);
	#else
		QApplication app(argc, argv);
	#endif

	// Used for networking and finding test resource files
	setTestModeEnabled(true);

	return Catch::Session().run(argc, argv);
}
