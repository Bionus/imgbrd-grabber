#include "models/filename.h"
#include <QCollator>
#include <QIcon>
#include <QJSEngine>
#include <QRegularExpression>
#include <algorithm>
#include "functions.h"
#include "models/api/api.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"
#include "post-filter.h"


Filename::Filename(const QString &format)
	: m_format(format)
{ }

QString Filename::expandConditionals(const QString &text, const QStringList &tags, const QMap<QString, Token> &tokens, QSettings *settings, int depth) const
{
	QString ret = text;

	QRegularExpression reg(QStringLiteral("\\<([^>]+)\\>"));
	auto matches = reg.globalMatch(text);
	while (matches.hasNext())
	{
		auto match = matches.next();
		QString cap = match.captured(1);
		if (!cap.isEmpty() && !cap.startsWith('<'))
		{
			cap += QStringLiteral(">").repeated(cap.count('<') - cap.count('>'));
			ret.replace("<" + cap + ">", this->expandConditionals(cap, tags, tokens, settings, depth + 1));
		}
	}

	if (depth > 0)
	{
		// Token-based conditions
		reg = QRegularExpression(QStringLiteral("(-)?(!)?(%([^:%]+)(?::[^%]+)?%)"));
		matches = reg.globalMatch(text);
		while (matches.hasNext())
		{
			const auto match = matches.next();
			const bool ignore = !match.captured(1).isEmpty();
			const bool invert = !match.captured(2).isEmpty();
			const QString &fullToken = match.captured(3);
			const QString &token = match.captured(4);
			if ((tokens.contains(token) && !isVariantEmpty(tokens[token].value())) == !invert)
			{
				const QString &rep = ignore || invert ? QString() : fullToken;
				ret.replace(match.captured(0), rep);
			}
			else
			{ return QString(); }
		}

		// Tag-based conditions
		reg = QRegularExpression(QStringLiteral("(-)?(!)?\"([^\"]+)\""));
		matches = reg.globalMatch(text);
		while (matches.hasNext())
		{
			const auto match = matches.next();
			const bool ignore = !match.captured(1).isEmpty();
			const bool invert = !match.captured(2).isEmpty();
			const QString &tag = match.captured(3);
			if (tags.contains(tag, Qt::CaseInsensitive) == !invert)
			{
				const QString &rep = ignore ? QString() : this->cleanUpValue(tag, QMap<QString, QString>(), settings);
				ret.replace(match.captured(0), rep);
			}
			else
			{ return QString(); }
		}
	}

	if (depth == 0)
	{ ret.replace(QRegularExpression(QStringLiteral("<<([^>]*)>>")), QStringLiteral("<\\1>")); }

	return ret;
}

QList<Token> Filename::getReplace(const QString &key, const Token &token, QSettings *settings) const
{
	QList<Token> ret;
	const QStringList &value = token.value().toStringList();

	if (token.whatToDoDefault().isEmpty())
	{
		ret.append(Token(value));
		return ret;
	}

	settings->beginGroup("Save");

	if (value.isEmpty())
	{ ret.append(Token(settings->value(key + "_empty", token.emptyDefault()).toString())); }
	else if (value.size() > settings->value(key + "_multiple_limit", 1).toInt())
	{
		const QString &whatToDo = settings->value(key + "_multiple", token.whatToDoDefault()).toString();
		if (whatToDo == QLatin1String("keepAll"))
		{ ret.append(Token(value)); }
		else if (whatToDo == QLatin1String("multiple"))
		{
			ret.reserve(ret.count() + value.count());
			for (const QString &val : value)
			{ ret.append(Token(val)); }
		}
		else if (whatToDo == QLatin1String("keepN"))
		{
			const int keepN = settings->value(key + "_multiple_keepN", 1).toInt();
			ret.append(Token(QStringList(value.mid(0, qMax(1, keepN)))));
		}
		else if (whatToDo == QLatin1String("keepNThenAdd"))
		{
			const int keepN = settings->value(key + "_multiple_keepNThenAdd_keep", 1).toInt();
			QString thenAdd = settings->value(key + "_multiple_keepNThenAdd_add", " (+ %count%)").toString();
			thenAdd.replace("%total%", QString::number(value.size()));
			thenAdd.replace("%count%", QString::number(value.size() - keepN));
			QStringList keptValues = value.mid(0, qMax(1, keepN));
			if (value.size() > keepN)
			{ ret.append(Token(keptValues.join(' ') + thenAdd)); }
			else
			{ ret.append(Token(keptValues)); }
		}
		else
		{ ret.append(Token(settings->value(key + "_value", token.multipleDefault()).toString())); }
	}
	else
	{ ret.append(Token(value)); }

	settings->endGroup();
	return ret;
}

