#include "reverse-search-loader.h"


ReverseSearchLoader::ReverseSearchLoader(QSettings *settings)
	: m_settings(settings)
{}

QList<ReverseSearchEngine> ReverseSearchLoader::getAllReverseSearchEngines() const
{
	QList<ReverseSearchEngine> ret;

	ret.append(ReverseSearchEngine(QIcon(":/images/reverse-search/iqdb.png"), "IQDB", "https://iqdb.org/?url={url}"));
	ret.append(ReverseSearchEngine(QIcon(":/images/reverse-search/saucenao.png"), "SauceNAO", "https://saucenao.com/search.php?db=999&url={url}"));
	ret.append(ReverseSearchEngine(QIcon(":/images/reverse-search/google.png"), "Google", "https://www.google.com/searchbyimage?image_url={url}"));
	ret.append(ReverseSearchEngine(QIcon(":/images/reverse-search/tineye.png"), "TinEye", "https://www.tineye.com/search/?url={url}"));
	ret.append(ReverseSearchEngine(QIcon(":/images/reverse-search/yandex.png"), "Yandex", "https://yandex.ru/images/search?rpt=imageview&img_url={url}"));

	return ret;
}
