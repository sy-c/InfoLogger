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

#include "InfoLoggerMessageList.h"
#include "infoLoggerMessageDecode.h"

InfoLoggerMessageList::InfoLoggerMessageList(TR_file* rawMessageList)
{
  msg = infoLog_decode(rawMessageList);
  if (msg == nullptr) {
    throw __LINE__;
  }
}

InfoLoggerMessageList::~InfoLoggerMessageList()
{
  infoLog_msg_destroy(msg); // destroy list (ok with null)
}

unsigned long InfoLoggerMessageList::size()
{
  unsigned long nMsg = 0;
  for (infoLog_msg_t* m = this->msg; m != nullptr; m = m->next) {
    nMsg++;
  }
  return nMsg;
}
