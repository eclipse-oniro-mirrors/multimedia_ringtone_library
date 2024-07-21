/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DUALFW_CONF_LOADER_H
#define DUALFW_CONF_LOADER_H

#include <string>
#include <memory>

#include "ringtone_log.h"
#include "datashare_helper.h"
#include "ringtone_restore_type.h"

namespace OHOS {
namespace Media {
struct DualFwConf {
    int32_t id;
    // 通知铃音
    std::string notificationSoundPath;
    // 卡1 铃音
    std::string ringtonePath;
    // 卡2 铃音
    std::string ringtone2Path;
    // 闹钟默认铃声
    std::string alarmAlertPath;
    // 卡1 短信
    std::string messagePath;
    // 卡2 短信
    std::string messageSub1;
};

class DualfwConfLoader {
public:
    DualfwConfLoader();
    int32_t Init();
    ~DualfwConfLoader() {};
    int32_t Load(DualFwConf& conf, const RestoreSceneType& type);
    void ShowConf(const DualFwConf& conf);
private:
    std::string GetConf(const std::string& key);
    std::shared_ptr<DataShare::DataShareHelper> dataShareHelper_;
};
} // namespace Media
} // namespace OHOS
#endif // DUALFW_CONF_LOADER_H