void Filename::setJavaScriptVariables(QJSEngine &engine, QSettings *settings, const QMap<QString, Token> &tokens) const
{
	QJSValue obj = engine.globalObject();

	for (auto it = tokens.begin(); it != tokens.end(); ++it)
	{
		const QString &key = it.key();
		QVariant val = it.value().value();

		if (val.type() == QVariant::StringList || val.type() == QVariant::String)
		{
			QString res;

			if (val.type() == QVariant::StringList)
			{
				QStringList vals = val.toStringList();
				const QString mainSeparator = settings->value("Save/separator", " ").toString();
				const QString tagSeparator = fixSeparator(settings->value("Save/" + key + "_sep", mainSeparator).toString());

				if (key != "all" && key != "tags")
				{ obj.setProperty(key + "s", engine.toScriptValue(vals)); }

				res = vals.join(tagSeparator);
			}
			else
			{ res = val.toString(); }

			if (key != "allo")
			{
				res = res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed();
				if (!settings->value("Save/replaceblanks", false).toBool())
				{ res.replace("_", " "); }
			}

			obj.setProperty(key, res);
		}
		else
		{ obj.setProperty(key, engine.toScriptValue(val)); }
	}
}

bool Filename::matchConditionalFilename(QString cond, QSettings *settings, const QMap<QString, Token> &tokens) const
{
	if (cond.isEmpty())
		return false;

	// Javascript conditions
	if (cond.startsWith("javascript:"))
	{
		// We remove the "javascript:" part
		cond = cond.right(cond.length() - 11);

		// Script execution
		QJSEngine engine;
		setJavaScriptVariables(engine, settings, tokens);
		QJSValue result = engine.evaluate(cond);
		if (result.isError())
		{
			log("Error in Javascript evaluation:<br/>" + result.toString());
			return false;
		}

		return result.toBool();
	}

	const QStringList options = cond.split(' ');
	const QStringList matches = PostFilter::filter(tokens, options);

	return matches.count() < options.count();
}

