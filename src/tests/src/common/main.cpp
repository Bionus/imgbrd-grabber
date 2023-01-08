#ifdef HEADLESS
	#include <QCoreApplication>
#else
	#include <QApplication>
#endif
#include "functions.h"
#include "vendor/catch/src/catch2/catch_all.hpp"


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
