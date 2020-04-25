#include "models/filename.h"
#include <QCollator>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QJSEngine>
#include <QRegularExpression>
#include <QSettings>
#include <algorithm>
#include <utility>
#include "filename/ast/filename-node-variable.h"
#include "filename/ast-filename.h"
#include "filename/conditional-filename.h"
#include "filename/filename-cache.h"
#include "filename/filename-execution-visitor.h"
#include "functions.h"
#include "loader/token.h"
#include "models/api/api.h"
#include "models/filtering/post-filter.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"


Filename::Filename()
	: Filename("")
{}

Filename::Filename(QString format)
	: m_format(std::move(format))
{
	m_ast = FilenameCache::Get(m_format);
}

QList<Token> Filename::getReplace(const QString &key, const Token &token, QSettings *settings) const
{
	QList<Token> ret;
	QStringList value = token.value().toStringList();

	if (token.whatToDoDefault().isEmpty()) {
		ret.append(Token(value));
		return ret;
	}

	settings->beginGroup("Save");

	const QString sort = settings->value(key + "_sort", "original").toString();
	if (sort == QLatin1String("name")) {
		value.sort();
	}

	if (value.isEmpty()) {
		ret.append(Token(settings->value(key + "_empty", token.emptyDefault()).toString()));
	} else if (value.size() > settings->value(key + "_multiple_limit", 1).toInt()) {
		const QString &whatToDo = settings->value(key + "_multiple", token.whatToDoDefault()).toString();
		if (whatToDo == QLatin1String("keepAll")) {
			ret.append(Token(value));
		} else if (whatToDo == QLatin1String("multiple")) {
			ret.reserve(ret.count() + value.count());
			for (const QString &val : value) {
				ret.append(Token(val));
			}
		} else if (whatToDo == QLatin1String("keepN")) {
			const int keepN = settings->value(key + "_multiple_keepN", 1).toInt();
			ret.append(Token(QStringList(value.mid(0, qMax(1, keepN)))));
		} else if (whatToDo == QLatin1String("keepNThenAdd")) {
			const int keepN = settings->value(key + "_multiple_keepNThenAdd_keep", 1).toInt();
			QString thenAdd = settings->value(key + "_multiple_keepNThenAdd_add", " (+ %count%)").toString();
			thenAdd.replace("%total%", QString::number(value.size()));
			thenAdd.replace("%count%", QString::number(value.size() - keepN));
			QStringList keptValues = value.mid(0, qMax(1, keepN));
			if (value.size() > keepN) {
				ret.append(Token(keptValues.join(' ') + thenAdd));
			} else {
				ret.append(Token(keptValues));
			}
		} else {
			ret.append(Token(settings->value(key + "_value", token.multipleDefault()).toString()));
		}
	} else {
		ret.append(Token(value));
	}

	settings->endGroup();
	return ret;
}

QList<QMap<QString, Token>> Filename::expandTokens(QMap<QString, Token> tokens, QSettings *settings) const
{
	QList<QMap<QString, Token>> ret;
	ret.append(tokens);

	const bool isJavascript = m_format.startsWith(QLatin1String("javascript:"));
	for (const QString &key : tokens.keys()) {
		const Token &token = tokens[key];
		if (token.value().type() != QVariant::StringList) {
			continue;
		}

		const bool hasToken = !isJavascript && m_ast->tokens().contains(key);
		const bool hasVar = isJavascript && m_format.contains(key);
		if (!hasToken && !hasVar) {
			continue;
		}

		QList<Token> reps = getReplace(key, token, settings);
		const int cnt = ret.count();
		for (int i = 0; i < cnt; ++i) {
			ret[i].insert(key, reps[0]);
			for (int j = 1; j < reps.count(); ++j) {
				tokens = ret[i];
				tokens.insert(key, reps[j]);
				ret.append(tokens);
			}
		}
	}

	return ret;
}

