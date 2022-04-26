//
// Created by gilee on 2022/04/20.
//

#ifndef WEBSERV_HTTPHEADERFIELD_HPP
#define WEBSERV_HTTPHEADERFIELD_HPP

#define CACHE_CONTROL_STR       "Cache-Control"
#define CONNECTION_STR          "Connection"
#define DATE_STR                "Date"
#define PRAGMA_STR              "Pragma"
#define TRAILER_STR             "Trailer"
#define TRANSFER_ENCODING_STR   "Transfer-Encoding"
#define UPGRADE_STR             "Upgrade"
#define VIA_STR                 "Via"
#define ACCEPT_STR              "Accept"
#define ACCEPT_CHARSET_STR      "Accept-Charset"
#define ACCEPT_ENCODING_STR     "Accept-Encoding"
#define ACCEPT_LANGUAGE_STR     "Accept-Language"
#define AUTHORIZATION_STR       "Authorization"
#define EXPECT_STR              "Expect"
#define FROM_STR                "From"
#define HOST_STR                "Host"
#define IF_MATCH_STR            "If-Match"
#define IF_MODIFIED_SINCE_STR   "If-Modified-Since"
#define IF_NONE_MATCH_STR       "If-None-Match"
#define IF_RANGE_STR            "If-Range"
#define IF_UNMODIFIED_SINCE_STR "If-Unmodified-Since"
#define MAX_FORWARDS_STR        "Max-Forwards"
#define PROXY_AUTHORIZATION_STR "Proxy-Authorization"
#define RANGE_STR               "Range"
#define REFERER_STR             "Referer"
#define TE_STR                  "TE"
#define USER_AGENT_STR          "User-Agent"
#define ACCEPT_RANGES_STR       "Accept-Ranges"
#define AGE_STR                 "Age"
#define ETAG_STR                "ETag"
#define LOCATION_STR            "Location"
#define PROXY_AUTHENTICATE_STR  "Proxy-Authenticate"
#define RETRY_AFTER_STR         "Retry-After"
#define SERVER_STR              "Server"
#define VARY_STR                "Vary"
#define WWW_AUTHENTICATE_STR    "WWW-Authenticate"
#define ALLOW_STR               "Allow"
#define CONTENT_ENCODING_STR    "Content-Encoding"
#define CONTENT_LANGUAGE_STR    "Content-Language"
#define CONTENT_LENGTH_STR      "Content-Length"
#define CONTENT_LOCATION_STR    "Content-Location"
#define CONTENT_MD5_STR         "Content-MD5"
#define CONTENT_RANGE_STR       "Content-Range"
#define CONTENT_TYPE_STR        "Content-Type"
#define EXPIRES_STR             "Expires"
#define LAST_MODIFIED_STR       "Last-Modified"
#define EXTENSION_HEADER_STR    "extension-header"

enum header {
    CACHE_CONTROL,
    CONNECTION,
    DATE,
    PRAGMA,
    TRAILER,
    TRANSFER_ENCODING,
    UPGRADE,
    VIA,
    ACCEPT,
    ACCEPT_CHARSET,
    ACCEPT_ENCODING,
    ACCEPT_LANGUAGE,
    AUTHORIZATION,
    EXPECT,
    FROM,
    HOST,
    IF_MATCH,
    IF_MODIFIED_SINCE,
    IF_NONE_MATCH,
    IF_RANGE,
    IF_UNMODIFIED_SINCE,
    MAX_FORWARDS,
    PROXY_AUTHORIZATION,
    RANGE,
    REFERER,
    TE,
    USER_AGENT,
    ACCEPT_RANGES,
    AGE,
    ETAG,
    LOCATION,
    PROXY_AUTHENTICATE,
    RETRY_AFTER,
    SERVER,
    VARY,
    WWW_AUTHENTICATE,
    ALLOW,
    CONTENT_ENCODING,
    CONTENT_LANGUAGE,
    CONTENT_LENGTH,
    CONTENT_LOCATION,
    CONTENT_MD5,
    CONTENT_RANGE,
    CONTENT_TYPE,
    EXPIRES,
    LAST_MODIFIED,
    EXTENSION_HEADER,
    ENUM_COUNT
};
#endif //WEBSERV_HTTPHEADERFIELD_HPP
