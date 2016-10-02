#include <QtScript>
#include "filename.h"
#include "functions.h"
#include "site.h"


typedef QPair<QString,QString> QStrP;

Filename::Filename()
{ }
Filename::Filename(QString format)
	: m_format(format)
{ }

QList<QMap<QString, QPair<QString, QString>>> Filename::getReplaces(QString filename, const Image &img, Profile *profile, QMap<QString, QStringList> custom) const
{
	QMap<QString, QPair<QString, QString>> replaces;
	QList<QMap<QString, QPair<QString, QString>>> ret;
	QSettings *settings = profile->getSettings();

	QString tagSeparator = settings->value("Save/separator", " ").toString();
	QMap<QString, QStringList> details = makeDetails(img, profile, settings);

	// Pool
	QRegularExpression poolRegexp("pool:(\\d+)");
	QRegularExpressionMatch poolMatch = poolRegexp.match(img.search().join(tagSeparator));
	replaces.insert("pool", QStrP(poolMatch.hasMatch() ? poolMatch.captured(1) : "", ""));

	// Basic shared values
	replaces.insert("filename", QStrP(QUrl::fromPercentEncoding(img.url().section('/', -1).section('.', 0, -2).toUtf8()), ""));
	replaces.insert("website", QStrP(img.parentSite()->url(), ""));
	replaces.insert("websitename", QStrP(img.parentSite()->name(), ""));
	replaces.insert("md5", QStrP(img.md5(), ""));
	replaces.insert("date", QStrP(img.createdAt().toString(QObject::tr("dd-MM-yyyy HH.mm")), ""));
	replaces.insert("id", QStrP(QString::number(img.id()), "0"));
	QStringList search = img.search();
	for (int i = 0; i < search.size(); ++i)
	{ replaces.insert("search_"+QString::number(i+1), QStrP(search[i], "")); }
	for (int i = search.size(); i < 10; ++i)
	{ replaces.insert("search_"+QString::number(i+1), QStrP("", "")); }
	replaces.insert("search", QStrP(search.join(tagSeparator), ""));
	replaces.insert("rating", QStrP(img.rating(), "unknown"));
	replaces.insert("score", QStrP(QString::number(img.score()), ""));
	replaces.insert("height", QStrP(QString::number(img.size().height()), "0"));
	replaces.insert("width", QStrP(QString::number(img.size().width()), "0"));
	replaces.insert("general", QStrP(details["generals"].join(tagSeparator), ""));
	replaces.insert("allo", QStrP(details["allos"].join(" "), ""));
	replaces.insert("tags", QStrP(details["alls"].join(tagSeparator), ""));
	replaces.insert("all", QStrP(details["alls"].join(tagSeparator), ""));
	for (int i = 0; i < custom.size(); ++i)
	{ replaces.insert(custom.keys().at(i), QStrP(custom.values().at(i).join(tagSeparator), "")); }
	replaces.insert("url_file", QStrP(img.url(), ""));
	replaces.insert("url_page", QStrP(img.pageUrl().toString(), ""));

	// JPEG extension
	QString ext = getExtension(img.url());
	if (settings->value("Save/noJpeg", true).toBool() && ext == "jpeg")
		ext = "jpg";
	replaces.insert("ext", QStrP(ext, "jpg"));

	// Remove duplicates in %all%
	QStringList rem = (filename.contains("%artist%") ? details["artists"] : QStringList()) +
		(filename.contains("%copyright%") ? details["copyrights"] : QStringList()) +
		(filename.contains("%character%") ? details["characters"] : QStringList()) +
		(filename.contains("%model%") ? details["models"] : QStringList()) +
		(filename.contains("%general%") ? details["generals"] : QStringList());
	QStringList l = details["alls"];
	QStringList namespaces = details["alls_namespaces"];
	for (int i = 0; i < rem.size(); ++i)
	{
		int index = l.indexOf(rem.at(i));
		l.removeAt(index);
		namespaces.removeAt(index);
	}
	replaces.insert("all", QStrP(l.join(tagSeparator), ""));
	replaces.insert("all_namespaces", QStrP(namespaces.join(" "), ""));

	ret.append(replaces);
	QStringList keys = QStringList() << "artist" << "copyright" << "character" << "model";
	for (QString key : keys)
	{
		if (filename.contains(QRegExp("%"+key+"(?::[^%]+)?%")))
		{
			QList<QPair<QString, QString>> reps = this->getReplace(key, details, settings);
			int cnt = ret.count();
			for (int i = 0; i < cnt; ++i)
			{
				for (int j = 0; j < reps.count(); ++j)
				{
					QPair<QString, QString> pair = reps[j];
					if (j == 0)
					{
						ret[i].insert(key, pair);
					}
					else
					{
						replaces = ret[i];
						replaces.insert(key, pair);
						ret.append(replaces);
					}
				}
			}
		}
	}

	return ret;
}

