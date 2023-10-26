// #include <stdint.h>
// #include <stdio.h>
// #include <queue>
// #include <vector>
// #include <iostream>
// #include <string.h>
// #include "json/json.h"
// #include <arpa/inet.h>
// using namespace std;

// const uint8_t MY_PROTO_MAGIC = 88;
// const uint32_t MY_PROTO_MAX_SIZE = 10 * 1024 * 1024;  // 10M
// const uint32_t MY_PROTO_HEAD_SIZE = 8;

// typedef enum MyProtoParserStatus {
//   ON_PARSER_INIT = 0,
//   ON_PARSER_HAED = 1,
//   ON_PARSER_BODY = 2,
// } MyProtoParserStatus;

// struct MyProtoHead {
//   uint8_t version;
//   uint8_t magic;
//   uint16_t server;
//   uint32_t len;
// };

// struct MyProtoMsg {
//   MyProtoHead head;
//   Json::Value body;
// };

// void myProtoMsgPrint(MyProtoMsg &msg) {
//   string jsonStr = "";
//   Json::FastWriter fWriter;
//   jsonStr = fWriter.write(msg.body);

//   printf(
//       "Head[version=%d,magic=%d,server=%d,len=%d]\n"
//       "Body:%s",
//       msg.head.version, msg.head.magic, msg.head.server, msg.head.len,
//       jsonStr.c_str());
// }
// /*
//     MyProto
//  */
// class MyProtoEnCode {
//  public:
//   uint8_t *encode(MyProtoMsg *pMsg, uint32_t &len);

//  private:
//   void headEncode(uint8_t *pData, MyProtoMsg *pMsg);
// };

// void MyProtoEnCode::headEncode(uint8_t *pData, MyProtoMsg *pMsg) {
//   *pData = 1;
//   ++pData;

//   *pData = MY_PROTO_MAGIC;
//   ++pData;

//   // head.server
//   *(uint16_t *)pData = htons(pMsg->head.server);
//   pData += 2;

//   // head.len
//   *(uint32_t *)pData = htonl(pMsg->head.len);
// }

// uint8_t *MyProtoEnCode::encode(MyProtoMsg *pMsg, uint32_t &len) {
//   uint8_t *pData = NULL;
//   Json::FastWriter fWriter;

//   string bodyStr = fWriter.write(pMsg->body);

//   len = MY_PROTO_HEAD_SIZE + (uint32_t)bodyStr.size();
//   pMsg->head.len = len;

//   pData = new uint8_t[len];

//   headEncode(pData, pMsg);

//   memcpy(pData + MY_PROTO_HEAD_SIZE, bodyStr.data(), bodyStr.size());

//   return pData;
// }

// /*
//     MyProto解包类
//  */
// class MyProtoDeCode {
//  public:
//   void init();
//   void clear();
//   bool parser(void *data, size_t len);
//   bool empty();
//   MyProtoMsg *front();
//   void pop();
//   bool parseFinish() const { return mCurParserStatus >= ON_PARSER_BODY; }

//  private:
//   bool parserHead(uint8_t **curData, uint32_t &curLen, uint32_t &parserLen,
//                   bool &parserBreak);
//   bool parserBody(uint8_t **curData, uint32_t &curLen, uint32_t &parserLen,
//                   bool &parserBreak);

//  private:
//   MyProtoMsg mCurMsg;                    // 当前解析中的协议消息体
//   queue<MyProtoMsg *> mMsgQ;             // 解析好的协议消息队列
//   vector<uint8_t> mCurReserved;          // 未解析的网络字节流
//   MyProtoParserStatus mCurParserStatus;  // 当前解析状态
// };

// void MyProtoDeCode::init() { mCurParserStatus = ON_PARSER_INIT; }

// void MyProtoDeCode::clear() {
//   MyProtoMsg *pMsg = NULL;

//   while (!mMsgQ.empty()) {
//     pMsg = mMsgQ.front();
//     delete pMsg;
//     mMsgQ.pop();
//   }
// }

// bool MyProtoDeCode::parserHead(uint8_t **curData, uint32_t &curLen,
//                                uint32_t &parserLen, bool &parserBreak) {
//   parserBreak = false;
//   if (curLen < MY_PROTO_HEAD_SIZE) {
//     parserBreak = true;
//     return true;
//   }

//   uint8_t *pData = *curData;

//   mCurMsg.head.version = *pData;
//   pData + 2;
//   mCurMsg.head.magic = *pData;
//   pData + 2;

//   if (MY_PROTO_MAGIC != mCurMsg.head.magic) {
//     return false;
//   }

//   mCurMsg.head.server = ntohs(*(uint16_t *)pData);
//   pData += 2;

//   mCurMsg.head.len = ntohl(*(uint32_t *)pData);