QStringList Filename::path(const Image &img, Profile *profile, const QString &pth, int counter, PathFlags flags) const
{ return path(img.tokens(profile), profile, pth, counter, flags); }
QStringList Filename::path(QMap<QString, Token> tokens, Profile *profile, QString folder, int counter, PathFlags flags) const
{
	if (m_ast->ast() == nullptr) {
		return QStringList();
	}

	QSettings *settings = profile->getSettings();

	// Computed tokens
	tokens.insert("count", Token(counter));
	tokens.insert("current_date", Token(QDateTime::currentDateTime()));

	// Conditional filenames
	if (flags.testFlag(PathFlag::ConditionalFilenames)) {
		QList<ConditionalFilename> filenames = getFilenames(settings);
		for (const auto &fn : filenames) {
			if (fn.matches(tokens, settings)) {
				if (!fn.path.isEmpty()) {
					folder = fn.path;
				}
				if (!fn.filename.format().isEmpty()) {
					return fn.filename.path(tokens, profile, folder, counter, flags & (~PathFlag::ConditionalFilenames));
				}
			}
		}
	}

	QStringList fns;

	if (m_format.isEmpty()) {
		return fns;
	}

	// Expand tokens into multiple filenames
	QList<QMap<QString, Token>> replacesList = expandTokens(tokens, settings);

	for (const auto &replaces : replacesList) {
		FilenameExecutionVisitor executionVisitor(replaces, settings);
		executionVisitor.setEscapeMethod(m_escapeMethod);
		executionVisitor.setKeepInvalidTokens(flags.testFlag(PathFlag::KeepInvalidTokens));
		// TODO(Bionus): PathFlag::ExpandConditionals
		QString cFilename = executionVisitor.run(*m_ast->ast());

		// Something wrong happened (JavaScript error...)
		if (cFilename.isEmpty()) {
			continue;
		}

		// "num" special token
		QRegularExpression rxNum("%num(?::.+)?%");
		auto numMatch = rxNum.match(cFilename);
		if (numMatch.hasMatch()) {
			FilenameParser parser(numMatch.captured());
			FilenameNodeVariable *var = parser.parseVariable();

			int num = 1;
			const QString hasNum = numMatch.captured();
			const bool noExt = var->opts.contains("noext");

			const int mid = QDir::toNativeSeparators(cFilename).lastIndexOf(QDir::separator());
			QDir dir(folder + (mid >= 0 ? QDir::separator() + cFilename.left(mid) : QString()));
			const QString cRight = mid >= 0 ? cFilename.right(cFilename.length() - mid - 1) : cFilename;
			QString filter = QString(cRight).replace(hasNum, "*");
			if (noExt) {
				const QString ext = replaces["ext"].toString();
				if (filter.endsWith("." + ext)) {
					filter = filter.left(filter.length() - ext.length()) + "*";
				}
			}
			QFileInfoList allFiles = dir.entryInfoList(QStringList() << filter, QDir::Files, QDir::NoSort);

			QFileInfoList files;
			for (const auto &file : allFiles) {
				if (!file.fileName().endsWith(".tmp")) {
					files.append(file);
				}
			}

			if (!files.isEmpty()) {
				// Get last file
				QCollator collator;
				collator.setNumericMode(true);
				const QFileInfo highest = noExt
					? *std::max_element(
						files.begin(),
						files.end(),
						[&collator](const QFileInfo &a, const QFileInfo &b) { return collator.compare(a.completeBaseName(), b.completeBaseName()) < 0; }
					)
					: *std::max_element(
						files.begin(),
						files.end(),
						[&collator](const QFileInfo &a, const QFileInfo &b) { return collator.compare(a.fileName(), b.fileName()) < 0; }
					);

				const QString last = highest.fileName();
				const int pos = cRight.indexOf(hasNum);
				const int len = last.length() - cRight.length() + hasNum.length();
				num = last.midRef(pos, len).toInt() + 1;
			}

			QString val = executionVisitor.variableToString(var->name, num, var->opts);
			cFilename.replace(numMatch.capturedStart(), numMatch.capturedLength(), val);
		}

		fns.append(cFilename);
	}

	const int cnt = fns.count();
	for (int i = 0; i < cnt; ++i) {
		if (flags.testFlag(PathFlag::Fix)) {
			// Trim directory names
			fns[i] = fns[i].trimmed();
			fns[i].replace(QRegularExpression(" */ *"), "/");

			// Max filename size option
			const int limit = !flags.testFlag(PathFlag::CapLength) ? 0 : settings->value("Save/limit").toInt();
			fns[i] = fixFilename(fns[i], folder, limit);
		}

		// Include directory in result
		if (flags.testFlag(PathFlag::IncludeFolder)) {
			fns[i] = folder + "/" + fns[i];
		}

		if (flags.testFlag(PathFlag::Fix)) {
			// Native separators
			fns[i] = QDir::toNativeSeparators(fns[i]);

			// We remove empty directory names
			const QChar sep = QDir::separator();
			fns[i].replace(QRegularExpression("(.)" + QRegularExpression::escape(sep) + "{2,}"), QString("\\1") + sep);
		}
	}

	return fns;
}

QString Filename::format() const
{
	return m_format;
}
void Filename::setFormat(const QString &format)
{
	m_format = format;
	m_ast = FilenameCache::Get(format);
}

void Filename::setEscapeMethod(QString (*escapeMethod)(const QVariant &))
{
	m_escapeMethod = escapeMethod;
}

