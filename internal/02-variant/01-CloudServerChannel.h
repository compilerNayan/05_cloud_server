/**
 * @file 01-CloudServerChannel.h
 * @brief Cloud server channel: receives requests and sends responses. Uses internet status
 * to decide when to restart the server; no explicit Start/Stop. Uses primary (default) server.
 */

#ifndef CLOUD_SERVER_CLOUD_SERVER_CHANNEL_H
#define CLOUD_SERVER_CLOUD_SERVER_CHANNEL_H

#include <IServer.h>
#include <ServerProvider.h>
#include <ILogger.h>
#include <IInternetConnectionStatusProvider.h>
#include <IHttpRequestQueue.h>
#include <IHttpResponseQueue.h>
#include <IHttpResponse.h>
#include <firebase/IFirebaseFacade.h>
#include <string>

#include "../01-interface/01-ICloudServerChannel.h"

/* @Component */
class CloudServerChannel final : public ICloudServerChannel {

    /* @Autowired */
    Private IInternetConnectionStatusProviderPtr internetStatusProvider;
    /* @Autowired */
    Private ILoggerPtr logger;
    /* @Autowired */
    Private IHttpRequestQueuePtr requestQueue;
    /* @Autowired */
    Private IHttpResponseQueuePtr responseQueue;

    /* @Autowired */
    Private IFirebaseFacadePtr firebaseFacade;

    Private IServerPtr server_;

    /** Last known internet connection id; when it changes we restart the server. */
    Private ULong lastInternetConnectionId_{0};

    /**
     * Returns true if we may send/receive: internet connected (id != 0), server restarted if id changed, server valid.
     */
    Private Bool PreCheck() {
        if (server_ == nullptr) {
            logger->Warning(Tag::Untagged, StdString("[CloudServerChannel] PreCheck skip: server is null"));
            return false;
        }
        if (internetStatusProvider == nullptr) {
            logger->Warning(Tag::Untagged, StdString("[CloudServerChannel] PreCheck skip: internetStatusProvider is null"));
            return false;
        }
        if(internetStatusProvider->IsHotspotConnected()) {
            return false;
        }
        if(!internetStatusProvider->IsWiFiConnected()) {
            return false;
        }
        if (!internetStatusProvider->IsInternetConnected()) {
            logger->Info(Tag::Untagged, StdString("[CloudServerChannel] PreCheck skip: no internet"));
            return false;
        }
        ULong internetConnectionId = internetStatusProvider->GetInternetConnectionId();
        if (internetConnectionId == 0) {
            logger->Info(Tag::Untagged, StdString("[CloudServerChannel] PreCheck skip: internet connection id 0"));
            return false;
        }
        if (internetConnectionId != lastInternetConnectionId_) {
            logger->Info(Tag::Untagged, StdString("[CloudServerChannel] Restarting server: internet connection id changed ") + std::to_string(lastInternetConnectionId_) + " -> " + std::to_string(internetConnectionId));
            server_->Stop();
            server_->Start(DEFAULT_SERVER_PORT);
            lastInternetConnectionId_ = internetConnectionId;
        }
        // Hack below. Fix it later
        if(firebaseFacade && firebaseFacade->IsDirty()) {
            logger->Info(Tag::Untagged, StdString("[CloudServerChannel] PreCheck skip: Firebase operations are dirty"));
            server_->Stop();
            server_->Start(DEFAULT_SERVER_PORT);
            return false;
        }
        if (!server_->IsRunning()) {
            logger->Warning(Tag::Untagged, StdString("[CloudServerChannel] PreCheck skip: server not running"));
            return false;
        }
        return true;
    }

    Public CloudServerChannel()
        : server_(ServerProvider::GetDefaultServer()) {
            if (server_) {
                logger->Info(Tag::Untagged, StdString("[CloudServerChannel] Created with server from ServerProvider"));
            }else {
                logger->Error(Tag::Untagged, StdString("[CloudServerChannel] Created but server is null (no server registered?)"));
            }
    }

    Public Bool ProcessRequest() override {
        if (!PreCheck()) return false;
        Val request = server_->ReceiveMessage();
        if (request == nullptr) return true;
        requestQueue->EnqueueRequest(request);
        logger->Info(Tag::Untagged, StdString("[CloudServerChannel] Request received and enqueued"));
        return true;
    }

    Public Bool ProcessResponse() override {
        if (!PreCheck()) return false;
        IHttpResponsePtr response = responseQueue->DequeueCloudResponse();
        if (response == nullptr) return true;
        StdString requestId = response->GetRequestId();
        if (requestId.empty()) {
            logger->Warning(Tag::Untagged, StdString("[CloudServerChannel] ProcessResponse skip: response has empty request id"));
            return false;
        }
        StdString message = response->ToHttpString();
        Bool sent = server_->SendMessage(requestId, message);
        if (logger && sent) logger->Info(Tag::Untagged, StdString("[CloudServerChannel] Response sent for request id ") + requestId);
        return sent;
    }

    Public Bool ProcessRequestAndResponse() override {
        Bool req = ProcessRequest();
        Bool rsp = ProcessResponse();
        return req && rsp;
    }

};

#endif  // CLOUD_SERVER_CLOUD_SERVER_CHANNEL_H
