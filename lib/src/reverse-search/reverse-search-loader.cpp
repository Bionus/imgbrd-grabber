#include "reverse-search/reverse-search-loader.h"
#include "reverse-search/reverse-search-engine.h"
#include "functions.h"


ReverseSearchLoader::ReverseSearchLoader(QSettings *settings)
	: m_settings(settings)
{}

QList<ReverseSearchEngine> ReverseSearchLoader::getAllReverseSearchEngines() const
{
	QMap<int, ReverseSearchEngine> ret;

	// Default groups
	if (!m_settings->childGroups().contains("WebServices"))
	{
		ret.insert(1, ReverseSearchEngine(1, savePath("webservices/1.ico"), "IQDB", "https://iqdb.org/?url={url}", 1));
		ret.insert(2, ReverseSearchEngine(2, savePath("webservices/2.ico"), "SauceNAO", "https://saucenao.com/search.php?db=999&url={url}", 2));
		ret.insert(3, ReverseSearchEngine(3, savePath("webservices/3.ico"), "Google", "https://www.google.com/searchbyimage?image_url={url}", 3));
		ret.insert(4, ReverseSearchEngine(4, savePath("webservices/4.ico"), "TinEye", "https://www.tineye.com/search/?url={url}", 4));
		ret.insert(5, ReverseSearchEngine(5, savePath("webservices/5.ico"), "Yandex", "https://yandex.ru/images/search?rpt=imageview&img_url={url}", 5));
		ret.insert(6, ReverseSearchEngine(6, savePath("webservices/6.ico"), "waifu2x", "http://waifu2x.udp.jp/index.html?url={url}", 6));
	}

	// Load groups
	m_settings->beginGroup("WebServices");
	for (const QString &group : m_settings->childGroups())
	{
		m_settings->beginGroup(group);
		int id = group.toInt();
		int order = m_settings->value("order").toInt();
		ret.insert(order, ReverseSearchEngine(id, savePath("webservices/" + group + ".ico"), m_settings->value("name").toString(), m_settings->value("url").toString(), order));
		m_settings->endGroup();
	}
	m_settings->endGroup();

	return ret.values();
}