bool Filename::returnError(const QString &msg, QString *error) const
{
	if (error != nullptr) {
		*error = msg;
	}

	return false;
}
bool Filename::isValid(Profile *profile, QString *error) const
{
	static const QString red = QStringLiteral("<span style=\"color:red\">%1</span>");
	static const QString orange = QStringLiteral("<span style=\"color:orange\">%1</span>");
	static const QString green = QStringLiteral("<span style=\"color:green\">%1</span>");

	// Field must be filled
	if (m_format.isEmpty()) {
		return returnError(red.arg(QObject::tr("Filename must not be empty!")), error);
	}

	// Can't validate javascript expressions
	if (m_format.startsWith("javascript:")) {
		returnError(orange.arg(QObject::tr("Can't validate Javascript expressions.")), error);
		return true;
	}

	// Can't validate invalid grammar
	if (!m_ast->error().isEmpty()) {
		return returnError(red.arg(QObject::tr("Can't compile your filename: %1").arg(m_ast->error())), error);
	}

	const auto &toks = m_ast->tokens();

	// Field must end by an extension
	if (!m_format.endsWith(".%ext%")) {
		return returnError(orange.arg(QObject::tr("Your filename doesn't ends by an extension, symbolized by %ext%! You may not be able to open saved files.")), error);
	}

	// Field must contain an unique token
	if (!toks.contains("md5") && !toks.contains("id") && !toks.contains("num")) {
		return returnError(orange.arg(QObject::tr("Your filename is not unique to each image and an image may overwrite a previous one at saving! You should use%md5%, which is unique to each image, to avoid this inconvenience.")), error);
	}

	// Looking for unknown tokens
	QStringList tokens = QStringList() << "tags" << "artist" << "general" << "copyright" << "character" << "model" << "photo_set" << "species" << "meta" << "filename" << "rating" << "md5" << "website" << "websitename" << "ext" << "all" << "id" << "search" << "search_(\\d+)" << "allo" << "date" << "score" << "count" << "width" << "height" << "pool" << "url_file" << "url_page" << "num" << "name" << "position" << "current_date";
	if (profile != nullptr) {
		tokens.append(profile->getAdditionalTokens());
		tokens.append(getCustoms(profile->getSettings()).keys());
	}
	static const QRegularExpression rx("%(.+?)%");
	auto matches = rx.globalMatch(m_format);
	while (matches.hasNext()) {
		auto match = matches.next();
		bool found = false;
		for (const QString &token : tokens) {
			if (QRegularExpression("%(?:gallery\\.)?" + token + "(?::[^%]+)?%").match(match.captured(0)).hasMatch()) {
				found = true;
			}
		}

		if (!found) {
			return returnError(orange.arg(QObject::tr("The %%1% token does not exist and will not be replaced.").arg(match.captured(1))), error);
		}
	}

	// Check for invalid windows characters
	#ifdef Q_OS_WIN
		QString txt = QString(m_format).remove(rx);
		if (txt.contains(':') || txt.contains('*') || txt.contains('?') || (txt.contains('"') && txt.count('<') == 0) || txt.count('<') != txt.count('>') || txt.contains('|')) {
			return returnError(red.arg(QObject::tr("Your format contains characters forbidden on Windows! Forbidden characters: * ? \" : < > |")), error);
		}
	#endif

	// Check if code is unique
	if (!toks.contains("md5") && !toks.contains("website") && !toks.contains("websitename") && !toks.contains("count") && toks.contains("id")) {
		return returnError(green.arg(QObject::tr("You have chosen to use the %id% token. Know that it is only unique for a selected site. The same ID can identify different images depending on the site.")), error);
	}

	// All tests passed
	returnError(green.arg(QObject::tr("Valid filename!")), error);
	return true;
}

bool Filename::needTemporaryFile(const QMap<QString, Token> &tokens) const
{
	if (m_format.startsWith("javascript:")) {
		return false;
	}

	const auto &toks = m_ast->tokens();

	return (
		(toks.contains("md5") && (!tokens.contains("md5") || tokens["md5"].value().toString().isEmpty())) ||
		(toks.contains("filesize") && (!tokens.contains("filesize") || tokens["filesize"].value().toInt() <= 0)) ||
		(toks.contains("width") && (!tokens.contains("width") || tokens["width"].value().toInt() <= 0)) ||
		(toks.contains("height") && (!tokens.contains("height") || tokens["height"].value().toInt() <= 0))
	);
}

int Filename::needExactTags(Site *site, const QString &api) const
{
	Q_UNUSED(api);

	QStringList forcedTokens = site != nullptr
		? site->getApis().first()->forcedTokens()
		: QStringList();

	if (forcedTokens.contains("*")) {
		return 2;
	}

	return needExactTags(forcedTokens);
}
int Filename::needExactTags(const QStringList &forcedTokens) const
{
	// Javascript filenames always need tags as we don't know what they might do
	if (m_format.startsWith("javascript:")) {
		return 2;
	}

	const auto &toks = m_ast->tokens();

	// If we need the filename and it is returned from the details page
	if (toks.contains("filename") && forcedTokens.contains("filename")) {
		return 2;
	}

	// If we need the date and it is returned from the details page
	if (toks.contains("date") && forcedTokens.contains("date")) {
		return 2;
	}

	// The filename contains one of the special tags
	QStringList forbidden = QStringList() << "artist" << "copyright" << "character" << "model" << "photo_set" << "species" << "meta" << "general";
	for (const QString &token : forbidden) {
		if (toks.contains(token)) {
			return 1;
		}
	}

	// Namespaces come from detailed tags
	if (m_format.contains("includenamespace")) {
		return 1;
	}

	return 0;
}
