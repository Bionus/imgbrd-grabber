#include "filename.h"
#include <algorithm>
#include <QtScript>
#include <QCollator>
#include <QIcon>
#include "site.h"
#include "profile.h"
#include "image.h"
#include "functions.h"

#define TAGS_SEPARATOR " "


typedef QPair<QString,QString> QStrP;

Filename::Filename(QString format)
	: m_format(format)
{ }

QString Filename::expandConditionals(QString text, QStringList tags, const QMap<QString, QVariant> &tokens, QSettings *settings, int depth) const
{
	QString ret = text;

	QRegularExpression reg("\\<([^>]+)\\>");
	auto matches = reg.globalMatch(text);
	while (matches.hasNext())
	{
		auto match = matches.next();
		QString cap = match.captured(1);
		if (!cap.isEmpty() && !cap.startsWith('<'))
		{
			cap += QString(">").repeated(cap.count('<') - cap.count('>'));
			ret.replace("<" + cap + ">", this->expandConditionals(cap, tags, tokens, settings, depth + 1));
		}
	}

	if (depth > 0)
	{
		// Token-based conditions
		reg = QRegularExpression("(-)?(!)?(%([^:%]+)(?::[^%]+)?%)");
		matches = reg.globalMatch(text);
		while (matches.hasNext())
		{
			auto match = matches.next();
			bool ignore = !match.captured(1).isEmpty();
			bool invert = !match.captured(2).isEmpty();
			QString fullToken = match.captured(3);
			QString token = match.captured(4);
			if ((tokens.contains(token) && !isVariantEmpty(tokens[token])) == !invert)
			{
				QString rep = ignore || invert ? "" : fullToken;
				ret.replace(match.captured(0), rep);
			}
			else
			{ return ""; }
		}

		// Tag-based conditions
		reg = QRegularExpression("(-)?(!)?\"([^\"]+)\"");
		matches = reg.globalMatch(text);
		while (matches.hasNext())
		{
			auto match = matches.next();
			bool ignore = !match.captured(1).isEmpty();
			bool invert = !match.captured(2).isEmpty();
			QString tag = match.captured(3);
			if (tags.contains(tag, Qt::CaseInsensitive) == !invert)
			{
				QString rep = ignore ? "" : this->cleanUpValue(tag, QMap<QString, QString>(), settings);
				ret.replace(match.captured(0), rep);
			}
			else
			{ return ""; }
		}
	}

	if (depth == 0)
	{ ret.replace(QRegularExpression("<<([^>]*)>>"), "<\\1>"); }

	return ret;
}

QList<QVariant> Filename::getReplace(const QString &key, const QStringList &value, QSettings *settings) const
{
	QList<QVariant> ret;

	QStringList applicable = QStringList() << "artist" << "copyright" << "character" << "model" << "species";
	if (!applicable.contains(key))
	{
		ret.append(value);
		return ret;
	}

	settings->beginGroup("Save");
	QString emptyDefault = key == "copyright" ? "misc" : (key == "artist" ? "anonymous" : "unknown");
	QString multipleDefault = key == "copyright" ? "crossover" : (key == "artist" ? "multiple artists" : (key == "character" ? "group" : "multiple"));

	if (value.isEmpty())
	{ ret.append(settings->value(key + "_empty", emptyDefault).toString()); }
	else if (value.size() > settings->value(key + "_multiple_limit", 1).toInt())
	{
		QString whatToDo = settings->value(key + "_multiple", "replaceAll").toString();
		if (whatToDo == "keepAll")
		{ ret.append(value); }
		else if (whatToDo == "multiple")
		{
			for (const QString &val : value)
			{ ret.append(val); }
		}
		else if (whatToDo == "keepN")
		{
			int keepN = settings->value(key + "_multiple_keepN", 1).toInt();
			ret.append(QStringList(value.mid(0, qMax(1, keepN))));
		}
		else if (whatToDo == "keepNThenAdd")
		{
			int keepN = settings->value(key + "_multiple_keepNThenAdd_keep", 1).toInt();
			QString thenAdd = settings->value(key + "_multiple_keepNThenAdd_add", " (+ %count%)").toString();
			thenAdd.replace("%total%", QString::number(value.size()));
			thenAdd.replace("%count%", QString::number(value.size() - keepN));
			QStringList keptValues = value.mid(0, qMax(1, keepN));
			if (value.size() > keepN)
			{ ret.append(keptValues.join(' ') + thenAdd); }
			else
			{ ret.append(keptValues); }
		}
		else
		{ ret.append(settings->value(key + "_value", multipleDefault).toString()); }
	}
	else
	{ ret.append(value); }

	settings->endGroup();
	return ret;
}

