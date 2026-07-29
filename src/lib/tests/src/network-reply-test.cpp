#include <QEventLoop>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#include "catch.h"
#include "custom-network-access-manager.h"
#include "network/network-reply.h"


static void drainEventLoop(int ms = 50)
{
	QEventLoop loop;
	QTimer::singleShot(ms, &loop, &QEventLoop::quit);
	loop.exec();
}

/**
 * Regression tests for the NetworkReply state machine.
 *
 * These never dispatch a request, so they need no network and run by default -
 * unlike the "[.][network]" NetworkManager tests, which Catch2 skips and which
 * depend on httpbin.org, and so never caught any of this.
 */
TEST_CASE("NetworkReply", "[network-reply]")
{
	CustomNetworkAccessManager manager;
	const QNetworkRequest request(QUrl("http://localhost/never-dispatched"));

	SECTION("Aborting before dispatch emits nothing")
	{
		NetworkReply reply(request, &manager);

		int finishedCount = 0;
		QObject::connect(&reply, &NetworkReply::finished, [&]() { finishedCount++; });

		reply.abort();
		drainEventLoop();

		// Callers that abort() deliberately do not want a completion callback, and a
		// synchronous emit would re-enter them mid-teardown
		// (PageApi::parse -> setReply -> abort -> parse).
		REQUIRE(finishedCount == 0);
	}

	SECTION("An aborted reply never reports itself as running")
	{
		NetworkReply reply(request, &manager);
		REQUIRE(reply.isRunning());

		reply.abort();
		REQUIRE_FALSE(reply.isRunning());

		// The throttling manager can still call start() on a reply that was aborted
		// while queued. This used to clear the aborted flag and return without
		// starting, leaving a reply that reported isRunning() forever with no request
		// in flight, no timer pending, and no signal it would ever emit - so its
		// concurrency slot was never released and downloads hung until restart.
		reply.start(0);
		drainEventLoop();

		REQUIRE_FALSE(reply.isRunning());
	}

	SECTION("Reading an aborted reply yields no data instead of a closed-device read")
	{
		NetworkReply reply(request, &manager);
		reply.abort();
		drainEventLoop();

		REQUIRE(reply.readAll().isEmpty());
	}
}
