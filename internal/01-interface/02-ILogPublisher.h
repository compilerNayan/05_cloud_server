/**
 * @file 02-ILogPublisher.h
 * @brief Interface for publishing logs (e.g. to cloud).
 */

#ifndef CLOUD_SERVER_ILOG_PUBLISHER_H
#define CLOUD_SERVER_ILOG_PUBLISHER_H

#include <StandardDefines.h>

DefineStandardPointers(ILogPublisher)
class ILogPublisher {
    Public virtual ~ILogPublisher() = default;

    /** Publish logs. Returns true if successful. */
    Public virtual Bool PublishLogs() = 0;
};

#endif  // CLOUD_SERVER_ILOG_PUBLISHER_H
