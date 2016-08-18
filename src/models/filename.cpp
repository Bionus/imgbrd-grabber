#include <QtScript>
#include "filename.h"
#include "functions.h"
#include "site.h"


typedef QPair<QString,QString> QStrP;

Filename::Filename(QString format)
	: m_format(format)
{ }

QString Filename::expandConditionals(QString text, QStringList tokens, QStringList tags, int depth)
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
			ret.replace("<" + cap + ">", this->expandConditionals(cap, tokens, tags, depth + 1));
		}
		pos += reg.matchedLength() + cap.count('<') - cap.count('>');
	}

	if (depth > 0)
	{
		QString r = ret;
		for (int i = 0; i < tokens.size(); ++i)
		{ r.replace(QRegExp("%"+tokens.at(i)+"(?::([0-9]+))?%"), ""); }
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

QList<QStrP> Filename::getReplace(QString setting, QMap<QString,QStringList> details, QSettings *settings)
{
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
			first = QStringList(details[setting+"s"].mid(0, qMin(1, keepN))).join(separator);
		}
		else if (whatToDo == "keepNThenAdd")
		{
			int keepN = settings->value(setting+"_multiple_keepNThenAdd_keep", 1).toInt();
			QString thenAdd = settings->value(setting+"_multiple_keepNThenAdd_add", " (+ %count%)").toString();
			thenAdd.replace("%total%", QString::number(details[setting+"s"].size()));
			thenAdd.replace("%count%", QString::number(details[setting+"s"].size() - keepN));
			first = QStringList(details[setting+"s"].mid(0, qMin(1, keepN))).join(separator) + thenAdd;
		}
		else
		{ first = settings->value(setting+"_value").toString(); }
	}
	else
	{ first = first = details[setting+"s"].join(separator); }

	ret.append(QStrP(first, second));
	return ret;
}

/**
 * Return the filename of the image according to the user's settings.
 * @param fn The user's filename.
 * @param pth The user's root save path.
 * @param counter Current image count (used for batch downloads).
 * @param complex Whether the filename is complex or not (contains conditionals).
 * @return The filename of the image, with any token replaced.
 */
