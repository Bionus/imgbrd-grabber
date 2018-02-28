#include <QTest>
#include <iostream>
#include "custom-network-access-manager.h"
#include "functions.h"
#include "test-suite.h"
#define SOURCE_DIR "release/sites/Danbooru (2.0)"
#define RESULTS_FILE "tests/resources/pages/danbooru.donmai.us/results."

#include "models/site.h"
#include "models/page.h"
#include "models/api/javascript-api.h"
#include "models/api/html-api.h"
int main(int argc, char *argv[])
{
	#ifdef HEADLESS
		QCoreApplication a(argc, argv);
	#else
		QGuiApplication a(argc, argv);
	#endif

	/*QString search = "rating:safe";
	int pagei = 1;
	int limit = 5;

	double ops = 1000;
	QStringList apis = QStringList() << "Html" << "Json" << "Xml";
	for (int enableJsModels = 0; enableJsModels <= 1; ++enableJsModels)
	{
		Profile *profile = new Profile("tests");
		profile->getSettings()->setValue("enableJsModels", (bool)enableJsModels);
		Source *source = new Source(profile, SOURCE_DIR);
		Site *site = source->getSites().first();
		Page *page = new Page(profile, site, profile->getSites().values(), QStringList() << search, pagei, limit);

		for (const QString &apiName : apis)
		{
			Api *api = Q_NULLPTR;
			for (Api *a : source->getApis())
				if (a->getName() == apiName)
					api = a;
			if (api == Q_NULLPTR)
				return 1;

			QFile f(RESULTS_FILE + apiName.toLower());
			if (!f.open(QFile::ReadOnly | QFile::Text))
				return 2;
			QString src = f.readAll();
			f.close();

			QTime myTimer;
			myTimer.start();
			for (int i = 0; i < ops; ++i)
			{
				PageUrl url = api->pageUrl(search, pagei, limit, -1, -1, -1, site);
				ParsedPage ret = api->parsePage(page, src, 0, limit);
			}

			int elapsed = myTimer.elapsed();
			qDebug() << (enableJsModels == 1 ? "JS" : "C++") << apiName << "elapsed" << elapsed << "per op" << (elapsed / ops) << "per sec" << (1000 * ops / elapsed);
		}
	}

	return 0;*/

	QStringList testSuites;
	testSuites.reserve(argc - 1);
	for (int i = 1; i < argc; ++i)
		testSuites.append(argv[i]);

	QMap<QString, int> results;
	int failed = 0;

	setTestModeEnabled(true);

	for (TestSuite *suite : TestSuite::getSuites())
	{
		if (!testSuites.isEmpty() && !testSuites.contains(suite->metaObject()->className()))
			continue;

		int result = QTest::qExec(suite);
		results.insert(suite->metaObject()->className(), result);
		if (result != 0)
		{
			failed++;
		}
	}

	for (auto it = results.begin(); it != results.end(); ++it)
	{
		std::cout << '[' << (it.value() != 0 ? "FAIL" : "OK") << "] " << it.key().toStdString() << std::endl;
	}

	return failed;
}
