/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_login_cert_helpers.h"

#include "common/settings.h"
#include "login/cert_helpers.h"

#include <openssl/pem.h>
#include <openssl/x509.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include <unistd.h>

namespace
{

class TempDirectoryGuard
{
public:
    TempDirectoryGuard()
    : originalPath_(std::filesystem::current_path())
    , tempPath_(std::filesystem::temp_directory_path() / ("lsb_login_cert_helpers_" + std::to_string(::getpid())))
    {
        std::filesystem::remove_all(tempPath_);
        std::filesystem::create_directories(tempPath_);
        std::filesystem::current_path(tempPath_);
    }

    ~TempDirectoryGuard()
    {
        std::filesystem::current_path(originalPath_);
        std::filesystem::remove_all(tempPath_);
    }

private:
    std::filesystem::path originalPath_;
    std::filesystem::path tempPath_;
};

class LoginAuthIPGuard
{
public:
    LoginAuthIPGuard()
    : value_(settings::get<std::string>("network.LOGIN_AUTH_IP"))
    {
    }

    ~LoginAuthIPGuard()
    {
        settings::set("network.LOGIN_AUTH_IP", value_);
    }

private:
    std::string value_;
};

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login cert helpers self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto fileContents(const std::filesystem::path& path) -> std::string
{
    std::ifstream input(path, std::ios::binary);
    return { std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() };
}

auto certificateCommonName(const std::filesystem::path& path) -> std::string
{
    FILE* fileHandle = std::fopen(path.string().c_str(), "rb");
    if (!fileHandle)
    {
        return {};
    }

    X509* cert = PEM_read_X509(fileHandle, nullptr, nullptr, nullptr);
    std::fclose(fileHandle);
    if (!cert)
    {
        return {};
    }

    char commonName[256] = {};
    X509_NAME_get_text_by_NID(X509_get_subject_name(cert), NID_commonName, commonName, sizeof(commonName));
    X509_free(cert);

    return commonName;
}

auto testGeneratesMissingLoginCertificatePair() -> bool
{
    bool ok = true;

    TempDirectoryGuard tempDirectory;
    LoginAuthIPGuard   settingGuard;
    settings::set("network.LOGIN_AUTH_IP", std::string("192.0.2.55"));

    certificateHelpers::generateSelfSignedCert();

    ok = expectEqual(std::filesystem::exists("login.key"), true, "login.key generated") && ok;
    ok = expectEqual(std::filesystem::exists("login.cert"), true, "login.cert generated") && ok;
    ok = expectEqual(certificateCommonName("login.cert"), std::string("192.0.2.55"), "certificate common name") && ok;

    return ok;
}

auto testDoesNotRegenerateWhenOnlyKeyExists() -> bool
{
    bool ok = true;

    TempDirectoryGuard tempDirectory;
    LoginAuthIPGuard   settingGuard;
    settings::set("network.LOGIN_AUTH_IP", std::string("192.0.2.56"));
    std::ofstream("login.key") << "existing key";

    certificateHelpers::generateSelfSignedCert();

    ok = expectEqual(fileContents("login.key"), std::string("existing key"), "existing key left untouched") && ok;
    ok = expectEqual(std::filesystem::exists("login.cert"), false, "missing cert not generated when key exists") && ok;

    return ok;
}

auto testDoesNotRegenerateWhenOnlyCertExists() -> bool
{
    bool ok = true;

    TempDirectoryGuard tempDirectory;
    LoginAuthIPGuard   settingGuard;
    settings::set("network.LOGIN_AUTH_IP", std::string("192.0.2.57"));
    std::ofstream("login.cert") << "existing cert";

    certificateHelpers::generateSelfSignedCert();

    ok = expectEqual(std::filesystem::exists("login.key"), false, "missing key not generated when cert exists") && ok;
    ok = expectEqual(fileContents("login.cert"), std::string("existing cert"), "existing cert left untouched") && ok;

    return ok;
}

} // namespace

auto runLoginCertHelpersSelfTests() -> bool
{
    bool ok = true;

    ok = testGeneratesMissingLoginCertificatePair() && ok;
    ok = testDoesNotRegenerateWhenOnlyKeyExists() && ok;
    ok = testDoesNotRegenerateWhenOnlyCertExists() && ok;

    return ok;
}
