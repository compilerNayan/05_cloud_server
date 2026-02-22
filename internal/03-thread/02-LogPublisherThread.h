/**
 * @file 02-LogPublisherThread.h
 * @brief IRunnable that calls ILogPublisher::PublishLogs() every 2 minutes.
 *        Call Run() every loop from main thread; throttled so publish runs at most every 2 mins.
 */

#ifndef CLOUD_SERVER_LOG_PUBLISHER_THREAD_H
#define CLOUD_SERVER_LOG_PUBLISHER_THREAD_H

#include <StandardDefines.h>
#include <IRunnable.h>
#include <osal/Core.h>
#include <ILogger.h>

#include "../01-interface/02-ILogPublisher.h"

/** Interval between publish runs (ms). */
static constexpr ULong kLogPublisherIntervalMs = 2 * 60 * 1000;

class LogPublisherThread final : public IRunnable {

    /* @Autowired */
    Private ILogPublisherPtr logPublisher;

    /* @Autowired */
    Private ILoggerPtr logger;

    Public Void Run() override {
        while (true) {
            logger->Info(Tag::Untagged, StdString("[LogPublisherThread] Publishing logs"));
            logPublisher->PublishLogs();
            Thread::Sleep(60 * 1000);
        }
    }
};

#endif  // CLOUD_SERVER_LOG_PUBLISHER_THREAD_H
