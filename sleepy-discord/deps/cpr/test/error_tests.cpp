#include <gtest/gtest.h>

#include <chrono>
#include <string>

#include "cpr/cpr.h"
#include <curl/curl.h>

#include "httpServer.hpp"
#include "httpsServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(ErrorTests, UnsupportedProtocolFailure) {
    Url url{"urk://wat.is.this"};
    Response response = cpr::Get(url);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::UNSUPPORTED_PROTOCOL, response.error.code);
}

TEST(ErrorTests, InvalidURLFailure) {
    Url url{"???"};
    Response response = cpr::Get(url);
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::URL_MALFORMAT, response.error.code);
}

TEST(ErrorTests, TimeoutFailure) {
    Url url{server->GetBaseUrl() + "/timeout.html"};
    Response response = cpr::Get(url, cpr::Timeout{1});
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::OPERATION_TIMEDOUT, response.error.code);
}

TEST(ErrorTests, ChronoTimeoutFailure) {
    Url url{server->GetBaseUrl() + "/timeout.html"};
    Response response = cpr::Get(url, cpr::Timeout{std::chrono::milliseconds{1}});
    EXPECT_EQ(0, response.status_code);
    EXPECT_EQ(ErrorCode::OPERATION_TIMEDOUT, response.error.code);
}

TEST(ErrorTests, ConnectTimeoutFailure) {
    Url url{"http://localhost:67"};
    Response response = cpr::Get(url, cpr::ConnectTimeout{1});
    EXPECT_EQ(0, response.status_code);
    // Sometimes a COULDNT_CONNECT happens before the OPERATION_TIMEDOUT:
    EXPECT_TRUE(response.error.code == ErrorCode::OPERATION_TIMEDOUT || response.error.code == ErrorCode::COULDNT_CONNECT);
}

TEST(ErrorTests, ChronoConnectTimeoutFailure) {
    Url url{"http://localhost:67"};
    Response response = cpr::Get(url, cpr::ConnectTimeout{std::chrono::milliseconds{1}});
    EXPECT_EQ(0, response.status_code);
    // Sometimes a COULDNT_CONNECT happens before the OPERATION_TIMEDOUT:
    EXPECT_TRUE(response.error.code == ErrorCode::OPERATION_TIMEDOUT || response.error.code == ErrorCode::COULDNT_CONNECT);
}

TEST(ErrorTests, LowSpeedTimeFailure) {
    Url url{server->GetBaseUrl() + "/low_speed.html"};
    Response response = cpr::Get(url, cpr::LowSpeed{1000, std::chrono::seconds(1)});
    // Do not check for the HTTP status code, since libcurl always provides the status code of the header if it was received
    EXPECT_EQ(ErrorCode::OPERATION_TIMEDOUT, response.error.code);
}

TEST(ErrorTests, LowSpeedBytesFailure) {
    Url url{server->GetBaseUrl() + "/low_speed_bytes.html"};
    Response response = cpr::Get(url, cpr::LowSpeed{1000, std::chrono::seconds(1)});
    // Do not check for the HTTP status code, since libcurl always provides the status code of the header if it was received
    EXPECT_EQ(ErrorCode::OPERATION_TIMEDOUT, response.error.code);
}

TEST(ErrorTests, ProxyFailure) {
    Url url{server->GetBaseUrl() + "/hello.html"};
    Response response = cpr::Get(url, cpr::Proxies{{"http", "http://bad_host.libcpr.org"}});
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(0, response.status_code);
    // Sometimes the DNS server returns a fake address instead of an NXDOMAIN response, leading to COULDNT_CONNECT.
    EXPECT_TRUE(response.error.code == ErrorCode::COULDNT_RESOLVE_PROXY || response.error.code == ErrorCode::COULDNT_CONNECT);
}

TEST(ErrorTests, BoolFalseTest) {
    Error error;
    EXPECT_FALSE(error);
}

TEST(ErrorTests, BoolTrueTest) {
    Error error;
    error.code = ErrorCode::UNSUPPORTED_PROTOCOL;
    EXPECT_TRUE(error);
}

TEST(ErrorTests, StringReprTest) {
    Error error;
    error.code = ErrorCode::UNSUPPORTED_PROTOCOL;
    EXPECT_EQ(std::to_string(error.code), "UNSUPPORTED_PROTOCOL");
}

TEST(ErrorTests, StringReprUnknownTest) {
    Error error;
    error.code = ErrorCode::UNKNOWN_ERROR;
    EXPECT_EQ(std::to_string(error.code), "UNKNOWN_ERROR");
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
