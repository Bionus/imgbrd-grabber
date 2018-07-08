#include "models/api/javascript-console-helper.h"
#include <QObject>
#include "logger.h"


JavascriptConsoleHelper::JavascriptConsoleHelper(const QString &prefix, QObject *parent)
	: QObject(parent), m_prefix(prefix)
{}


void JavascriptConsoleHelper::debug(const QString &msg) const
{
	LOG(m_prefix + msg, Logger::Debug);
}

void JavascriptConsoleHelper::error(const QString &msg) const
{
	LOG(m_prefix + msg, Logger::Error);
}

void JavascriptConsoleHelper::info(const QString &msg) const
{
	LOG(m_prefix + msg, Logger::Info);
}

void JavascriptConsoleHelper::log(const QString &msg) const
{
	LOG(m_prefix + msg, Logger::Info);
}

void JavascriptConsoleHelper::warn(const QString &msg) const
{
	LOG(m_prefix + msg, Logger::Warning);
}
