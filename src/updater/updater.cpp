#include "updater.h"


int Updater::versionToInt(QString version)
{
	version = version.replace(".", "");
	int beta = version.endsWith("a") ? 1 : (version.endsWith("b") ? 2 : 9);
	return version.replace("a", "").replace("b", "").toInt() * 10 + beta;
}