QList<QMap<QString, Token>> Filename::expandTokens(const QString &filename, QMap<QString, Token> tokens, QSettings *settings) const
{
	QList<QMap<QString, Token>> ret;
	ret.append(tokens);

	const bool isJavascript = filename.startsWith(QLatin1String("javascript:"));
	for (const QString &key : tokens.keys())
	{
		const Token &token = tokens[key];
		if (token.value().type() != QVariant::StringList)
			continue;

		const bool hasToken = !isJavascript && filename.contains(QRegularExpression("%"+key+"(?::[^%]+)?%"));
		const bool hasVar = isJavascript && filename.contains(key);
		if (!hasToken && !hasVar)
			continue;

		QList<Token> reps = getReplace(key, token, settings);
		const int cnt = ret.count();
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

QStringList Filename::path(const Image& img, Profile *profile, const QString &pth, int counter, bool complex, bool maxLength, bool shouldFixFilename, bool getFull, bool keepInvalidTokens) const
{ return path(img.tokens(profile), profile, pth, counter, complex, maxLength, shouldFixFilename, getFull, keepInvalidTokens); }
QStringList Filename::path(QMap<QString, Token> tokens, Profile *profile, QString folder, int counter, bool complex, bool maxLength, bool shouldFixFilename, bool getFull, bool keepInvalidTokens) const
{
	QSettings *settings = profile->getSettings();
	QString filename = m_format;

	// Count token
	tokens.insert("count", Token(counter));

	// Conditional filenames
	if (complex)
	{
		QMap<QString, QPair<QString, QString>> filenames = getFilenames(settings);
		for (auto it = filenames.begin(); it != filenames.end(); ++it)
		{
			if (matchConditionalFilename(it.key(), settings, tokens))
			{
				const QPair<QString, QString> &result = it.value();
				if (!result.first.isEmpty())
				{ filename = result.first; }
				if (!result.second.isEmpty())
				{ folder = result.second; }
			}
		}
	}

	// Expand tokens into multiple filenames
	QList<QMap<QString, Token>> replacesList = expandTokens(filename, tokens, settings);

	QStringList fns;

	if (filename.startsWith("javascript:"))
	{
		// We remove the "javascript:" part
		filename = filename.right(filename.length() - 11);

		for (const auto &replaces : replacesList)
		{
			// Script execution
			QJSEngine engine;
			setJavaScriptVariables(engine, settings, replaces);
			QJSValue result = engine.evaluate(filename);
			if (result.isError())
			{
				log("Error in Javascript evaluation:<br/>" + result.toString());
				return QStringList();
			}

			fns.append(result.toString());
		}
	}
	else if (!filename.isEmpty())
	{
		// We get path and remove useless slashes from filename
		folder.replace("\\", "/");
		//filename.replace("\\", "/");
		if (filename.at(0) == QChar('/'))
		{ filename = filename.right(filename.length() - 1); }
		if (folder.right(1) == "/")
		{ folder = folder.left(folder.length() - 1); }

		QStringList ignoredTokens = QStringList() << "path" << "num";

		for (const auto &replaces : replacesList)
		{
			QString cFilename = QString(filename);
			QString hasNum;
			QString numOptions;
			const QStringList namespaces = replaces["all_namespaces"].value().toStringList();

			// Conditionals
			if (complex)
			{ cFilename = this->expandConditionals(cFilename, tokens["allos"].value().toStringList(), replaces, settings); }

			// Replace everything
			QRegExp replacerx("%([^:]+)(?::([^%]+))?%");
			replacerx.setMinimal(true);
			int p = 0;
			while ((p = replacerx.indexIn(cFilename, p)) != -1)
			{
				const QString &key = replacerx.cap(1);
				const QString &options = replacerx.captureCount() > 1 ? replacerx.cap(2) : QString();

				if (replaces.contains(key))
				{
					const QVariant &val = replaces[key].value();
					const QString &res = optionedValue(val, key, options, settings, namespaces);
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
				const int mid = QDir::toNativeSeparators(cFilename).lastIndexOf(QDir::separator());
				QDir dir(folder + (mid >= 0 ? QDir::separator() + cFilename.left(mid) : QString()));
				const QString cRight = mid >= 0 ? cFilename.right(cFilename.length() - mid - 1) : cFilename;
				const QString filter = QString(cRight).replace(hasNum, "*");
				QFileInfoList files = dir.entryInfoList(QStringList() << filter, QDir::Files, QDir::NoSort);

				// Sort files naturally
				QCollator collator;
				collator.setNumericMode(true);
				std::sort(files.begin(), files.end(), [&collator](const QFileInfo &a, const QFileInfo &b)
				{ return collator.compare(a.fileName(), b.fileName()) < 0; });

				int num = 1;
				if (!files.isEmpty())
				{
					const QString last = files.last().fileName();
					const int pos = cRight.indexOf(hasNum);
					const int len = last.length() - cRight.length() + 5;
					num = last.midRef(pos, len).toInt() + 1;
				}
				cFilename.replace(hasNum, optionedValue(num, "num", numOptions, settings, namespaces));
			}

			fns.append(cFilename);
		}
	}

	const int cnt = fns.count();
	for (int i = 0; i < cnt; ++i)
	{
		if (shouldFixFilename)
		{
			// Trim directory names
			fns[i] = fns[i].trimmed();
			fns[i].replace(QRegularExpression(" */ *"), "/");

			// Max filename size option
			const int limit = !maxLength ? 0 : settings->value("Save/limit").toInt();
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
			const QChar sep = QDir::separator();
			fns[i].replace(QRegularExpression("(.)" + QRegularExpression::escape(sep) + "{2,}"), QString("\\1") + sep);
		}
	}

	return fns;
}

QString Filename::cleanUpValue(QString res, const QMap<QString, QString> &options, QSettings *settings) const
{
	// Forbidden characters
	if (!options.contains("unsafe"))
	{ res = res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed(); }

	// Replace underscores by spaces
	if (!options.contains("underscores") && (!settings->value("Save/replaceblanks", false).toBool() || options.contains("spaces")))
	{ res = res.replace("_", " "); }

	return res;
}

QString Filename::optionedValue(const QVariant &val, const QString &key, const QString &ops, QSettings *settings, QStringList namespaces) const
{
	bool cleaned = false;

	// Parse options
	QMap<QString, QString> options;
	if (!ops.isEmpty())
	{
		QStringList opts = ops.split(QRegularExpression("(?<!\\\\),"), QString::SkipEmptyParts);
		for (const QString &opt : opts)
		{
			const int index = opt.indexOf('=');
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
		const QString format = options.value("format", QObject::tr("MM-dd-yyyy HH.mm"));
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
				const QString &nspace = key == "all" ? namespaces[i] : key;
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
				const QString nspace = key == "all" ? namespaces[i] : key;
				namespaced.append((!excluded.contains(nspace) ? nspace + ":" : QString()) + vals[i]);
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

QString Filename::fixSeparator(const QString &separator) const
{
	return QString(separator)
		.replace("\\n", "\n")
		.replace("\\r", "\r");
}

QString Filename::getFormat() const
{
	return m_format;
}
void Filename::setFormat(const QString &format)
{
	m_format = format;
}

void Filename::setEscapeMethod(QString (*escapeMethod)(const QVariant &))
{
	m_escapeMethod = escapeMethod;
}

bool Filename::returnError(const QString &msg, QString *error) const
{
	if (error != nullptr)
		*error = msg;

	return false;
}
bool Filename::isValid(Profile *profile, QString *error) const
{
	static const QString red = QStringLiteral("<span style=\"color:red\">%1</span>");
	static const QString orange = QStringLiteral("<span style=\"color:orange\">%1</span>");
	static const QString green = QStringLiteral("<span style=\"color:green\">%1</span>");

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
	QStringList tokens = QStringList() << "tags" << "artist" << "general" << "copyright" << "character" << "model" << "species" << "meta" << "filename" << "rating" << "md5" << "website" << "websitename" << "ext" << "all" << "id" << "search" << "search_(\\d+)" << "allo" << "date" << "score" << "count" << "width" << "height" << "pool" << "url_file" << "url_page" << "num";
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
			return returnError(orange.arg(QObject::tr("The %%1% token does not exist and will not be replaced.").arg(match.captured(1))), error);
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

bool Filename::needTemporaryFile(const QMap<QString, Token> &tokens) const
{
    return (
        (m_format.contains(QRegularExpression("%md5(?::([^%]+))?%")) && (!tokens.contains("md5") || tokens["md5"].value().toString().isEmpty())) ||
        (m_format.contains(QRegularExpression("%filesize(?::([^%]+))?%")) && (!tokens.contains("filesize") || tokens["filesize"].value().toInt() <= 0))
    );
}

int Filename::needExactTags(Site *site, const QString &api) const
{
	Q_UNUSED(api);

	QStringList forcedTokens = site != nullptr
		? site->getApis().first()->forcedTokens()
		: QStringList();

	if (forcedTokens.contains("*"))
		return 2;

	return needExactTags(forcedTokens);
}
int Filename::needExactTags(const QStringList &forcedTokens) const
{
	// Javascript filenames always need tags as we don't know what they might do
	if (m_format.startsWith("javascript:"))
		return 2;

	// If we need the filename and it is returned from the details page
	if (m_format.contains(QRegularExpression("%filename(?::([^%]+))?%")) && forcedTokens.contains("filename"))
		return 2;

	// If we need the date and it is returned from the details page
	if (m_format.contains(QRegularExpression("%date(?::([^%]+))?%")) && forcedTokens.contains("date"))
		return 2;

	// The filename contains one of the special tags
	QStringList forbidden = QStringList() << "artist" << "copyright" << "character" << "model" << "species" << "meta" << "general";
	for (const QString &token : forbidden)
		if (m_format.contains(QRegularExpression("%" + token + "(?::([^%]+))?%")))
			return 1;

	// Namespaces come from detailed tags
	if (m_format.contains("includenamespace"))
		return 1;

	return 0;
}