QString generateJavaScriptVariableInternal(const QString &name, const QString &value)
{
	return "var " + name + " = " + value + ";\r\n";
}
QString generateJavaScriptVariableDate(const QString &name, const QString &value)
{
	return generateJavaScriptVariableInternal(name, "new Date(\"" + value + "\")");
}
QString generateJavaScriptVariable(const QString &name, const QStringList &values)
{
	return generateJavaScriptVariableInternal(name, values.isEmpty() ? "[]" : "[\"" + values.join("\", \"") + "\"]");
}
QString generateJavaScriptVariable(const QString &name, const QString &value)
{
	return generateJavaScriptVariableInternal(name, "\"" + value + "\"");
}

QString Filename::generateJavaScriptVariables(QSettings *settings, const QMap<QString, QVariant> &tokens) const
{
	QString inits = "";
	for (const QString &key : tokens.keys())
	{
		QVariant val = tokens[key];
		QString res;

		if (val.type() == QVariant::StringList)
		{
			QStringList vals = val.toStringList();
			QString mainSeparator = settings->value("Save/separator", " ").toString();
			QString tagSeparator = fixSeparator(settings->value("Save/" + key + "_sep", mainSeparator).toString());

			if (key != "all" && key != "tags")
			{ inits += generateJavaScriptVariable(key + "s", vals); }

			res = vals.join(tagSeparator);
		}
		else if (val.type() == QVariant::DateTime)
		{ res = val.toDateTime().toString(Qt::ISODate); }
		else
		{ res = val.toString(); }

		if (key != "allo")
		{
			res = res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed();
			if (!settings->value("Save/replaceblanks", false).toBool())
			{ res.replace("_", " "); }
		}

		if (key == "date")
		{ inits += generateJavaScriptVariableDate(key, res); }
		else
		{ inits += generateJavaScriptVariable(key, res); }
	}
	return inits;
}

bool Filename::matchConditionalFilename(QString cond, QSettings *settings, const QMap<QString, QVariant> &tokens) const
{
	// Javascript conditions
	if (cond.startsWith("javascript:"))
	{
		// We remove the "javascript:" part
		cond = cond.right(cond.length() - 11);

		// Variables initialization
		QString inits = generateJavaScriptVariables(settings, tokens);

		// Script execution
		QScriptEngine engine;
		QScriptValue result = engine.evaluate(QScriptProgram(inits + cond));
		if (result.isError())
		{
			log("Error in Javascript evaluation:<br/>" + result.toString());
			return false;
		}

		return result.toBool();
	}

	// Other conditions require tag tokens
	if (!tokens.contains("tags"))
		return false;

	QStringList options = cond.split(' ');

	// Token conditions
	int condPer = cond.count('%');
	if (condPer > 0 && condPer % 2 == 0)
	{
		QRegularExpression reg("%([^%]+?)%");
		auto matches = reg.globalMatch(cond);
		while (matches.hasNext())
		{
			auto match = matches.next();
			QString token = match.captured(1);
			if (tokens.contains(token))
			{
				options.removeOne(match.captured(0));

				QVariant val = tokens[token];
				if (val.type() == QVariant::StringList)
				{ options.append(val.toStringList()); }
			}
		}
	}

	// Tag conditions
	for (const QString &opt : options)
		if (tokens["tags"].toStringList().contains(opt))
			return true;

	return false;
}

