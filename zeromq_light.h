/*
*
*  MIT License
*
*  Copyright (c) 2017 finixbit <finix@protonmail.com>
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE.
*
*
*/

#ifndef API_ZEROMQ_LIGHT_H
#define API_ZEROMQ_LIGHT_H

#include <cstdint> /* uint32_t */
#include <string>
#include <cstring>
#include <zmq.h>

/*
Zeromq Socket types.
*/
#define ZMQ_PUB  1
#define ZMQ_SUB  2
#define ZMQ_REQ  3
#define ZMQ_REP  4
#define ZMQ_PULL 7
#define ZMQ_PUSH 8

#define DEGUG_ZEROMQ_LIGHT_CXX true


class ZeromqLight {
private:
  std::string zmqConnString;
  bool zqmConnectionStatus;
  void* zmqContext;
  void* zmqSocket;
  uint32_t zmqConnType;

public:
  ZeromqLight();
  ZeromqLight(std::string &_zmqConnString, uint32_t &_zmqConnType);
  ~ZeromqLight(void);
  void init(std::string &_zmqConnString, uint32_t _zmqConnType);
  void zmqBind();
  void zmqConnect();
  bool zmqConnIsInvalid();
  typedef bool (*getDataFuncptr)(char*, uint32_t);
  void zmqRecieveMessage(getDataFuncptr getDataFun);
  bool zmqSendMessage(std::string &data);
  //bool zmqSendMessage(string &topic, string &data);
  void zmqDebugMessage(const char data[]);
};

ZeromqLight::ZeromqLight() {}

ZeromqLight::ZeromqLight(std::string &_zmqConnString, uint32_t &_zmqConnType):
  zmqConnString(_zmqConnString), zmqConnType(_zmqConnType) {
  try {
    zmqContext = zmq_ctx_new();
    zmqSocket  = zmq_socket(zmqContext , zmqConnType);
    zqmConnectionStatus = true;
  } catch (...) {
    zmqDebugMessage("Zeromq Socket Error");
    zqmConnectionStatus = false;
  }
}

ZeromqLight::~ZeromqLight(void) {}

void ZeromqLight::init(std::string &_zmqConnString, uint32_t _zmqConnType) {
  zmqConnString = _zmqConnString;
  zmqConnType = _zmqConnType;
  try {
    zmqContext   = zmq_ctx_new();
    zmqSocket  = zmq_socket(zmqContext , zmqConnType);
    zqmConnectionStatus = true;
  } catch (...) {
    zmqDebugMessage("Zeromq Socket Error");
    zqmConnectionStatus = false;
  }
}

void ZeromqLight::zmqBind() {
  try {
    int32_t rc = -1;
    rc = zmq_bind(zmqSocket , zmqConnString.c_str());
    if (rc == -1) {
      zmqDebugMessage("Zeromq Bind Error - ");
      zqmConnectionStatus = false;
    } else {
      zqmConnectionStatus = true;
    }
  } catch (...) {
    zmqDebugMessage("Zeromq Bind Error - 1");
    zqmConnectionStatus = false;
  }
}

void ZeromqLight::zmqConnect() {
  try {
    int32_t rc = -1;
    rc = zmq_connect(zmqSocket , zmqConnString.c_str());
    if (rc == -1) {
      zmqDebugMessage("Zeromq Connect Error");
      zqmConnectionStatus = false;
    }
    if (zmqConnType == ZMQ_SUB) {
      zmq_setsockopt(zmqSocket, ZMQ_SUBSCRIBE,  "", strlen(""));
    }
  } catch (...) {
    zmqDebugMessage("Zeromq Connect Error");
    zqmConnectionStatus = false;
  }
}

bool ZeromqLight::zmqConnIsInvalid() {
  if (zqmConnectionStatus)
    return false;
  return true;
}

void ZeromqLight::zmqRecieveMessage(getDataFuncptr getDataFun) {
  for (;;) {
    zmq_msg_t reply;
    zmq_msg_init(&reply);
    zmq_msg_recv(&reply, zmqSocket, 0);
    size_t length = zmq_msg_size(&reply);
    
    char* msg = (char*)malloc(sizeof(char) * length);
    memcpy(msg, zmq_msg_data(&reply), length);
    getDataFun(msg, length);
    zmq_msg_close(&reply);
    free(msg);
  }
}

bool ZeromqLight::zmqSendMessage(std::string &data) {
  try {
    zmq_msg_t message;
    zmq_msg_init_size(&message, data.length());
    memcpy(zmq_msg_data(&message), data.c_str(), data.length());
    zmq_msg_send(&message, zmqSocket, 0);
    zmq_msg_close(&message);
  } catch (...) {
    zmqDebugMessage("Zeromq Sending Data Error");
    return false;
  }
  return true;
}

void ZeromqLight::zmqDebugMessage(const char data[]) {
  #ifdef DEGUG_ZEROMQ_LIGHT_CXX
    std::cout << data << std::endl;
  #endif
}

#endif // API_ZEROMQ_LIGHT_H