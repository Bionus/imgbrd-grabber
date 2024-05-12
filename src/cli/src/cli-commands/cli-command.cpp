#include "cli-command.h"
#include <QEventLoop>
#include <QObject>
#include <QTimer>


CliCommand::CliCommand(QObject *parent)
	: QObject(parent)
{}


int CliCommand::execute()
{
	// If the command fails validation, we early return (whether to show help or not is up to the specific command)
	if (!validate()) {
		return 1;
	}

	int ret = 0;

	// Run the command in an event loop to get the exit code "synchronously"
	QEventLoop loop;
	QObject::connect(this, &CliCommand::finished, [&](int exitCode) {
		ret = exitCode;
		loop.quit();
	});
	QTimer::singleShot(0, this, &CliCommand::run);
	loop.exec();

	return ret;
}