QList<QMap<QString, QVariant>> Filename::expandTokens(const QString &filename, QMap<QString, QVariant> tokens, QSettings *settings) const
{
	QList<QMap<QString, QVariant>> ret;
	ret.append(tokens);

	bool isJavascript = filename.startsWith("javascript:");
	for (const QString &key : tokens.keys())
	{
		const QVariant &val = tokens[key];
		if (val.type() != QVariant::StringList)
			continue;

		bool hasToken = !isJavascript && filename.contains(QRegularExpression("%"+key+"(?::[^%]+)?%"));
		bool hasVar = isJavascript && filename.contains(key);
		if (!hasToken && !hasVar)
			continue;

		QList<QVariant> reps = getReplace(key, val.toStringList(), settings);
		int cnt = ret.count();
		for (int i = 0; i < cnt; ++i)
		{
			ret[i].insert(key, reps[0]);
			for (int j = 1; j < reps.count(); ++j)
			{
				tokens = ret[i];
				tokens.insert(key, reps[j]);
				ret.append(tokens);
			}
		}
	}

	return ret;
}

QStringList Filename::path(const Image& img, Profile *profile, QString pth, int counter, bool complex, bool maxLength, bool shouldFixFilename, bool getFull, bool keepInvalidTokens) const
{ return path(img.tokens(profile), profile, pth, counter, complex, maxLength, shouldFixFilename, getFull, keepInvalidTokens); }
QStringList Filename::path(QMap<QString, QVariant> tokens, Profile *profile, QString folder, int counter, bool complex, bool maxLength, bool shouldFixFilename, bool getFull, bool keepInvalidTokens) const
{
	QSettings *settings = profile->getSettings();
	QString filename = m_format;

	// Count token
	tokens.insert("count", counter);

	// Custom tokens (if the tokens contain tags)
	if (tokens.contains("tags"))
	{
		QMap<QString, QStringList> scustom = getCustoms(settings);
		QMap<QString, QStringList> custom;
		for (const QString &tag : tokens["tags"].toStringList())
		{
			for (const QString &key : scustom.keys())
			{
				if (!custom.contains(key))
				{ custom.insert(key, QStringList()); }
				if (scustom[key].contains(tag, Qt::CaseInsensitive))
				{ custom[key].append(tag); }
			}
		}
		for (const QString &key : custom.keys())
		{ tokens.insert(key, custom[key]); }
	}

	// Conditional filenames
	QMap<QString, QPair<QString, QString>> filenames = getFilenames(settings);
	for (const QString &cond : filenames.keys())
	{
		if (matchConditionalFilename(cond, settings, tokens))
		{
			QPair<QString, QString> result = filenames[cond];
			if (!result.first.isEmpty())
			{ filename = result.first; }
			if (!result.second.isEmpty())
			{ folder = result.second; }
		}
	}

	// Expand tokens into multiple filenames
	QList<QMap<QString, QVariant>> replacesList = expandTokens(filename, tokens, settings);

	QStringList fns;

	if (filename.startsWith("javascript:"))
	{
		// We remove the "javascript:" part
		filename = filename.right(filename.length() - 11);

		for (const auto &replaces : replacesList)
		{
			// Variables initialization
			QString inits = generateJavaScriptVariables(settings, replaces);

			// Script execution
			QScriptEngine engine;
			QScriptValue result = engine.evaluate(QScriptProgram(inits + filename));
			if (result.isError())
			{
				log("Error in Javascript evaluation:<br/>" + result.toString());
				return QStringList();
			}

			fns.append(result.toString());
		}
	}
	else
	{
		// We get path and remove useless slashes from filename
		folder.replace("\\", "/");
		//filename.replace("\\", "/");
		if (filename.left(1) == "/")
		{ filename = filename.right(filename.length() - 1); }
		if (folder.right(1) == "/")
		{ folder = folder.left(folder.length() - 1); }

		QStringList ignoredTokens = QStringList() << "path" << "num";

		for (const auto &replaces : replacesList)
		{
			QString cFilename = QString(filename);
			QString hasNum;
			QString numOptions;
			QStringList namespaces = replaces["all_namespaces"].toStringList();

			// Conditionals
			if (complex)
			{ cFilename = this->expandConditionals(cFilename, tokens["allos"].toStringList(), replaces, settings); }

			// Replace everything
			QRegExp replacerx("%([^:]+)(?::([^%]+))?%");
			replacerx.setMinimal(true);
			int p = 0;
			while ((p = replacerx.indexIn(cFilename, p)) != -1)
			{
				QString key = replacerx.cap(1);
				QString options = replacerx.captureCount() > 1 ? replacerx.cap(2) : QString();

				if (replaces.contains(key))
				{
					QVariant val = replaces[key];

					QString res = optionedValue(val, key, options, settings, namespaces);
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
				else if (!keepInvalidTokens)
				{ cFilename.remove(replacerx.cap(0)); }
				else
				{ p += replacerx.matchedLength(); }
			}

			if (!hasNum.isEmpty())
			{
				int mid = QDir::toNativeSeparators(cFilename).lastIndexOf(QDir::separator());
				QDir dir(folder + (mid >= 0 ? QDir::separator() + cFilename.left(mid) : ""));
				QString cRight = mid >= 0 ? cFilename.right(cFilename.length() - mid - 1) : cFilename;
				QString filter = QString(cRight).replace(hasNum, "*");
				QFileInfoList files = dir.entryInfoList(QStringList() << filter, QDir::Files, QDir::NoSort);

				// Sort files naturally
				QCollator collator;
				collator.setNumericMode(true);
				std::sort(files.begin(), files.end(), [&collator](const QFileInfo &a, const QFileInfo &b)
				{ return collator.compare(a.fileName(), b.fileName()) < 0; });

				int num = 1;
				if (!files.isEmpty())
				{
					QString last = files.last().fileName();
					int pos = cRight.indexOf(hasNum);
					int len = last.length() - cRight.length() + 5;
					num = last.mid(pos, len).toInt() + 1;
				}
				cFilename.replace(hasNum, optionedValue(num, "num", numOptions, settings, namespaces));
			}

			fns.append(cFilename);
		}
	}

	int cnt = fns.count();
	for (int i = 0; i < cnt; ++i)
	{
		// Trim directory names
		fns[i] = fns[i].trimmed();
		fns[i].replace(QRegularExpression(" */ *"), "/");

		// Max filename size option
		if (shouldFixFilename)
		{
			int limit = !maxLength ? 0 : settings->value("Save/limit").toInt();
			fns[i] = fixFilename(fns[i], folder, limit);
		}

		// Include directory in result
		if (getFull)
		{ fns[i] = folder + "/" + fns[i]; }

		if (shouldFixFilename)
		{
			// Native separators
			fns[i] = QDir::toNativeSeparators(fns[i]);

			// We remove empty directory names
			QChar sep = QDir::separator();
			fns[i].replace(QRegularExpression("(.)" + QRegularExpression::escape(sep) + "{2,}"), QString("\\1") + sep);
		}
	}

	return fns;
}

QString Filename::cleanUpValue(QString res, QMap<QString, QString> options, QSettings *settings) const
{
	// Forbidden characters
	if (!options.contains("unsafe"))
	{ res = res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed(); }

	// Replace underscores by spaces
	if (!options.contains("underscores") && (!settings->value("Save/replaceblanks", false).toBool() || options.contains("spaces")))
	{ res = res.replace("_", " "); }

	return res;
}

QString Filename::optionedValue(const QVariant &val, QString key, QString ops, QSettings *settings, QStringList namespaces) const
{
	bool cleaned = false;

	// Parse options
	QMap<QString,QString> options;
	if (!ops.isEmpty())
	{
		QStringList opts = ops.split(QRegularExpression("(?<!\\\\),"), QString::SkipEmptyParts);
		for (const QString &opt : opts)
		{
			int index = opt.indexOf('=');
			if (index != -1)
			{
				QString v = opt.mid(index + 1);
				v.replace("\\,", ",");
				options.insert(opt.left(index), v);
			}
			else
			{ options.insert(opt, "true"); }
		}
	}

	QString res;

	// Type-specific options
	if (val.type() == QVariant::DateTime)
	{
		QString format = options.value("format", QObject::tr("MM-dd-yyyy HH.mm"));
		res = val.toDateTime().toString(format);
	}
	else if (val.type() == QVariant::Int)
	{ res = options.contains("length") ? QString("%1").arg(val.toInt(), options["length"].toInt(), 10, QChar('0')) : QString::number(val.toInt()); }
	else if (val.type() == QVariant::StringList)
	{
		QStringList vals = val.toStringList();
		QString mainSeparator = settings->value("Save/separator", " ").toString();
		QString tagSeparator = fixSeparator(settings->value("Save/" + key + "_sep", mainSeparator).toString());

		// Namespaces
		if (options.contains("ignorenamespace"))
		{
			QStringList ignored = options["ignorenamespace"].split(' ');
			QStringList filtered, filteredNamespaces;
			for (int i = 0; i < vals.count(); ++i)
			{
				QString nspace = key == "all" ? namespaces[i] : key;
				if (!ignored.contains(nspace))
				{
					filtered.append(vals[i]);
					filteredNamespaces.append(namespaces[i]);
				}
			}
			vals = filtered;
			namespaces = filteredNamespaces;
		}
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
		if (options.contains("separator"))
		{ tagSeparator = fixSeparator(options["separator"]); }
		if (options.contains("sort"))
		{ qSort(vals); }

		// Clean each value separately
		for (QString &t : vals)
		{ t = this->cleanUpValue(t, options, settings); }
		cleaned = true;

		res = vals.join(tagSeparator);
	}
	else
	{ res = val.toString(); }

	// String options
	if (options.contains("maxlength"))
	{ res = res.left(options["maxlength"].toInt()); }
	if (options.contains("htmlescape"))
	{ res = res.toHtmlEscaped(); }

	// Forbidden characters and spaces replacement settings
	if (key != "allo" && !key.startsWith("url_") && key != "filename" && !cleaned)
	{ res = this->cleanUpValue(res, options, settings); }

	// Escape if necessary
	if (m_escapeMethod != nullptr && options.contains("escape"))
	{ res = m_escapeMethod(res); }

	return res;
}

QString Filename::fixSeparator(QString separator) const
{
	return QString(separator)
		.replace("\\n", "\n")
		.replace("\\r", "\r");
}

QString Filename::getFormat() const
{
	return m_format;
}
void Filename::setFormat(QString format)
{
	m_format = format;
}

void Filename::setEscapeMethod(QString (*escapeMethod)(QVariant))
{
	m_escapeMethod = escapeMethod;
}

bool Filename::returnError(QString msg, QString *error) const
{
	if (error != nullptr)
		*error = msg;

	return false;
}
bool Filename::isValid(Profile *profile, QString *error) const
{
	QString red = "<span style=\"color:red\">%1</span>";
	QString orange = "<span style=\"color:orange\">%1</span>";
	QString green = "<span style=\"color:green\">%1</span>";

	// Field must be filled
	if (m_format.isEmpty())
		return returnError(red.arg(QObject::tr("Filename must not be empty!")), error);

	// Can't validate javascript expressions
	if (m_format.startsWith("javascript:"))
	{
		returnError(orange.arg(QObject::tr("Can't validate Javascript expressions.")), error);
		return true;
	}

	// Field must end by an extension
	if (!m_format.endsWith(".%ext%"))
		return returnError(orange.arg(QObject::tr("Your filename doesn't ends by an extension, symbolized by %ext%! You may not be able to open saved files.")), error);

	// Field must contain an unique token
	if (!m_format.contains("%md5%") && !m_format.contains("%id%") && !m_format.contains("%num%"))
		return returnError(orange.arg(QObject::tr("Your filename is not unique to each image and an image may overwrite a previous one at saving! You should use%md5%, which is unique to each image, to avoid this inconvenience.")), error);

	// Looking for unknown tokens
	QStringList tokens = QStringList() << "tags" << "artist" << "general" << "copyright" << "character" << "model" << "species" << "filename" << "rating" << "md5" << "website" << "websitename" << "ext" << "all" << "id" << "search" << "search_(\\d+)" << "allo" << "date" << "score" << "count" << "width" << "height" << "pool" << "url_file" << "url_page";
	if (profile != nullptr)
	{ tokens.append(getCustoms(profile->getSettings()).keys()); }
	QRegularExpression rx("%(.+?)%");
	auto matches = rx.globalMatch(m_format);
	while (matches.hasNext())
	{
		auto match = matches.next();
		bool found = false;
		for (int i = 0; i < tokens.length(); i++)
		{
			if (QRegularExpression("%"+tokens[i]+"(?::[^%]+)?%").match(match.captured(0)).hasMatch())
				found = true;
		}

		if (!found)
			return returnError(orange.arg(QObject::tr("The %%1% token does not exist and will not be replaced.")).arg(match.captured(1)), error);
	}

	// Check for invalid windows characters
	#ifdef Q_OS_WIN
		QString txt = QString(m_format).remove(rx);
		if (txt.contains(':') || txt.contains('*') || txt.contains('?') || (txt.contains('"') && txt.count('<') == 0) || txt.count('<') != txt.count('>') || txt.contains('|'))
			return returnError(red.arg(QObject::tr("Your format contains characters forbidden on Windows! Forbidden characters: * ? \" : < > |")), error);
	#endif

	// Check if code is unique
	if (!m_format.contains("%md5%") && !m_format.contains("%website%") && !m_format.contains("%websitename%") && !m_format.contains("%count%") && m_format.contains("%id%"))
		return returnError(green.arg(QObject::tr("You have chosen to use the %id% token. Know that it is only unique for a selected site. The same ID can identify different images depending on the site.")), error);

	// All tests passed
	returnError(green.arg(QObject::tr("Valid filename!")), error);
	return true;
}

bool Filename::needExactTags(Site *site, QString api) const
{
	bool forceImageUrl = site != nullptr && site->contains("Regex/ForceImageUrl");
	bool needDate = site != nullptr && (api == "Html" || api == "Rss") && site->contains("Regex/ImageDate");
	return needExactTags(forceImageUrl, needDate);
}
bool Filename::needExactTags(bool forceImageUrl, bool needDate) const
{
	// Javascript filenames always need tags as we don't know what they might do
	if (m_format.startsWith("javascript:"))
		return true;

	// If we need the filename and it is returned from the details page
	if (m_format.contains(QRegularExpression("%filename(?::([^%]+))?%")) && forceImageUrl)
		return true;

	// If we need the date and it is returned from the details page
	if (m_format.contains(QRegularExpression("%date(?::([^%]+))?%")) && needDate)
		return true;

	// The filename contains one of the special tags
	QStringList forbidden = QStringList() << "artist" << "copyright" << "character" << "model" << "species" << "general";
	for (const QString &token : forbidden)
		if (m_format.contains(QRegularExpression("%" + token + "(?::([^%]+))?%")))
			return true;

	// Namespaces come from detailed tags
	if (m_format.contains("includenamespace"))
		return true;

	return false;
}
