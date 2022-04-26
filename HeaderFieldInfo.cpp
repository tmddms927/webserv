//
// Created by gilee on 2022/04/20.
//

#include "HeaderFieldInfo.hpp"
#include "HTTPHeaderField.hpp"

HeaderFieldInfo::HeaderFieldInfo() {
    key.push_back(CACHE_CONTROL_STR);
    key.push_back(CONNECTION_STR);
    key.push_back(DATE_STR);
    key.push_back(PRAGMA_STR);
    key.push_back(TRAILER_STR);
    key.push_back(TRANSFER_ENCODING_STR);
    key.push_back(UPGRADE_STR);
    key.push_back(VIA_STR);
    key.push_back(ACCEPT_STR);
    key.push_back(ACCEPT_CHARSET_STR);
    key.push_back(ACCEPT_ENCODING_STR);
    key.push_back(ACCEPT_LANGUAGE_STR);
    key.push_back(AUTHORIZATION_STR);
    key.push_back(EXPECT_STR);
    key.push_back(FROM_STR);
    key.push_back(HOST_STR);
    key.push_back(IF_MATCH_STR);
    key.push_back(IF_MODIFIED_SINCE_STR);
    key.push_back(IF_NONE_MATCH_STR);
    key.push_back(IF_RANGE_STR);
    key.push_back(IF_UNMODIFIED_SINCE_STR);
    key.push_back(MAX_FORWARDS_STR);
    key.push_back(PROXY_AUTHORIZATION_STR);
    key.push_back(RANGE_STR);
    key.push_back(REFERER_STR);
    key.push_back(TE_STR);
    key.push_back(USER_AGENT_STR);
    key.push_back(ACCEPT_RANGES_STR);
    key.push_back(AGE_STR);
    key.push_back(ETAG_STR);
    key.push_back(LOCATION_STR);
    key.push_back(PROXY_AUTHENTICATE_STR);
    key.push_back(RETRY_AFTER_STR);
    key.push_back(SERVER_STR);
    key.push_back(VARY_STR);
    key.push_back(WWW_AUTHENTICATE_STR);
    key.push_back(ALLOW_STR);
    key.push_back(CONTENT_ENCODING_STR);
    key.push_back(CONTENT_LANGUAGE_STR);
    key.push_back(CONTENT_LENGTH_STR);
    key.push_back(CONTENT_LOCATION_STR);
    key.push_back(CONTENT_MD5_STR);
    key.push_back(CONTENT_RANGE_STR);
    key.push_back(CONTENT_TYPE_STR);
    key.push_back(EXPIRES_STR);
    key.push_back(LAST_MODIFIED_STR);
    key.push_back(EXTENSION_HEADER_STR);
}

const std::string &HeaderFieldInfo::operator[](unsigned int idx) const{
    return key[idx];
}
