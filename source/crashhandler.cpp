#include "crashhandler.h"
#include <QDir>
#include <QProcess>
#include <QCoreApplication>
#include <QString>
#include <QDebug>
#include "functions.h"
#if defined(Q_OS_MAC)
	#include "client/mac/handler/exception_handler.h"
#elif defined(Q_OS_LINUX)
	#include "client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN)
	#include "client/windows/handler/exception_handler.h"
#endif



/************************************************************************/
/* CrashHandlerPrivate                                                  */
/************************************************************************/

class CrashHandlerPrivate
{
	public:
		CrashHandlerPrivate() { pHandler = NULL; }
		~CrashHandlerPrivate() { delete pHandler; }
		void InitCrashHandler(const QString& dumpPath);
		static google_breakpad::ExceptionHandler* pHandler;
		static bool bReportCrashesToSystem;
};

google_breakpad::ExceptionHandler* CrashHandlerPrivate::pHandler = NULL;
bool CrashHandlerPrivate::bReportCrashesToSystem = false;



/************************************************************************/
/* DumpCallback                                                         */
/************************************************************************/

#if defined(Q_OS_WIN)
bool DumpCallback(const wchar_t* _dump_dir,const wchar_t* _minidump_id,void* context,EXCEPTION_POINTERS* exinfo,MDRawAssertionInfo* assertion,bool success)
#elif defined(Q_OS_LINUX)
bool DumpCallback(const google_breakpad::MinidumpDescriptor &md,void *context, bool success)
#elif defined(Q_OS_MAC)
bool DumpCallback(const char* _dump_dir,const char* _minidump_id,void *context, bool success)
#endif
{
	QString dir, mid;
	Q_UNUSED(context);
	#if defined(Q_OS_WIN)
		dir = QString::fromWCharArray(_dump_dir);
		mid = QString::fromWCharArray(_minidump_id);
		Q_UNUSED(_dump_dir);
		Q_UNUSED(_minidump_id);
		Q_UNUSED(assertion);
		Q_UNUSED(exinfo);
	#elif defined(Q_OS_LINUX)
		dir = QString(md.directory().c_str());
		mid = QString::number(md.fd());
	#elif defined(Q_OS_MAC)
		dir = QString::fromWCharArray(_dump_dir);
		mid = QString::fromWCharArray(_minidump_id);
	#endif

	log(QObject::tr("Minidump sauvegardé dans le dossier \"%1\" avec l'id \"%2\" (%3)").arg(dir, mid, success ? QObject::tr("réussite") : QObject::tr("échec")));
	if (success)
	{
		QFile f(savePath("lastdump"));
		if (f.open(QFile::WriteOnly))
		{
			f.write(QDir::toNativeSeparators(dir+"/"+mid+".dmp").toAscii());
			f.close();
		}
	}
	QProcess::startDetached("CrashReporter.exe");

	return CrashHandlerPrivate::bReportCrashesToSystem ? success : true;
}

void CrashHandlerPrivate::InitCrashHandler(const QString& dumpPath)
{
	if (pHandler != NULL)
	{ return; }

	#if defined(Q_OS_WIN)
		std::wstring pathAsStr = (const wchar_t*)dumpPath.utf16();
		pHandler = new google_breakpad::ExceptionHandler(
			pathAsStr,
			/*FilterCallback*/ 0,
			DumpCallback,
			/*context*/
			0,
			true
			);
	#elif defined(Q_OS_LINUX)
		std::string pathAsStr = dumpPath.toStdString();
		google_breakpad::MinidumpDescriptor md(pathAsStr);
		pHandler = new google_breakpad::ExceptionHandler(
			md,
			/*FilterCallback*/ 0,
			DumpCallback,
			/*context*/ 0,
			true,
			-1
			);
	#elif defined(Q_OS_MAC)
		std::string pathAsStr = dumpPath.toStdString();
		pHandler = new google_breakpad::ExceptionHandler(
			pathAsStr,
			/*FilterCallback*/ 0,
			DumpCallback,
			/*context*/
			0,
			true,
			NULL
			);
	#endif
}



/************************************************************************/
/* CrashHandler                                                         */
/************************************************************************/

CrashHandler* CrashHandler::instance()
{
	static CrashHandler globalHandler;
	return &globalHandler;
}

CrashHandler::CrashHandler()
{ d = new CrashHandlerPrivate(); }

CrashHandler::~CrashHandler()
{ delete d; }

void CrashHandler::setReportCrashesToSystem(bool report)
{ d->bReportCrashesToSystem = report; }

bool CrashHandler::writeMinidump()
{
	bool res = d->pHandler->WriteMinidump();
	if (res)
	{ qDebug("BreakpadQt: writeMinidump() successed."); }
	else
	{ qWarning("BreakpadQt: writeMinidump() failed."); }
	return res;
}

void CrashHandler::Init( const QString& reportPath )
{ d->InitCrashHandler(reportPath); }