QString Filename::expandConditionals(QString text, QStringList tokens, QStringList tags, QMap<QString, QPair<QString, QString>> replaces, int depth) const
{
	QString ret = text;

	QRegExp reg = QRegExp("\\<([^>]+)\\>");
	int pos = 0;
	while ((pos = reg.indexIn(text, pos)) != -1)
	{
		QString cap = reg.cap(1);
		if (!cap.isEmpty())
		{
			cap += QString(">").repeated(cap.count('<') - cap.count('>'));
			ret.replace("<" + cap + ">", this->expandConditionals(cap, tokens, tags, replaces, depth + 1));
		}
		pos += reg.matchedLength() + cap.count('<') - cap.count('>');
	}

	if (depth > 0)
	{
		QString r = ret;
		for (QString token : tokens)
		{
			if (replaces.contains(token) && !replaces[token].first.isEmpty())
			{ r.replace(QRegExp("%"+token+"(?::[^%]+)?%"), ""); }
		}
		if (r.contains(QRegExp("%[^:%]+(?::[^%]+)?%")))
		{ return ""; }

		reg = QRegExp("\"([^\"]+)\"");
		pos = 0;
		while ((pos = reg.indexIn(text, pos)) != -1)
		{
			if (!reg.cap(1).isEmpty() && tags.contains(reg.cap(1)))
			{ ret.replace(reg.cap(0), reg.cap(1)); }
			else
			{ return ""; }
			pos += reg.matchedLength();
		}
	}

	return ret;
}

QList<QPair<QString,QString>> Filename::getReplace(QString setting, QMap<QString,QStringList> details, QSettings *settings) const
{
	settings->beginGroup("Save");

	QList<QStrP> ret;
	QString first = "";
	QString second = settings->value(setting+"_empty").toString();

	int limit = settings->value(setting+"_multiple_limit", 1).toInt();
	QString separator = settings->value(setting+"_sep", " ").toString();

	if (details[setting+"s"].size() > limit)
	{
		QString whatToDo = settings->value(setting+"_multiple", "replaceAll").toString();
		if (whatToDo == "keepAll")
		{ first = details[setting+"s"].join(separator); }
		else if (whatToDo == "multiple")
		{
			int i;
			for (i = 0; i < details[setting+"s"].count() - 1; ++i)
			{ ret.append(QStrP(details[setting+"s"][i], second)); }
			first = details[setting+"s"][i];
		}
		else if (whatToDo == "keepN")
		{
			int keepN = settings->value(setting+"_multiple_keepN", 1).toInt();
			first = QStringList(details[setting+"s"].mid(0, qMax(1, keepN))).join(separator);
		}
		else if (whatToDo == "keepNThenAdd")
		{
			int keepN = settings->value(setting+"_multiple_keepNThenAdd_keep", 1).toInt();
			QString thenAdd = settings->value(setting+"_multiple_keepNThenAdd_add", " (+ %count%)").toString();
			thenAdd.replace("%total%", QString::number(details[setting+"s"].size()));
			thenAdd.replace("%count%", QString::number(details[setting+"s"].size() - keepN));
			first = QStringList(details[setting+"s"].mid(0, qMax(1, keepN))).join(separator) + (details[setting+"s"].size() > keepN ? thenAdd : "");
		}
		else
		{ first = settings->value(setting+"_value").toString(); }
	}
	else
	{ first = first = details[setting+"s"].join(separator); }

	ret.append(QStrP(first, second));
	settings->endGroup();
	return ret;
}

