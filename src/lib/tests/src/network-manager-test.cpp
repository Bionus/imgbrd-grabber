#include <QEventLoop>
#include <QNetworkRequest>
#include <QUrl>
#include "catch.h"
#include "network/network-manager.h"
#include "network/network-reply.h"
#include "functions.h"


TEST_CASE("NetworkManager", "[.][network]")
{
	setTestModeEnabled(false);

	/**
	 * Start a request that should take 300ms.
	 * Wait for 2s.
	 * The request should have been received and the active query count reset.
	 */
	SECTION("Basic")
	{
		NetworkManager manager;
		manager.setMaxConcurrency(1);

		REQUIRE(manager.activeQueries() == 0);

		QNetworkRequest request(QUrl("https://httpbin.org/delay/0.1"));
		NetworkReply *reply = manager.get(request);

		bool received = false;
		QObject::connect(reply, &NetworkReply::finished, [&]() {
			received = true;
		});

		QEventLoop loop;
		QTimer::singleShot(2000, &loop, &QEventLoop::quit);
		loop.exec();

		REQUIRE(received);
		REQUIRE(manager.activeQueries() == 0);
	}

	/**
	 * Start a request that should take 300ms.
	 * Abort the request after 100ms.
	 * Wait for 2s.
	 * The request should have been finished (without data) and the active query count reset.
	 */
	SECTION("Abort after starting")
	{
		NetworkManager manager;
		manager.setMaxConcurrency(1);

		REQUIRE(manager.activeQueries() == 0);

		QNetworkRequest request(QUrl("https://httpbin.org/delay/0.3"));
		NetworkReply *reply = manager.get(request);

		bool received = false;
		QObject::connect(reply, &NetworkReply::finished, [&]() {
			received = true;
		});
		QTimer::singleShot(100, [&]() {
			reply->abort();
		});

		QEventLoop loop;
		QTimer::singleShot(2000, &loop, &QEventLoop::quit);
		loop.exec();

		REQUIRE(received);
		REQUIRE(manager.activeQueries() == 0);
	}

	/**
	 * Start a request that should take 300ms and should start in 2s.
	 * Abort the request after 100ms.
	 * Wait for 5s.
	 * The request should have been finished (without data) and the active query count reset.
	 */
	SECTION("Abort before starting")
	{
		NetworkManager manager;
		manager.setMaxConcurrency(1);
		manager.setInterval(-1, 2000);

		REQUIRE(manager.activeQueries() == 0);

		// Start a random request to trigger the interval
		QNetworkRequest fakeRequest(QUrl("https://httpbin.org/status/200"));
		manager.get(fakeRequest);

		QNetworkRequest request(QUrl("https://httpbin.org/delay/0.3"));
		NetworkReply *reply = manager.get(request);

		bool received = false;
		QObject::connect(reply, &NetworkReply::finished, [&]() {
			received = true;
		});
		QTimer::singleShot(100, [&]() {
			reply->abort();
		});

		QEventLoop loop;
		QTimer::singleShot(5000, &loop, &QEventLoop::quit);
		loop.exec();

		REQUIRE(received);
		REQUIRE(manager.activeQueries() == 0);
	}

	setTestModeEnabled(true);
}
