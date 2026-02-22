/**
 * @file 02-LogPublisher.h
 * @brief Publishes logs from ILogBuffer to Firebase via IFirebaseFacade.
 */

#ifndef CLOUD_SERVER_LOG_PUBLISHER_H
#define CLOUD_SERVER_LOG_PUBLISHER_H

#include "../01-interface/02-ILogPublisher.h"
#include <IFirebaseFacade.h>
#include <ILogBuffer.h>

/* @Component */
class LogPublisher final : public ILogPublisher {

    /** Max logs per Firebase publish (match Firebase payload limit). */
    Static const Size kMaxLogsPerPublish = 30;

    /* @Autowired */
    Private IFirebaseFacadePtr firebaseFacade;
    /* @Autowired */
    Private ILogBufferPtr logBuffer;

    Public Bool PublishLogs() override {
        if (!firebaseFacade || !logBuffer) return false;
        for (;;) {
            StdMap<ULongLong, StdString> logs = logBuffer->TakeLogsAtMost(kMaxLogsPerPublish);
            if (logs.empty()) return true;
            FirebaseOperationResult res = firebaseFacade->PublishLogs(logs);
            if (res != FirebaseOperationResult::OperationSucceeded) {
                logBuffer->AddLogs(logs);
                return false;
            }
        }
    }
};

#endif  // CLOUD_SERVER_LOG_PUBLISHER_H