QMap<QString, QStringList> Filename::makeDetails(const Image& img, Profile *profile, QSettings *settings) const
{
	QStringList ignore = profile->getIgnored();
	QStringList remove = settings->value("ignoredtags").toString().split(' ', QString::SkipEmptyParts);

	QMap<QString, QStringList> details;
	QRegExp reg;
	reg.setCaseSensitivity(Qt::CaseInsensitive);
	reg.setPatternSyntax(QRegExp::Wildcard);
	for (Tag tag : img.filteredTags(remove))
	{
		QString t = tag.text();

		details[ignore.contains(t, Qt::CaseInsensitive) ? "generals" : tag.type()+"s"].append(t);
		details["alls"].append(t);
		details["alls_namespaces"].append(tag.type());

		QString underscored = QString(t);
		underscored.replace(' ', '_');
		details["allos"].append(underscored);
	}

	// Actual copyrights, shortened if necessary
	if (settings->value("Save/copyright_useshorter", true).toBool())
	{
		QStringList copyrights;
		for (QString cop : details["copyrights"])
		{
			bool found = false;
			for (int r = 0; r < copyrights.size(); ++r)
			{
				if (copyrights.at(r).left(cop.size()) == cop.left(copyrights.at(r).size()))
				{
					if (cop.size() < copyrights.at(r).size())
					{ copyrights[r] = cop; }
					found = true;
				}
			}
			if (!found)
			{ copyrights.append(cop); }
		}
		details["copyrights"] = copyrights;
	}

	return details;
}

