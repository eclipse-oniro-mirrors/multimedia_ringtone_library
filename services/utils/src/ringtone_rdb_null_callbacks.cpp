/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "ringtone_rdb_callbacks.h"

#include <chrono>
#include <set>
#include <sys/stat.h>
#include <thread>

#include "rdb_sql_utils.h"
#include "ringtone_errno.h"
#include "ringtone_log.h"
#include "ringtone_db_const.h"
#include "ringtone_file_utils.h"
#include "ringtone_mimetype_utils.h"
#include "ringtone_type.h"
#include "ringtone_utils.h"
#include "result_set_utils.h"
#include "preferences_helper.h"

namespace OHOS {
namespace Media {
using namespace std;

RingtoneDataNullCallBack::RingtoneDataNullCallBack(void)
{
}

RingtoneDataNullCallBack::~RingtoneDataNullCallBack(void)
{
}

int32_t RingtoneDataNullCallBack::OnCreate(NativeRdb::RdbStore &store)
{
    return NativeRdb::E_OK;
}

int32_t RingtoneDataNullCallBack::OnUpgrade(NativeRdb::RdbStore &store, int32_t oldVersion, int32_t newVersion)
{
    RINGTONE_INFO_LOG("RingtoneDataNullCallBack OnUpgrade old:%d, new:%d", oldVersion, newVersion);
    return NativeRdb::E_OK;
}
} // namespace Media
} // namespace OHOS
