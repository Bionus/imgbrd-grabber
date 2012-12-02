#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <QString>



class CrashHandlerPrivate;
class CrashHandler
{
	public:
		static CrashHandler* instance();
		void Init(const QString&  reportPath);
		void setReportCrashesToSystem(bool report);
		bool writeMinidump();

	private:
		CrashHandler();
		~CrashHandler();
		Q_DISABLE_COPY(CrashHandler)
		CrashHandlerPrivate* d;
};

#endif // CRASHHANDLER_H
