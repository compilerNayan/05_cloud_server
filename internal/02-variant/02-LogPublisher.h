/**
 * @file 02-LogPublisher.h
 * @brief Publishes logs from ILogBuffer to Firebase via IFirebaseFacade.
 */

#ifndef CLOUD_SERVER_LOG_PUBLISHER_H
#define CLOUD_SERVER_LOG_PUBLISHER_H

#include "../01-interface/02-ILogPublisher.h"
#include <IFirebaseFacade.h>
#include <ILogBuffer.h>
#include <ILogger.h>
#include <IInternetConnectionStatusProvider.h>

/* @Component */
class LogPublisher final : public ILogPublisher {

    /** Max logs per Firebase publish (match Firebase payload limit). */
    Static const Size kMaxLogsPerPublish = 30;

    /* @Autowired */
    Private IFirebaseFacadePtr firebaseFacade;
    /* @Autowired */
    Private ILogBufferPtr logBuffer;

    /* @Autowired */
    Private ILoggerPtr logger;

    /* @Autowired */
    Private IInternetConnectionStatusProviderPtr internetStatusProvider;

    /** Last known internet connection id; when it changes we restart Firebase operations. */
    Private ULong lastInternetConnectionId_{0};

    /**
     * Returns true if we may publish: internet connected (id != 0), Firebase operations restarted if id changed.
     */
    Private Bool PreCheck() {
        if (logger == nullptr) return false;
        if (internetStatusProvider == nullptr) {
            logger->Warning(Tag::Untagged, StdString("[LogPublisher] PreCheck skip: internetStatusProvider is null"));
            return false;
        }
        if (internetStatusProvider->IsHotspotConnected()) return false;
        if (!internetStatusProvider->IsInternetConnected()) {
            logger->Info(Tag::Untagged, StdString("[LogPublisher] PreCheck skip: no internet"));
            return false;
        }
        ULong internetConnectionId = internetStatusProvider->GetInternetConnectionId();
        if (internetConnectionId == 0) {
            logger->Info(Tag::Untagged, StdString("[LogPublisher] PreCheck skip: internet connection id 0"));
            return false;
        }
        if (internetConnectionId != lastInternetConnectionId_) {
            logger->Info(Tag::Untagged, StdString("[LogPublisher] Restarting Firebase operations: internet connection id changed ") + std::to_string(lastInternetConnectionId_) + " -> " + std::to_string(internetConnectionId));
            if (firebaseFacade) {
                firebaseFacade->StopFirebaseOperations();
                firebaseFacade->StartFirebaseOperations();
            }
            lastInternetConnectionId_ = internetConnectionId;
        }
        return true;
    }

    Public Bool PublishLogs() override {
        if (!PreCheck()) return false;
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
