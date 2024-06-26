/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "get_self_permissions.h"

#include <thread>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "ringtone_errno.h"
#include "ringtone_log.h"
#include "token_setproc.h"

namespace OHOS {
namespace Media {
using namespace Security::AccessToken;

void RingtonePermissionUtilsUnitTest::SetAccessTokenPermission(const std::string &processName,
    const std::vector<std::string> &permission, uint64_t &tokenId)
{
    auto perms = std::make_unique<const char *[]>(permission.size());
    for (size_t i = 0; i < permission.size(); i++) {
        perms[i] = permission[i].c_str();
    }

    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = permission.size(),
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms.get(),
        .acls = nullptr,
        .processName = processName.c_str(),
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    if (tokenId == 0) {
        RINGTONE_ERR_LOG("Get Acess Token Id Failed");
        return;
    }
    int ret = SetSelfTokenID(tokenId);
    if (ret != 0) {
        RINGTONE_ERR_LOG("Set Acess Token Id Failed");
        return;
    }
    ret = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    if (ret < 0) {
        RINGTONE_ERR_LOG("Reload Native Token Info Failed");
        return;
    }
}

int32_t RingtonePermissionUtilsUnitTest::SetHapPermission(const std::string &bundleName,
    const int32_t userId, bool isSystem)
{
    HapInfoParams info = {
        .userID = userId,
        .bundleName = bundleName,
        .instIndex = 0,
        .appIDDesc = bundleName,
        .isSystemApp = isSystem
    };

    HapPolicyParams policy = {
        .apl = APL_SYSTEM_BASIC,
        .domain = "test.domain.ringtonelibrary",
        .permList = { },
        .permStateList = {
            {
                .permissionName = "ohos.permission.WRITE_RINGTONE",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 }
            }
        }
    };
    AccessTokenIDEx tokenIdEx = { 0 };
    tokenIdEx = AccessTokenKit::AllocHapToken(info, policy);
    int ret = SetSelfTokenID(tokenIdEx.tokenIDEx);
    if (ret != 0) {
        RINGTONE_ERR_LOG("Set hap token failed, err: %{public}d", ret);
        return E_PERMISSION_DENIED;
    }
    return E_SUCCESS;
}
} // namespace Media
} // namespace OHOS