//   if (mCurMsg.head.len > MY_PROTO_MAX_SIZE) {
//     return false;
//   }

//   (*curData) += MY_PROTO_HEAD_SIZE;
//   curLen -= MY_PROTO_HEAD_SIZE;
//   parserLen += MY_PROTO_HEAD_SIZE;
//   mCurParserStatus = ON_PARSER_HAED;

//   return true;
// }

// bool MyProtoDeCode::parserBody(uint8_t **curData, uint32_t &curLen,
//                                uint32_t &parserLen, bool &parserBreak) {
//   parserBreak = false;
//   uint32_t jsonSize = mCurMsg.head.len - MY_PROTO_HEAD_SIZE;
//   if (curLen < jsonSize) {
//     parserBreak = true;  // 终止解析
//     return true;
//   }

//   Json::Reader reader;  // json解析类
//   if (!reader.parse((char *)(*curData), (char *)((*curData) + jsonSize),
//                     mCurMsg.body, false)) {
//     return false;
//   }

//   // 解析指针向前移动jsonSize字节
//   (*curData) += jsonSize;
//   curLen -= jsonSize;
//   parserLen += jsonSize;
//   mCurParserStatus = ON_PARSER_BODY;

//   return true;
// }

// bool MyProtoDeCode::parser(void *data, size_t len) {
//   if (len <= 0) {
//     return false;
//   }

//   uint32_t curLen = 0;
//   uint32_t parserLen = 0;
//   uint8_t *curData = NULL;

//   curData = (uint8_t *)data;
//   // 把当前要解析的网络字节流写入未解析完字节流之后
//   while (len--) {
//     mCurReserved.push_back(*curData);
//     ++curData;
//   }

//   curLen = mCurReserved.size();
//   curData = (uint8_t *)&mCurReserved[0];

//   while (curLen > 0) {
//     bool parserBreak = false;
//     // 解析协议头
//     if (ON_PARSER_INIT == mCurParserStatus ||
//         ON_PARSER_BODY == mCurParserStatus) {
//       if (!parserHead(&curData, curLen, parserLen, parserBreak)) {
//         return false;
//       }

//       if (parserBreak) break;
//     }

//     // 解析完协议头，解析协议体
//     if (ON_PARSER_HAED == mCurParserStatus) {
//       if (!parserBody(&curData, curLen, parserLen, parserBreak)) {
//         return false;
//       }

//       if (parserBreak) break;
//     }

//     if (ON_PARSER_BODY == mCurParserStatus) {
//       MyProtoMsg *pMsg = NULL;
//       pMsg = new MyProtoMsg;
//       *pMsg = mCurMsg;
//       mMsgQ.push(pMsg);
//     }
//   }

//   if (parserLen > 0) {
//     mCurReserved.erase(mCurReserved.begin(), mCurReserved.begin() +
//     parserLen);
//   }

//   return true;
// }

// bool MyProtoDeCode::empty() { return mMsgQ.empty(); }

// MyProtoMsg *MyProtoDeCode::front() {
//   MyProtoMsg *pMsg = NULL;
//   pMsg = mMsgQ.front();
//   return pMsg;
// }

// void MyProtoDeCode::pop() { mMsgQ.pop(); }

// int main() {
//   uint32_t len = 0;
//   uint8_t *pData = NULL;
//   MyProtoMsg msg1;
//   MyProtoMsg msg2;
//   MyProtoDeCode myDecode;
//   MyProtoEnCode myEncode;

//   msg1.head.server = 1;
//   msg1.body["op"] = "set";
//   msg1.body["key"] = "id";
//   msg1.body["value"] = "9856";

//   msg2.head.server = 2;
//   msg2.body["op"] = "get";
//   msg2.body["key"] = "id";

//   myDecode.init();
//   pData = myEncode.encode(&msg1, len);
//   myDecode.parser(pData, 3);
//   if (!myDecode.parseFinish()) {
//     std::cout << "not Finish" << std::endl;
//   } else {
//     std::cout << "parse Finish" << std::endl;
//   }

//   myDecode.parser(pData + 3, len - 3);
//   if (!myDecode.parseFinish()) {
//     std::cout << "not Finish" << std::endl;
//   } else {
//     std::cout << "parse Finish" << std::endl;
//   }

//   pData = myEncode.encode(&msg2, len);
//   if (!myDecode.parser(pData, len)) {
//     cout << "parser falied!" << endl;
//   } else {
//     cout << "msg2 parser successful!" << endl;
//   }

//   MyProtoMsg *pMsg = NULL;
//   while (!myDecode.empty()) {
//     pMsg = myDecode.front();
//     myProtoMsgPrint(*pMsg);
//     myDecode.pop();
//   }

//   return 0;
// }

int main() {}