QStringList Filename::path(const Image& img, Profile *profile, QString pth, int counter, bool complex, bool maxlength, bool shouldFixFilename, bool getFull) const
{
	QSettings *settings = profile->getSettings();
	QStringList remove = settings->value("ignoredtags").toString().split(' ', QString::SkipEmptyParts);

	QMap<QString,QStringList> custom = QMap<QString,QStringList>(), scustom = getCustoms(settings);
	QMap<QString,QStringList> details = makeDetails(img, profile, settings);
	for (Tag tag : img.filteredTags(remove))
	{
		for (int r = 0; r < scustom.size(); ++r)
		{
			QString key = scustom.keys().at(r);
			if (!custom.contains(key))
			{ custom.insert(key, QStringList()); }
			if (scustom[key].contains(tag.text(), Qt::CaseInsensitive))
			{ custom[key].append(tag.text()); }
		}
	}

	QString filename = m_format;
	auto replacesList = this->getReplaces(filename, img, profile, custom);

	// Conditional filenames
	QMap<QString, QPair<QString, QString>> filenames = getFilenames(settings);
	for (int i = 0; i < filenames.size(); ++i)
	{
		QString cond = filenames.keys().at(i);
		QStringList options = cond.split(' ');

		int condPer = cond.count('%');
		if (condPer > 0 && condPer % 2 == 0)
		{
			QRegExp reg("%([^%]+)%");
			reg.setMinimal(true);
			int p = 0;
			while ((p = reg.indexIn(cond, p)) != -1)
			{
				QString token = reg.cap(1);
				if (replacesList.first().contains(token))
				{
					options.removeOne(reg.cap(0));

					// Real tokens
					if (details.contains(token + "s"))
					{ options.append(details[token + "s"]); }

					// Custom tokens
					if (!replacesList.first().value(token).first.isEmpty())
					{ options.append(replacesList.first().value(token).first.split(' ')); }
				}
				p += reg.matchedLength();
			}
		}

		for (QString opt : options)
		{
			if (details["alls"].contains(opt))
			{
				if (!filenames.value(cond).first.isEmpty())
				{
					filename = filenames.value(cond).first;

					// Update replaces accordingly
					replacesList = this->getReplaces(filename, img, profile, custom);
				}
				if (!filenames.value(cond).second.isEmpty())
				{ pth = filenames.value(cond).second; }
				break;
			}
		}
	}

	QStringList fns;

	if (filename.startsWith("javascript:"))
	{
		// We remove the "javascript:" part
		filename = filename.right(filename.length() - 11);

		for (auto replaces : replacesList)
		{
			// Variables initialization
			QString inits = "";
			QStringList keys = replaces.keys();
			for (int i = 0; i < replaces.size(); ++i)
			{
				QString key = keys.at(i);
				QString res = replaces[key].first.isEmpty() ? replaces[key].second : replaces[key].first;
				if (key != "allo")
				{
					res = res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed();
					if (!settings->value("Save/replaceblanks", false).toBool())
					{ res.replace("_", " "); }
				}

				if (key == "date")
				{ inits += "var " + key + " = new Date(\"" + res + "\");\r\n"; }
				else
				{ inits += "var " + key + " = \"" + res + "\";\r\n"; }
			}

			// Script execution
			QScriptEngine engine;
			QScriptValue result = engine.evaluate(QScriptProgram(inits + filename));
			if (result.isError())
			{
				error(0, QObject::tr("Erreur d'évaluation du Javascript :<br/>") + result.toString());
				return QStringList();
			}

			fns.append(result.toString());
		}
	}
	else
	{
		// We get path and remove useless slashes from filename
		pth.replace("\\", "/");
		filename.replace("\\", "/");
		if (filename.left(1) == "/")
		{ filename = filename.right(filename.length() - 1); }
		if (pth.right(1) == "/")
		{ pth = pth.left(pth.length() - 1); }

		QStringList specialTokens = QStringList() << "count";
		QStringList ignoredTokens = QStringList() << "path" << "num";

		for (auto replaces : replacesList)
		{
			QString cFilename = QString(filename);
			QString hasNum;
			QString numOptions;
			QStringList namespaces = replaces["all_namespaces"].first.split(' ');

			// Conditionals
			if (complex)
			{
				QStringList tokens = QStringList() << "tags" << "artist" << "general" << "copyright" << "character" << "model" << "model|artist" << "filename" << "rating" << "md5" << "website" << "ext" << "all" << "id" << "search" << "allo" << "date" << "count" << "search_(\\d+)" << "score" << "height" << "width" << "path" << "pool" << "url_file" << "url_page" << custom.keys();
				cFilename = this->expandConditionals(cFilename, tokens, details["allos"], replaces);
			}

			// Replace everything
			QRegExp replacerx("%([^:]+)(?::([^%]+))?%");
			replacerx.setMinimal(true);
			int p = 0;
			while ((p = replacerx.indexIn(cFilename, p)) != -1)
			{
				QString key = replacerx.cap(1);
				QString options = replacerx.captureCount() > 1 ? replacerx.cap(2) : QString();

				if (replaces.contains(key) || specialTokens.contains(key))
				{
					QString res = replaces[key].first.isEmpty() ? replaces[key].second : replaces[key].first;
					if (key == "count")
						res = QString::number(counter);

					res = optionedValue(res, key, options, img, settings, namespaces);

					cFilename.replace(replacerx.cap(0), res);
					p += res.length();
				}
				else if (ignoredTokens.contains(key))
				{
					if (key == "num")
					{
						hasNum = replacerx.cap(0);
						numOptions = options;
					}

					p += replacerx.matchedLength();
				}
				else
				{ cFilename.remove(replacerx.cap(0)); }
			}

			if (!hasNum.isEmpty())
			{
				QFileInfoList files = QDir(pth).entryInfoList(QStringList() << QString(cFilename).replace(hasNum, "*"), QDir::Files, QDir::Name);
				int num = 1;
				if (!files.isEmpty())
				{
					QString last = files.last().fileName();
					int pos = cFilename.indexOf(hasNum);
					int len = last.length() - cFilename.length() + 5;
					num = last.mid(pos, len).toInt() + 1;
				}
				cFilename.replace(hasNum, optionedValue(QString::number(num), "num", numOptions, img, settings, namespaces));
			}

			fns.append(cFilename);
		}
	}

	int cnt = fns.count();
	for (int i = 0; i < cnt; ++i)
	{
		// Trim directory names
		fns[i] = fns[i].trimmed();
		fns[i].replace(QRegExp(" */ *"), "/");

		// We remove empty directory names
		while (fns[i].indexOf("//") >= 0)
		{ fns[i].replace("//", "/"); }

		// Max filename size option
		if (shouldFixFilename)
		{
			int limit = !maxlength ? 0 : settings->value("Save/limit").toInt();
			fns[i] = fixFilename(fns[i], pth, limit);
		}

		// Include directory in result
		if (getFull)
		{ fns[i] = QDir::toNativeSeparators(pth + "/" + fns[i]); }
	}

	return fns;
}
QString Filename::optionedValue(QString res, QString key, QString ops, const Image& img, QSettings *settings, QStringList namespaces) const
{
	QString tagSeparator = settings->value("Save/separator", " ").toString();

	// Parse options
	QMap<QString,QString> options;
	if (!ops.isEmpty())
	{
		QStringList opts = ops.split(',');
		for (QString opt : opts)
		{
			int index = opt.indexOf('=');
			if (index != -1)
			{ options.insert(opt.left(index), opt.mid(index + 1)); }
			else
			{ options.insert(opt, "true"); }
		}
	}

	// Apply options
	if (key == "date" && options.contains("format"))
	{ res = img.createdAt().toString(options["format"]); }
	if (key == "count" || key == "num")
	{ res = options.contains("length") ? QString("%1").arg(res.toInt(), options["length"].toInt(), 10, QChar('0')) : res; }
	if (options.contains("maxlength"))
	{ res = res.left(options["maxlength"].toInt()); }
	if (key == "all" || key == "tags" || key == "general" || key == "artist" || key == "copyright" || key == "character")
	{
		QStringList vals = res.split(tagSeparator);
		if (options.contains("includenamespace"))
		{
			QStringList excluded;
			if (options.contains("excludenamespace"))
			{ excluded = options["excludenamespace"].split(' '); }

			QStringList namespaced;
			for (int i = 0; i < vals.count(); ++i)
			{
				QString nspace = key == "all" ? namespaces[i] : key;
				namespaced.append((!excluded.contains(nspace) ? nspace + ":" : "") + vals[i]);
			}
			vals = namespaced;
		}
		res = vals.join(options.contains("separator") ? options["separator"] : tagSeparator);
	}

	// Forbidden characters and spaces replacement settings
	if (key != "allo" && !key.startsWith("url_") && !options.contains("unsafe"))
	{
		res = res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed();
		if (!settings->value("Save/replaceblanks", false).toBool())
		{ res = res.replace("_", " "); }
	}

	return res;
}

