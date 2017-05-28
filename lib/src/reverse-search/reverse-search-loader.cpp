#include "reverse-search-loader.h"
#include "reverse-search-engine.h"
#include "functions.h"


ReverseSearchLoader::ReverseSearchLoader(QSettings *settings)
	: m_settings(settings)
{}

QMap<int, ReverseSearchEngine> ReverseSearchLoader::getAllReverseSearchEngines() const
{
	QMap<int, ReverseSearchEngine> ret;

	// Default groups
	if (!m_settings->childGroups().contains("WebServices"))
	{
		ret.insert(1, ReverseSearchEngine(savePath("webservices/1.ico"), "IQDB", "https://iqdb.org/?url={url}"));
		ret.insert(2, ReverseSearchEngine(savePath("webservices/2.ico"), "SauceNAO", "https://saucenao.com/search.php?db=999&url={url}"));
		ret.insert(3, ReverseSearchEngine(savePath("webservices/3.ico"), "Google", "https://www.google.com/searchbyimage?image_url={url}"));
		ret.insert(4, ReverseSearchEngine(savePath("webservices/4.ico"), "TinEye", "https://www.tineye.com/search/?url={url}"));
		ret.insert(5, ReverseSearchEngine(savePath("webservices/5.ico"), "Yandex", "https://yandex.ru/images/search?rpt=imageview&img_url={url}"));
		ret.insert(6, ReverseSearchEngine(savePath("webservices/6.ico"), "waifu2x", "http://waifu2x.udp.jp/index.html?url={url}"));
	}

	// Load groups
	m_settings->beginGroup("WebServices");
	for (QString group : m_settings->childGroups())
	{
		m_settings->beginGroup(group);
		ret.insert(group.toInt(), ReverseSearchEngine(savePath("webservices/" + group + ".ico"), m_settings->value("name").toString(), m_settings->value("url").toString()));
		m_settings->endGroup();
	}
	m_settings->endGroup();

	return ret;
}
