#ifndef USER_AGENT_CLIENT_HINTS_H
#define USER_AGENT_CLIENT_HINTS_H

#include <QOperatingSystemVersion>


class QNetworkRequest;
class QString;

/**
 * Basic implementation of the User-Agent Client Hints draft.
 * https://wicg.github.io/ua-client-hints/
 */
class UserAgentClientHints
{
	public:
		explicit UserAgentClientHints();

		void setRequestHeaders(QNetworkRequest &request) const;

		QString arch() const;
		QString bitness() const;
		QString formFactor() const;
		bool mobile() const;
		QString model() const;
		QString platform() const;
		QString platformVersion() const;
		bool wow64() const;

	private:
		QOperatingSystemVersion m_osVersion;
};

#endif // USER_AGENT_CLIENT_HINTS_H
