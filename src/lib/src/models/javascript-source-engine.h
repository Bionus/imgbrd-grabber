#ifndef JAVASCRIPT_SOURCE_ENGINE_H
#define JAVASCRIPT_SOURCE_ENGINE_H

#include "source-engine.h"
#include <QFileSystemWatcher>
#include <QJSValue>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include "tags/tag-name-format.h"


class Api;
class Auth;
class QJSEngine;
class QMutex;

/**
 * Represents a JavaScript engine for a source, usually stored in a "model.js" file.
 *
 * @see https://bionus.github.io/imgbrd-grabber/docs/sites/source.html
 */
class JavaScriptSourceEngine : public SourceEngine
{
	Q_OBJECT

	public:
		/**
		 * Create a JavaScript source for a given file.
		 * @param path The JavaScript file to load.
		 * @param helperFile The JavaScript "helper.js" file necessary for JS sources to run.
		 */
		explicit JavaScriptSourceEngine(QString path, QString helperFile, QObject *parent = nullptr);
		~JavaScriptSourceEngine() override;

		/**
		 * A JavaScript source is considered valid if the file could be parsed properly and has at least one API.
		 * @return Whether this JavaScript source is valid.
		 */
		bool isValid() const override;

		// Getters
		const QString &getName() const override;
		const QList<Api*> &getApis() const override;
		const QMap<QString, Auth*> &getAuths() const override;
		const QStringList &getAdditionalTokens() const override;

	private:
		void load();
		QJSEngine *jsEngine();
		QMutex *jsEngineMutex();

	private slots:
		void reload();

	private:
		// Input
		QString m_path;
		QString m_helperFile;

		// Parsed information
		QJSValue m_jsSource;
		QString m_name;
		QList<Api*> m_apis;
		QMap<QString, Auth*> m_auths;
		QStringList m_additionalTokens;
		TagNameFormat m_tagNameFormat; // Unused

		QFileSystemWatcher m_watcher;
};

#endif // JAVASCRIPT_SOURCE_ENGINE_H
