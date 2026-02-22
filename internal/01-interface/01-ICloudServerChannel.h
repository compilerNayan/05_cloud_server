/**
 * @file 01-ICloudServerChannel.h
 * @brief Interface for the cloud server channel: receive requests and send responses.
 */

#ifndef CLOUD_SERVER_ICLOUD_SERVER_CHANNEL_H
#define CLOUD_SERVER_ICLOUD_SERVER_CHANNEL_H

#include <StandardDefines.h>
#include <IHttpRequest.h>

DefineStandardPointers(ICloudServerChannel)
class ICloudServerChannel {
    Public virtual ~ICloudServerChannel() = default;

    /**
     * Receive one request from the cloud server, if available.
     * @return IHttpRequestPtr if a request was received, nullptr otherwise
     */
    Public virtual Bool ProcessRequest() = 0;

    /**
     * Send a response to the client identified by requestId.
     * @param requestId Unique request ID (e.g. from the received IHttpRequest)
     * @param message Response body to send
     * @return true if sent successfully, false otherwise
     */
    Public virtual Bool ProcessResponse() = 0;

    Public Virtual Bool ProcessRequestAndResponse() = 0;
};

#endif  // CLOUD_SERVER_ICLOUD_SERVER_CHANNEL_H
