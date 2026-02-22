/**
 * @file 01-CloudServerThread.h
 * @brief IRunnable that runs ProcessRequestAndResponse on ICloudServerChannel in an infinite loop.
 */

#ifndef CLOUD_SERVER_CLOUD_SERVER_THREAD_H
#define CLOUD_SERVER_CLOUD_SERVER_THREAD_H

#include <IRunnable.h>
#include <Thread.h>
#include "../01-interface/01-ICloudServerChannel.h"

class CloudServerThread final : public IRunnable {

    /* @Autowired */
    Private ICloudServerChannelPtr cloudServerChannel;

    Public Void Run() override {
        cloudServerChannel->ProcessRequestAndResponse();
        Thread::Sleep(100);
    }
};

#endif  // CLOUD_SERVER_CLOUD_SERVER_THREAD_H
