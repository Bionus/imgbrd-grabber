#include "models/api/javascript-console-helper.h"
#include <QObject>
#include <utility>
#include "logger.h"


JavascriptConsoleHelper::JavascriptConsoleHelper(QString prefix, QObject *parent)
	: QObject(parent), m_prefix(std::move(prefix))
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
