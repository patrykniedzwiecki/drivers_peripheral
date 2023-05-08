/*
 * Copyright 2023 Shenzhen Kaihong DID Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef PARSE_COMMAND_H
#define PARSE_COMMAND_H
#include <cinttypes>
#include <string>
enum class codecMime { AVC, HEVC };

using CommandOpt = struct CommandOpt {
    std::string fileInput = "";
    std::string fileOutput = "";
    uint32_t width = 0;
    uint32_t height = 0;
    bool useBuffer = false;
    codecMime codec = codecMime::AVC;
};

class CommandParse {
public:
    CommandParse()
    {}
    ~CommandParse()
    {}
    bool Parse(int argc, char *argv[], CommandOpt &opt);

private:
    void ShowUsage();
};
#endif