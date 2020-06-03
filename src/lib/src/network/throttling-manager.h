#ifndef THROTTLING_MANAGER_H
#define THROTTLING_MANAGER_H

#include <QDateTime>
#include <QMap>


class NetworkReply;

class ThrottlingManager
{
	public:
		ThrottlingManager() = default;

		int interval(int key) const;
		void setInterval(int key, int msInterval);
		void clear();

		int msToRequest(int key) const;
		void start(int key, NetworkReply *reply);

	private:
		QMap<int, int> m_intervals;
		QMap<int, QDateTime> m_lastRequests;
};

#endif // THROTTLING_MANAGER_H
