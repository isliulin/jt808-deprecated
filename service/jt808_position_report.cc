// Copyright 2019 Yuming Meng. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "service/jt808_position_report.h"

#include <unistd.h>
#include <string.h>

#include "bcd/bcd.h"
#include "common/jt808_position_report.h"
#include "common/jt808_util.h"
#include "service/jt808_util.h"


void ParsePositionReport(const uint8_t *buffer,
                         const size_t &len, const size_t &offset) {
  uint16_t u16val;
  uint32_t u32val;
  double latitude;
  double longitude;
  float altitude;
  float speed;
  float bearing;
  AlarmBit alarm_bit;
  StatusBit status_bit;
  unsigned char timestamp[6] = {0};
  char phone_num[6] = {0};
  char device_num[12] = {0};

  memcpy(phone_num, &buffer[5], 6);
  StringFromBcdCompress(phone_num, device_num, 6);
  memcpy(&u32val, &buffer[13+offset], 4);
  alarm_bit.value = EndianSwap32(u32val);
  memcpy(&u32val, &buffer[17+offset], 4);
  status_bit.value = EndianSwap32(u32val);
  memcpy(&u32val, &buffer[21+offset], 4);
  latitude = EndianSwap32(u32val) / 1000000.0;
  memcpy(&u32val, &buffer[25+offset], 4);
  longitude = EndianSwap32(u32val) / 1000000.0;
  memcpy(&u16val, &buffer[29+offset], 2);
  altitude = static_cast<float>(EndianSwap16(u16val));
  memcpy(&u16val, &buffer[31+offset], 2);
  speed = static_cast<float>(EndianSwap16(u16val) * 10.0);
  memcpy(&u16val, &buffer[33+offset], 2);
  bearing = static_cast<float>(EndianSwap16(u16val));
  timestamp[0] = HexFromBcd(buffer[35+offset]);
  timestamp[1] = HexFromBcd(buffer[36+offset]);
  timestamp[2] = HexFromBcd(buffer[37+offset]);
  timestamp[3] = HexFromBcd(buffer[38+offset]);
  timestamp[4] = HexFromBcd(buffer[39+offset]);
  timestamp[5] = HexFromBcd(buffer[40+offset]);
  fprintf(stdout, "\tdevice: %s\n"
                  "\talarm flags: %08X\n""\tstatus flags: %08X\n"
                  "\tlongitude: %lf%c\n"
                  "\tlatitude: %lf%c\n"
                  "\taltitude: %f\n"
                  "\tspeed: %f\n""\tbearing: %f\n"
                  "\ttimestamp: %02d-%02d-%02d, %02d:%02d:%02d\n",
          device_num, alarm_bit.value, status_bit.value,
          longitude, status_bit.bit.ewlongitude == 0 ? 'E':'W',
          latitude, status_bit.bit.snlatitude == 0 ? 'N':'S',
          altitude, speed, bearing,
          timestamp[0], timestamp[1], timestamp[2],
          timestamp[3], timestamp[4], timestamp[5]);
  if (len >= (46+offset)) {
    fprintf(stdout, "\tgnss satellite count: %d\n", buffer[43+offset]);
  }
  if (len >= (51+offset)) {
    fprintf(stdout, "\tgnss position status: %d\n", buffer[48+offset]);
  }
}

