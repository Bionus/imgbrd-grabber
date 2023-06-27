#ifndef USER_AGENT_H
#define USER_AGENT_H

#include <QString>


/**
 * User-Agent HTTP header generator based on Chromium implementation.
 * https://source.chromium.org/chromium/chromium/src/+/refs/heads/main:content/common/user_agent.cc
 */
QString buildUserAgent();

/**
 * Create an User-Agent for the specified product name/version.
*/
QString buildUserAgentForProduct(const QString &product);

#endif // USER_AGENT_H
