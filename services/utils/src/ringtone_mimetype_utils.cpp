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

#include "ringtone_mimetype_utils.h"

#include <fstream>

#include "ringtone_log.h"
#include "ringtone_errno.h"

namespace OHOS {
namespace Media {
using namespace std;
using namespace nlohmann;

using MimeTypeMap = unordered_map<string, vector<string>>;

MimeTypeMap RingtoneMimeTypeUtils::mediaJsonMap_;
const string MIMETYPE_JSON_PATH = "/system/etc/userfilemanager/userfilemanager_mimetypes.json";
const string DEFAULT_MIME_TYPE = "application/octet-stream";

static const std::unordered_map<std::string, std::vector<std::string>> RINGTONE_MIME_TYPE_MAP = {
    { "application/epub+zip", { "epub" } },
    { "application/lrc", { "lrc"} },
    { "application/pkix-cert", { "cer" } },
    { "application/rss+xml", { "rss" } },
    { "application/sdp", { "sdp" } },
    { "application/smil+xml", { "smil" } },
    { "application/ttml+xml", { "ttml", "dfxp" } },
    { "application/vnd.ms-pki.stl", { "stl" } },
    { "application/vnd.ms-powerpoint", { "pot", "ppt" } },
    { "application/vnd.ms-wpl", { "wpl" } },
    { "application/vnd.stardivision.writer", { "vor" } },
    { "application/vnd.youtube.yt", { "yt" } },
    { "application/x-font", { "pcf" } },
    { "application/x-mobipocket-ebook", { "prc", "mobi" } },
    { "application/x-pem-file", { "pem" } },
    { "application/x-pkcs12", { "p12", "pfx" } },
    { "application/x-subrip", { "srt" } },
    { "application/x-webarchive", { "webarchive" } },
    { "application/x-webarchive-xml", { "webarchivexml" } },
    { "application/pgp-signature", { "pgp" } },
    { "application/x-x509-ca-cert", { "crt", "der" } },
    { "application/json", { "json" } },
    { "application/javascript", { "js" } },
    { "application/zip", { "zip" } },
    { "application/rar", { "rar" } },
    { "application/pdf", { "pdf" } },
    { "application/msword", { "doc" } },
    { "application/ms-excel", { "xls" } },
    { "application/vnd.openxmlformats-officedocument.wordprocessingml.document", { "docx" } },
    { "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", { "xlsx" } },
    { "application/vnd.openxmlformats-officedocument.presentationml.presentation", { "pptx" } },
    { "audio/3gpp", { "3ga" } },
    { "audio/ac3", { "ac3", "a52"} },
    { "audio/amr", { "amr" } },
    { "audio/imelody", { "imy" } },
    { "audio/midi", { "rtttl", "xmf", "rtx" } },
    { "audio/mobile-xmf", { "mxmf"} },
    { "audio/mp4", { "m4a", "m4b", "m4p", "f4a", "f4b", "f4p" } },
    { "audio/mpegurl", { "m3u" } },
    { "audio/sp-midi", { "smf" } },
    { "audio/x-matroska", { "mka" } },
    { "audio/x-pn-realaudio", { "ra" } },
    { "audio/x-mpeg", { "mp3" } },
    { "audio/aac", { "aac", "adts", "adt" } },
    { "audio/basic", { "snd" } },
    { "audio/flac", { "flac" } },
    { "audio/mpeg", { "mp3", "mp2", "mp1", "mpa", "m4r" } },
    { "audio/wav", { "wav" } },
    { "audio/ogg", { "ogg" } },
    { "image/gif", { "gif"} },
    { "image/heic", { "heic" } },
    { "image/heic-sequence", { "heics", "heifs" } },
    { "image/bmp", { "bmp", "bm" } },
    { "image/heif", { "heif", "hif" } },
    { "image/avif", { "avif" } },
    { "image/ico", { "cur" } },
    { "image/webp", { "webp"} },
    { "image/x-adobe-dng", { "dng" } },
    { "image/x-fuji-raf", { "raf" } },
    { "image/x-icon", { "ico" } },
    { "image/x-nikon-nrw", { "nrw" } },
    { "image/x-panasonic-rw2", { "rw2" } },
    { "image/x-pentax-pef", { "pef" } },
    { "image/x-samsung-srw", { "srw" } },
    { "image/x-sony-arw", { "arw" } },
    { "image/jpeg", { "jpg", "jpeg", "jpe" } },
    { "image/png", { "png" } },
    { "image/svg+xml", { "svg" } },
    { "image/x-dcraw", { "raw" } },
    { "video/3gpp2", { "3gpp2", "3gp2", "3g2" } },
    { "video/3gpp", { "3gpp", "3gp" } },
    { "video/mp4", { "m4v", "f4v", "mp4v", "mpeg4", "mp4" }},
    { "video/mp2t", { "m2ts", "mts"} },
    { "video/mp2ts", { "ts" } },
    { "video/vnd.youtube.yt", { "yt" } },
    { "video/x-webex", { "wrf" } },
    { "video/mpeg", { "mpeg", "mpeg2", "mpv2", "mp2v", "m2v", "m2t", "mpeg1", "mpv1", "mp1v", "m1v", "mpg" } },
    { "video/quicktime", { "mov" } },
    { "video/x-matroska", { "mkv" } },
    { "video/webm", { "webm" } },
    { "video/H264", { "h264" } },
    { "video/x-flv", { "flv" } },
    { "video/avi", { "avi" } },
    { "text/comma-separated-values", { "csv" } },
    { "text/plain", { "diff", "po", "txt" } },
    { "text/rtf", { "rtf" } },
    { "text/text", { "phps", "m3u", "m3u8" } },
    { "text/xml", { "xml" } },
    { "text/x-vcard", { "vcf" } },
    { "text/x-c++hdr", { "hpp", "h++", "hxx", "hh" } },
    { "text/x-c++src", { "cpp", "c++", "cxx", "cc" } },
    { "text/css", { "css" } },
    { "text/html", { "html", "htm", "shtml"} },
    { "text/markdown", { "md", "markdown" } },
    { "text/x-java", { "java" } },
    { "text/x-python", { "py" } }
};

/**
 * The format of the target json file:
 * First floor: Media type string, such as image, video, audio, etc.
 * Second floor: Mime type string
 * Third floor: Extension array.
*/
void RingtoneMimeTypeUtils::CreateMapFromJson()
{
    std::ifstream jFile(MIMETYPE_JSON_PATH);
    if (!jFile.is_open()) {
        RINGTONE_ERR_LOG("Failed to open: %{private}s", MIMETYPE_JSON_PATH.c_str());
        mediaJsonMap_ = RINGTONE_MIME_TYPE_MAP;
        RINGTONE_INFO_LOG("Add ringtone mime type map success");
        return;
    }
    json firstFloorObjs;
    jFile >> firstFloorObjs;
    for (auto& firstFloorObj : firstFloorObjs.items()) {
        json secondFloorJsons = json::parse(firstFloorObj.value().dump(), nullptr, false);
        if (secondFloorJsons.is_discarded()) {
            continue;
        }
        for (auto &secondFloorJson : secondFloorJsons.items()) {
            json thirdFloorJsons = json::parse(secondFloorJson.value().dump(), nullptr, false);
            // Key: MimeType, Value: Extension array.
            if (!thirdFloorJsons.is_discarded()) {
                mediaJsonMap_.insert(std::pair<string, vector<string>>(secondFloorJson.key(), thirdFloorJsons));
            }
        }
    }
}

int32_t RingtoneMimeTypeUtils::InitMimeTypeMap()
{
    CreateMapFromJson();
    if (mediaJsonMap_.empty()) {
        RINGTONE_ERR_LOG("JsonMap is empty");
        return E_FAIL;
    }
    return E_OK;
}

string RingtoneMimeTypeUtils::GetMimeTypeFromExtension(const string &extension)
{
    return GetMimeTypeFromExtension(extension, mediaJsonMap_);
}

string RingtoneMimeTypeUtils::GetMimeTypeFromExtension(const string &extension,
    const MimeTypeMap &mimeTypeMap)
{
    string tmp = extension;
    transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
    for (auto &item : mimeTypeMap) {
        for (auto &ext : item.second) {
            if (ext == tmp) {
                return item.first;
            }
        }
    }
    return DEFAULT_MIME_TYPE;
}

RingtoneMediaType RingtoneMimeTypeUtils::GetMediaTypeFromMimeType(const string &mimeType)
{
    size_t pos = mimeType.find_first_of("/");
    if (pos == string::npos) {
        RINGTONE_ERR_LOG("Invalid mime type: %{public}s", mimeType.c_str());
        return RINGTONE_MEDIA_TYPE_INVALID;
    }
    string prefix = mimeType.substr(0, pos);
    if (prefix == "audio") {
        return RINGTONE_MEDIA_TYPE_AUDIO;
    } else {
        RINGTONE_ERR_LOG("Invalid mime type: %{public}s", mimeType.c_str());
        return RINGTONE_MEDIA_TYPE_INVALID;
    }
}
} // namespace Media
} // namespace OHOS