QString Filename::getFormat() const
{
	return m_format;
}
void Filename::setFormat(QString format)
{
	m_format = format;
}

bool Filename::returnError(QString msg, QString *error) const
{
	if (error != nullptr)
		*error = msg;

	return false;
}
bool Filename::isValid(QString *error) const
{
	// Field must be filled
	if (m_format.isEmpty())
		return returnError(QObject::tr("<span style=\"color:red\">Les noms de fichiers ne doivent pas être vides !</span>"), error);

	// Can't validate javascript expressions
	if (m_format.startsWith("javascript:"))
	{
		returnError(QObject::tr("<span style=\"color:orange\">Impossible de valider les expressions Javascript.</span>"), error);
		return true;
	}

	// Field must end by an extension
	if (!m_format.endsWith(".%ext%"))
		return returnError(QObject::tr("<span style=\"color:orange\">Votre nom de fichier ne finit pas par une extension, symbolisée par %ext% ! Vous risquez de ne pas pouvoir ouvrir vos fichiers.</span>"), error);

	// Field must contain an unique token
	if (!m_format.contains("%md5%") && !m_format.contains("%id%") && !m_format.contains("%count%"))
		return returnError(QObject::tr("<span style=\"color:orange\">Votre nom de fichier n'est pas unique à chaque image et une image risque d'en écraser une précédente lors de la sauvegarde ! Vous devriez utiliser le symbole %md5%, unique à chaque image, pour éviter ce désagrément.</span>"), error);

	// Looking for unknown tokens
	QSettings *settings = new QSettings(savePath("settings.ini"));
	auto customs = getCustoms(settings);
	settings->deleteLater();
	QStringList tokens = QStringList() << "tags" << "artist" << "general" << "copyright" << "character" << "model" << "filename" << "rating" << "md5" << "website" << "ext" << "all" << "id" << "search" << "search_(\\d+)" << "allo" << customs.keys() << "date" << "score" << "count" << "width" << "height" << "pool" << "url_file" << "url_page";
	QRegExp rx("%(.+)%");
	rx.setMinimal(true);
	int pos = 0;
	while ((pos = rx.indexIn(m_format, pos)) != -1)
	{
		bool found = false;
		for (int i = 0; i < tokens.length(); i++)
		{
			if (QRegExp("%"+tokens[i]+"(?::[^%]+)?%").indexIn(rx.cap(0)) != -1)
				found = true;
		}

		if (!found)
			return returnError(QObject::tr("<span style=\"color:orange\">Le symbole %%1% n\'existe pas et ne sera pas remplacé.</span>").arg(rx.cap(1)), error);

		pos += rx.matchedLength();
	}

	// Check for invalid windows characters
	#ifdef Q_OS_WIN
		QString txt = QString(m_format).remove(rx);
		if (txt.contains(':') || txt.contains('*') || txt.contains('?') || (txt.contains('"') && txt.count('<') == 0) || txt.count('<') != txt.count('>') || txt.contains('|'))
			return returnError(QObject::tr("<span style=\"color:red\">Votre format contient des caractères interdits sur windows ! Caractères interdits : * ? \" : < > |</span>"), error);
	#endif

	// Check if code is unique
	if (!m_format.contains("%md5%") && !m_format.contains("%website%") && !m_format.contains("%count%") && m_format.contains("%id%"))
		return returnError(QObject::tr("<span style=\"color:green\">Vous avez choisi d'utiliser le symbole %id%. Sachez que celui-ci est unique pour un site choisi. Le même ID pourra identifier des images différentes en fonction du site.</span>"), error);

	// All tests passed
	returnError(QObject::tr("<span style=\"color:green\">Format valide !</span>"), error);
	return true;
}

bool Filename::needExactTags(Site *site) const
{
	return this->needExactTags(site->contains("Regex/ForceImageUrl"));
}
bool Filename::needExactTags(bool forceImageUrl) const
{
	// Javascript filenames always need tags as we don't know what they might do
	if (m_format.startsWith("javascript:"))
		return true;

	// If we need the filename and it is returned from the details page
	if (m_format.contains("%filename%") && forceImageUrl)
		return true;

	// The filename contains one of the special tags
	QStringList forbidden = QStringList() << "artist" << "copyright" << "character" << "model" << "general";
	for (QString token : forbidden)
		if (m_format.contains("%" + token + "%"))
			return true;

	return false;
}
