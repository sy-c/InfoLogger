// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "InfoLoggerDispatch.h"

///////////////////////////////////////////
// class InfoLoggerDispatch implementation
///////////////////////////////////////////

InfoLoggerDispatch::InfoLoggerDispatch(ConfigInfoLoggerServer* vConfig, SimpleLog* vLog, std::string vLogPrefix)
{
  logPrefix = vLogPrefix;
  input = std::make_unique<AliceO2::Common::Fifo<std::shared_ptr<InfoLoggerMessageList>>>(vConfig->dbDispatchQueueSize);
  if (vLog != NULL) {
    theLog = vLog;
  } else {
    theLog = &defaultLog;
  }
  theConfig = vConfig;
  dispatchThread = std::make_unique<Thread>(InfoLoggerDispatch::threadCallback, this, "InfoLoggerDispatch", 50000);
  dispatchThread->start();
}

InfoLoggerDispatch::~InfoLoggerDispatch()
{
  dispatchThread->stop();
}

int InfoLoggerDispatch::pushMessage(const std::shared_ptr<InfoLoggerMessageList>& msg)
{
  if (input->isFull()) {
    return -1;
  }
  input->push(msg);
  //theLog->info("push message\n");
  return 0;
}

int InfoLoggerDispatch::customLoop()
{
  return 0;
}

Thread::CallbackResult InfoLoggerDispatch::threadCallback(void* arg)
{
  InfoLoggerDispatch* dPtr = (InfoLoggerDispatch*)arg;
  if (dPtr == NULL) {
    return Thread::CallbackResult::Error;
  }

  if (!dPtr->isReady) {
    return Thread::CallbackResult::Idle;
  }

  int nMsgProcessed = 0;

  if (dPtr->customLoop()) {
    return Thread::CallbackResult::Error;
  }

  while (!dPtr->input->isEmpty()) {
    std::shared_ptr<InfoLoggerMessageList> nextMessage = nullptr;
    dPtr->input->front(nextMessage);
    if (dPtr->customMessageProcess(nextMessage)) {
      return Thread::CallbackResult::Idle;
    }
    dPtr->input->pop(nextMessage);
    nMsgProcessed++;
  }

  if (nMsgProcessed == 0) {
    return Thread::CallbackResult::Idle;
  }
  return Thread::CallbackResult::Ok;
}

int InfoLoggerDispatchPrint::customMessageProcess(std::shared_ptr<InfoLoggerMessageList> msg)
{
  infoLog_msg_print(msg->msg);
  return 0;
}

void InfoLoggerDispatch::logInfo(const char* message, ...) {
  char buffer[1024] = "";
  va_list ap;
  va_start(ap, message);
  vsnprintf(buffer,sizeof(buffer), message, ap);
  va_end(ap);
  theLog->info("%s%s", logPrefix.c_str(), buffer);
}

void InfoLoggerDispatch::logError(const char* message, ...) {
  char buffer[1024] = "";
  va_list ap;
  va_start(ap, message);
  vsnprintf(buffer,sizeof(buffer), message, ap);
  va_end(ap);
  theLog->error("%s%s", logPrefix.c_str(), buffer);
}

void InfoLoggerDispatch::logWarning(const char* message, ...) {
  char buffer[1024] = "";
  va_list ap;
  va_start(ap, message);
  vsnprintf(buffer,sizeof(buffer), message, ap);
  va_end(ap);
  theLog->warning("%s%s", logPrefix.c_str(), buffer);
}

/////////////////////////////////////////////////
// end of class InfoLoggerDispatch implementation
/////////////////////////////////////////////////

