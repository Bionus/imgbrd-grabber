#include "helpers.h"
#include <QLayout>
#include <QMessageBox>
#if defined(Q_OS_WIN)
	#include <comdef.h>
	#include <QDir>
	#include <ShlObj.h>
	#include <Windows.h>
	#include "logger.h"
#elif defined(Q_OS_MAC)
	#include <QProcess>
	#include <QStringList>
#else
	#include <QDesktopServices>
	#include <QUrl>
#endif
#include <QWidget>


/**
 * Popup a message notifying the user that something went wrong.
 * @param	parent	The parent widget
 * @param	error	The error message
 */
void error(QWidget *parent, const QString &error)
{
	QMessageBox::critical(parent, QObject::tr("Error"), error);
}

/**
 * Opens the file explorer and select the file.
 * @param	pathIn	The path to the file.
 */
void showInGraphicalShell(const QString &pathIn)
{
	// Mac & Windows support folder or file.
	#if defined(Q_OS_WIN)
		QString path = QDir::toNativeSeparators(pathIn).toStdString().c_str();
		auto *filename = new wchar_t[path.length() + 1];
		path.toWCharArray(filename);
		filename[path.length()] = 0;
		ITEMIDLIST *pidl = nullptr;
		SFGAOF out;
		HRESULT hr = SHParseDisplayName(filename, nullptr, &pidl, SFGAO_FILESYSTEM, &out);
		if (SUCCEEDED(hr)) {
			SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
			ILFree(pidl);
		} else {
			LPCTSTR errMsg = _com_error(hr).ErrorMessage();
			QString msg = QString::fromLatin1(errMsg);
			log(QString("Error parsing path display name for '%1': %2").arg(pathIn, msg), Logger::Error);
		}
	#elif defined(Q_OS_MAC) && !defined(QT_NO_PROCESS)
		QStringList scriptArgs;
		scriptArgs << QLatin1String("-e") << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(pathIn);
		QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
		scriptArgs.clear();
		scriptArgs << QLatin1String("-e") << QLatin1String("tell application \"Finder\" to activate");
		QProcess::execute("/usr/bin/osascript", scriptArgs);
	#else
		QDesktopServices::openUrl(QUrl("file:///" + pathIn));
	#endif
}

void clearLayout(QLayout *layout)
{
	if (layout == nullptr) {
		return;
	}

	while (layout->count() > 0) {
		QLayoutItem *item = layout->takeAt(0);
		if (item->layout() != nullptr) {
			clearLayout(item->layout());
			item->layout()->deleteLater();
		}
		if (item->widget() != nullptr) {
			item->widget()->deleteLater();
		}
		delete item;
	}
}