QStringList Filename::path(Image &img, QSettings *settings, QString pth, int counter, bool complex, bool maxlength, bool shouldFixFilename, bool getFull)
{
	QStringList ignore = loadIgnored();
	QStringList remove = settings->value("ignoredtags").toString().split(' ', QString::SkipEmptyParts);
	settings->beginGroup("Save");

	QStringList copyrights;
	QString cop;
	bool found;
	QMap<QString,QStringList> custom = QMap<QString,QStringList>(), scustom = getCustoms();
	QMap<QString,QStringList> details;
	QRegExp reg;
	reg.setCaseSensitivity(Qt::CaseInsensitive);
	reg.setPatternSyntax(QRegExp::Wildcard);
	for (Tag tag : img.tags())
	{
		QString t = tag.text();
		bool removed = false;
		for (int j = 0; j < remove.size(); ++j)
		{
			reg.setPattern(remove.at(j));
			if (reg.exactMatch(t))
			{ removed = true; }
		}
		if (!removed)
		{
			for (int r = 0; r < scustom.size(); ++r)
			{
				QString key = scustom.keys().at(r);
				if (!custom.contains(key))
				{ custom.insert(key, QStringList()); }
				if (scustom[key].contains(t, Qt::CaseInsensitive))
				{ custom[key].append(t); }
			}
			details[ignore.contains(tag.text(), Qt::CaseInsensitive) ? "generals" : tag.type()+"s"].append(t);
			details["alls"].append(t);

			QString underscored = QString(t);
			underscored.replace(' ', '_');
			details["allos"].append(underscored);
		}
	}
	if (settings->value("copyright_useshorter", true).toBool())
	{
		for (QString cop : details["copyrights"])
		{
			found = false;
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
	}
	else
	{ copyrights = details["copyrights"]; }

	QString ext = getExtension(img.url());

	QString tagSeparator = settings->value("separator").toString();
	QRegularExpression poolRegexp("pool:(\\d+)");
	QRegularExpressionMatch poolMatch = poolRegexp.match(img.search().join(tagSeparator));

	QMap<QString,QStrP> replaces = QMap<QString,QStrP>();
	replaces.insert("ext", QStrP(ext, "jpg"));
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
	replaces.insert("pool", QStrP(poolMatch.hasMatch() ? poolMatch.captured(1) : "", ""));
	replaces.insert("rating", QStrP(img.rating(), "unknown"));
	replaces.insert("score", QStrP(QString::number(img.score()), ""));
	replaces.insert("height", QStrP(QString::number(img.size().height()), "0"));
	replaces.insert("width", QStrP(QString::number(img.size().width()), "0"));
	replaces.insert("general", QStrP(details["generals"].join(tagSeparator), ""));
	replaces.insert("allo", QStrP(details["allos"].join(" "), ""));
	replaces.insert("all", QStrP(details["alls"].join(tagSeparator), ""));
	for (int i = 0; i < custom.size(); ++i)
	{ replaces.insert(custom.keys().at(i), QStrP(custom.values().at(i).join(tagSeparator), "")); }
	replaces.insert("url_file", QStrP(img.url(), ""));
	replaces.insert("url_page", QStrP(img.pageUrl().toString(), ""));

	// Filename
	QString filename = m_format;
	QMap<QString,QPair<QString,QString>> filenames = getFilenames();
	for (int i = 0; i < filenames.size(); ++i)
	{
		QString cond = filenames.keys().at(i);
		QStringList options = cond.split(' ');

		int condPer = cond.count("%");
		if (condPer > 0 && condPer % 2 == 0)
		{
			QRegExp reg("%([^%]+)%");
			reg.setMinimal(true);
			int p = 0;
			while ((p = reg.indexIn(cond, p)) != -1)
			{
				QString token = reg.cap(1);
				if (replaces.contains(token))
				{
					options.removeOne(reg.cap(0));
					if (!replaces[token].first.isEmpty())
					{ options.append(replaces[token].first.split(' ')); }
				}
				p += reg.matchedLength();
			}
		}

		for (QString opt : options)
		{
			if (details["alls"].contains(opt))
			{
				if (!filenames.value(cond).first.isEmpty())
				{ filename = filenames.value(cond).first; }
				if (!filenames.value(cond).second.isEmpty())
				{ pth = filenames.value(cond).second; }
				break;
			}
		}
	}

	// Remove duplicates in %all%
	QStringList rem = (filename.contains("%artist%") ? details["artists"] : QStringList()) +
		(filename.contains("%copyright%") ? copyrights : QStringList()) +
		(filename.contains("%character%") ? details["characters"] : QStringList()) +
		(filename.contains("%model%") ? details["models"] : QStringList()) +
		(filename.contains("%general%") ? details["generals"] : QStringList());
	QStringList l = details["alls"];
	for (int i = 0; i < rem.size(); ++i)
	{ l.removeAll(rem.at(i)); }
	replaces.insert("all", QStrP(l.join(tagSeparator), ""));

	if (filename.startsWith("javascript:"))
	{
		// We remove the "javascript:" part
		filename = filename.right(filename.length() - 11);

		// FIXME real multiple filename management shared with normal filenames
		QStringList repKays = QStringList() << "artist" << "copyright" << "character" << "model";
		for (QString key : repKays)
		{
			QList<QStrP> repls = this->getReplace(key, details, settings);
			replaces.insert(key, repls.first());
		}
		// end FIXME

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
				if (!settings->value("replaceblanks", false).toBool())
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

		filename = result.toString();
	}
	else
	{
		// We get path and remove useless slashes from filename
		pth.replace("\\", "/");
		filename.replace("\\", "/");
		if (filename.left(1) == "/")	{ filename = filename.right(filename.length()-1);	}
		if (pth.right(1) == "/")		{ pth = pth.left(pth.length()-1);					}

		// Conditionals
		if (complex)
		{
			QStringList tokens = QStringList() << "artist" << "general" << "copyright" << "character" << "model" << "model|artist" << "filename" << "rating" << "md5" << "website" << "ext" << "all" << "id" << "search" << "allo" << "date" << "date:([^%]+)" << "count(:\\d+)?(:\\d+)?" << "search_(\\d+)" << "score" << "height" << "width" << "path" << "pool" << "url_file" << "url_page" << custom.keys();
			filename = this->expandConditionals(filename, tokens, details["allos"]);
		}

		// Replace everything
		QRegExp replacerx("%([^:]+)(?::([^%]+))?%");
		replacerx.setMinimal(true);
		int p = 0;
		while ((p = replacerx.indexIn(filename, p)) != -1)
		{
			QString key = replacerx.cap(1);
			if (replaces.contains(key))
			{
				QMap<QString,QString> options;
				if (replacerx.captureCount() > 1)
				{
					QStringList opts = replacerx.cap(2).split(',');
					for (QString opt : opts)
					{
						int index = opt.indexOf('=');
						if (index != -1)
						{ options.insert(opt.left(index), opt.mid(index + 1)); }
						else
						{ options.insert(opt, "true"); }
					}
				}

				QString res = replaces[key].first.isEmpty() ? replaces[key].second : replaces[key].first;
				if (options.contains("maxlength"))
				{ res = res.left(options["maxlength"].toInt()); }

				if (key != "allo" && key != "url_file" && key != "url_page")
				{
					res = res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed();
					if (!settings->value("replaceblanks", false).toBool())
					{ res.replace("_", " "); }
				}

				filename.replace(replacerx.cap(0), res);
			}
			else
			{ p += replacerx.matchedLength(); }
		}

		// Complex expressions using regexes
		QRegExp rxdate("%date:([^%]+)%");
		rxdate.setMinimal(true);
		p = 0;
		while ((p = rxdate.indexIn(filename, p)) != -1)
		{ filename.replace(rxdate.cap(0), img.createdAt().toString(rxdate.cap(1))); }
		QRegExp rxcounter("%count(:\\d+)?(:\\d+)?%");
		rxcounter.setMinimal(true);
		p = 0;
		while ((p = rxcounter.indexIn(filename, p)) != -1)
		{ filename.replace(rxcounter.cap(0), rxcounter.captureCount() > 0 ? QString::number(counter, 'f', rxcounter.cap(1).toInt()) : QString::number(counter)); }
	}

	QStringList fns = QStringList() << filename;
	QStringList keys = QStringList() << "artist" << "copyright" << "character" << "model";

	for (QString key : keys)
	{
		QList<QStrP> replaces = this->getReplace(key, details, settings);
		int cnt = fns.count();
		for (int i = 0; i < cnt; ++i)
		{
			if (fns[i].contains("%"+key+"%"))
			{
				for (int j = 0; j < replaces.count(); ++j)
				{
					QString res = replaces[j].first.isEmpty() ? replaces[j].second : replaces[j].first;
					res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed();
					if (!settings->value("replaceblanks", false).toBool())
					{ res.replace("_", " "); }

					QString filename = QString(fns[i]);
					filename = filename.replace("%"+key+"%", res);

					if (j < replaces.count() - 1)
						fns.append(filename);
					else
						fns[i] = filename;
				}
			}
		}
	}

	int cnt = fns.count();
	for (int i = 0; i < cnt; ++i)
	{
		QString filename = fns[i];

		// Trim directory names
		filename = filename.trimmed();
		filename.replace(QRegExp(" */ *"), "/");

		// We remove empty directory names
		while (filename.indexOf("//") >= 0)
		{ filename.replace("//", "/"); }

		// Max filename size option
		if (shouldFixFilename)
		{
			int limit = !maxlength ? 0 : settings->value("limit").toInt();
			fns[i] = fixFilename(filename, pth, limit);
		}

		if (getFull)
		{
			fns[i] = QDir::toNativeSeparators(fns[i]);
			if (fns[i].left(1) == QDir::toNativeSeparators("/"))
			{ fns[i] = fns[i].right(fns[i].length() - 1); }
			if (fns[i].right(1) == QDir::toNativeSeparators("/"))
			{ fns[i] = fns[i].left(fns[i].length() - 1); }
			fns[i] = QDir::toNativeSeparators(pth + "/" + fns[i]);
		}
	}

	return fns;
}

QString Filename::getFormat()
{
	return m_format;
}
