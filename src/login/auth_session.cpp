/*
===========================================================================

  Copyright (c) 2023 LandSandBoat Dev Teams

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

#include "auth_session.h"

#include "auth_handshake.h"
#include "auth_password.h"
#include "common/ipc.h"
#include "common/utils.h"
#include "otp_helpers.h"

#include <bcrypt/BCrypt.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void auth_session::start()
{
    if (loginHelpers::ClassifyAuthHandshakeStart(socket_.lowest_layer().is_open()) == loginHelpers::auth_handshake_start_action::START_HANDSHAKE)
    {
        socket_.async_handshake(
            asio::ssl::stream_base::server,
            [this, self = shared_from_this()](std::error_code ec)
            {
                const auto errStr = ec ? fmt::format("Error from {}: (EC: {}), {}", ipAddress, ec.value(), ec.message()) : std::string{};
                switch (loginHelpers::ClassifyAuthHandshakeCompletion(static_cast<bool>(ec), errStr))
                {
                    case loginHelpers::auth_handshake_completion_action::START_READ:
                        do_read();
                        return;
                    case loginHelpers::auth_handshake_completion_action::CLOSE_WITH_LEGACY_XILOADER_HINT:
                        ShowWarning(errStr);
                        ShowWarning("Failed to handshake!");
                        ShowWarning("This is likely due to the client using an outdated/incompatible version of xiloader.");
                        ShowWarning("Please make sure you're using the latest release: https://github.com/LandSandBoat/xiloader/releases");
                        socket_.next_layer().close();
                        return;
                    case loginHelpers::auth_handshake_completion_action::CLOSE:
                        ShowWarning(errStr);
                        ShowWarning("Failed to handshake!");
                        socket_.next_layer().close();
                        return;
                }
            });
    }
}

void auth_session::do_read()
{
    std::memset(buffer_.data(), 0, buffer_.size());

    socket_.async_read_some(
        asio::buffer(buffer_.data(), buffer_.size()),
        [this, self = shared_from_this()](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                read_func();
            }
            else
            {
                DebugSockets(fmt::format("async_read_some error in auth_session from IP {} ({}: {})", ipAddress, ec.value(), ec.message()));
                handle_error(ec, self);
            }
        });
}

void auth_session::read_func()
{
    const auto jsonBuffer = nlohmann::json::parse(buffer_, nullptr, false);

    const auto sendJsonAsBuffer = [&](const json& json_)
    {
        std::string jsonString       = json_.dump();
        const char* jsonStringBuffer = jsonString.c_str();
        size_t      jsonStringSize   = strlen(jsonStringBuffer);

        std::memset(buffer_.data(), 0, buffer_.size());
        std::memcpy(buffer_.data(), jsonStringBuffer, jsonStringSize);

        do_write(jsonStringSize);
    };

    const auto sendLoginResult = [&](const login_result errorCode)
    {
        json loginErrorCodeReply;
        loginErrorCodeReply["result"] = errorCode; // "old style" backwards compatible error code

        sendJsonAsBuffer(loginErrorCodeReply);
    };

    const auto sendJsonOnlyErrorMessage = [&](const std::string& errorMessage)
    {
        json loginErrorCodeReply;
        loginErrorCodeReply["error_message"] = errorMessage;

        sendJsonAsBuffer(loginErrorCodeReply);
    };

    if (jsonBuffer.is_discarded()) // not json
    {
        ref<uint8>(buffer_.data(), 0) = static_cast<uint8>(malformedAuthPacketResult(ref<uint8>(buffer_.data(), 0)));
        do_write(1);

        // close socket
        socket_.lowest_layer().shutdown(asio::socket_base::shutdown_both);
        socket_.lowest_layer().close();
        return;
    }

    int8                   code                = loginHelpers::jsonGet<int8>(jsonBuffer, "command").value_or(0);
    std::string            username            = loginHelpers::jsonGet<std::string>(jsonBuffer, "username").value_or("");
    std::string            password            = loginHelpers::jsonGet<std::string>(jsonBuffer, "password").value_or("");
    std::string            updated_password    = loginHelpers::jsonGet<std::string>(jsonBuffer, "new_password").value_or("");
    std::string            otp                 = loginHelpers::jsonGet<std::string>(jsonBuffer, "otp").value_or("");
    std::string            trust_token         = loginHelpers::jsonGet<std::string>(jsonBuffer, "trust_token").value_or("");
    bool                   trust_this_computer = loginHelpers::jsonGet<bool>(jsonBuffer, "trust_this_computer").value_or(false);
    std::array<uint8_t, 3> version             = loginHelpers::jsonGet<uint8, 3>(jsonBuffer, "version").value_or(std::array<uint8_t, 3>{ 0, 0, 0 });

    const auto inputValidation = validateAuthInput(version, username, password);

    // Check major.minor but ignore trivial
    if (inputValidation == auth_input_validation::VERSION_UNSUPPORTED)
    {
        sendJsonOnlyErrorMessage(loginHelpers::FormatUnsupportedXiloaderVersionError(version));
        return;
    }

    DebugSockets(fmt::format("auth code: {} from {}", code, ipAddress));

    // data checks
    if (inputValidation == auth_input_validation::MALFORMED_USERNAME)
    {
        ShowWarningFmt("login_parse: malformed username from {}", ipAddress);
        return;
    }

    if (inputValidation == auth_input_validation::MALFORMED_PASSWORD)
    {
        ShowWarningFmt("login_parse: malformed password from {}", ipAddress);
        return;
    }

    switch (static_cast<login_cmd>(code))
    {
        case login_cmd::LOGIN_NOOP:
        {
            // no-op. This can happen if control + C is pressed in xiloader.
            break;
        }
        case login_cmd::LOGIN_ATTEMPT:
        {
            DebugSockets(fmt::format("LOGIN_ATTEMPT from {}", ipAddress));

            // Look up and validate account password
            auto accountInfo = validatePassword(username, password);
            if (!accountInfo)
            {
                sendLoginResult(login_result::LOGIN_ERROR);
                return;
            }

            auto [accountID, status] = *accountInfo;

            // Reject banned/non-normal accounts before processing OTP or trust tokens
            const auto accountGate = loginHelpers::classifyLoginAttemptAccountStatus(status);
            if (accountGate != loginHelpers::login_attempt_account_gate::ALLOW)
            {
                // Purge any lingering trust tokens for banned accounts
                if (accountGate == loginHelpers::login_attempt_account_gate::REJECT_BANNED)
                {
                    otpHelpers::removeAllTrustTokens(accountID);
                }
                sendLoginResult(login_result::LOGIN_FAIL);
                return;
            }

            const bool needsOTP = otpHelpers::doesAccountNeedOTP(accountID, "TOTP");
            bool       trustTokenValid = false;
            if (needsOTP && !trust_token.empty())
            {
                trustTokenValid = otpHelpers::validateTrustToken(accountID, trust_token);
            }
            // totpValid is only evaluated when needsOTP and not trusted by token
            // and not already on the trust-token-invalid-without-OTP path — same
            // call points as the pre-extract control flow.
            bool totpValid = false;
            if (needsOTP)
            {
                const bool trustedByToken = !trust_token.empty() && trustTokenValid;
                if (!trustedByToken && !(otp.empty() && !trust_token.empty()))
                {
                    totpValid = otpHelpers::validateTOTP(otp, otpHelpers::getAccountSecret(username, "TOTP"));
                }
            }

            const auto otpPlan = loginHelpers::PlanLoginAttemptOTP(needsOTP, trust_token, otp, trustTokenValid, totpValid);
            if (const auto otpResult = loginHelpers::LoginResultForLoginAttemptOTP(otpPlan.outcome))
            {
                sendLoginResult(*otpResult);
                return;
            }
            const bool otpVerified = otpPlan.otpVerified;

            db::preparedStmt("UPDATE accounts SET accounts.timelastmodify = NULL WHERE accounts.id = ?", accountID);

            const auto payload = ipc::toBytesWithHeader(ipc::AccountLogin{
                .accountId = accountID,
            });

            dealerChannel_.send(zmq::message_t(payload.data(), payload.size()));

            // set Satchel to the same size as inventory on all chars on their account if character has OTP
            // Note: Upgrades happen in-game with gobbiebag
            if (otpVerified)
            {
                db::preparedStmt("UPDATE char_storage a JOIN char_storage b ON a.charid = b.charid "
                                 "SET a.satchel = b.inventory "
                                 "WHERE a.charid IN (SELECT charid FROM chars WHERE accid = ?)",
                                 accountID);
            }
            // TODO: Lock out same account logging in multiple times. Can check data/view session existence on same IP/account?
            // Not a real problem because the account is locked out when a character is logged in.

            /*
            const auto rset = db::preparedStmt("SELECT charid "
                    "FROM accounts_sessions "
                    "WHERE accid = ? LIMIT 1", accountID);
            if (rset && rset->rowsCount() != 0 && rset->next())
            {
                // TODO: kick player out of map server if already logged in
                // uint32 charid = rset->get<uint32>("charid");

                // This error message doesn't work when sent this way. Unknown how to transmit "1039" error message to a client already logged in.
                // session_t& authenticatedSession = get_authenticated_session(socket_, session.sentAccountID);
                // if (auto data = authenticatedSession.buffer_.data()session)
                // {
                //  generateErrorMessage(data->buffer_.data(), 139);
                //  data->do_write(0x24);
                //  return;
                //}
                ref<uint8>(buffer_.data(), 0) = LOGIN_ERROR_ALREADY_LOGGED_IN;
                do_write(1);
                return;
            }
            */

            // Success
            unsigned char hash[loginHelpers::SessionHashLength];
            uint32        hashData = loginHelpers::SessionHashSeed(earth_time::timestamp(), static_cast<uint32>(getpid()));
            md5(reinterpret_cast<uint8*>(&hashData), hash, sizeof(hashData));

            json loginSuccessReply;
            loginSuccessReply["result"]       = static_cast<uint8>(loginHelpers::LoginAttemptSuccessResult);
            loginSuccessReply["account_id"]   = accountID;
            loginSuccessReply["session_hash"] = hash; // This has to be sent as an array, json.dump() tries to convert to UTF which fails

            if (loginHelpers::ShouldIssueTrustToken(trust_this_computer, otpVerified))
            {
                try
                {
                    auto newToken = otpHelpers::generateTrustToken();
                    otpHelpers::saveTrustToken(accountID, newToken);
                    loginSuccessReply["trust_token"] = newToken;
                }
                catch (const std::runtime_error& e)
                {
                    ShowError(fmt::format("Failed to generate trust token: {}", e.what()));
                }
            }

            sendJsonAsBuffer(loginSuccessReply);

            auto& session          = loginHelpers::get_authenticated_session(ipAddress, asStringFromUntrustedSource(hash, sizeof(hash)));
            session.accountID      = accountID;
            session.authorizedTime = timer::now();
        }
        break;
        case login_cmd::LOGIN_CREATE:
        {
            DebugSockets(fmt::format("LOGIN_CREATE from {}", ipAddress));

            // check if account creation is disabled
            if (const auto settingsResult = loginHelpers::LoginResultForAccountCreateSettings(
                    loginHelpers::ClassifyAccountCreationSettings(settings::get<bool>("login.ACCOUNT_CREATION"))))
            {
                ShowWarningFmt("login_parse: New account attempt <{}> but is disabled in settings.",
                               username);
                sendLoginResult(*settingsResult);
                return;
            }

            // looking for same login
            const auto rset = db::preparedStmt("SELECT accounts.id FROM accounts WHERE accounts.login = ?", username);
            if (const auto lookupResult = loginHelpers::LoginResultForAccountCreateLookup(
                    loginHelpers::ClassifyAccountCreateLookup(static_cast<bool>(rset), rset ? rset->rowsCount() : 0)))
            {
                sendLoginResult(*lookupResult);
                return;
            }

            // creating new account_id
            uint32 accid = 0;

            const auto rset1 = db::preparedStmt("SELECT COALESCE(MAX(accounts.id), 0) AS max_id FROM accounts");
            if (rset1 && rset1->rowsCount() != 0 && rset1->next())
            {
                accid = loginHelpers::NextAccountID(rset1->get<uint32>("max_id"));
            }
            else
            {
                sendLoginResult(login_result::LOGIN_ERROR_CREATE);
                return;
            }

            // creating new account
            std::tm timecreateinfo = earth_time::to_local_tm();

            char strtimecreate[128];
            strftime(strtimecreate, sizeof(strtimecreate), "%Y:%m:%d %H:%M:%S", &timecreateinfo);

            const auto rset2 = db::preparedStmt(
                "INSERT INTO accounts(id,login,password,timecreate,timelastmodify,status,priv) "
                "VALUES(?, ?, ?, ?, NULL, ?, ?)",
                accid,
                username,
                BCrypt::generateHash(password),
                strtimecreate,
                static_cast<uint8>(ACCOUNT_STATUS_CODE::NORMAL),
                static_cast<uint8>(ACCOUNT_PRIVILEGE_CODE::USER));

            if (!rset2)
            {
                sendLoginResult(login_result::LOGIN_ERROR_CREATE);
                return;
            }

            sendLoginResult(login_result::LOGIN_SUCCESS_CREATE);
            break;
        }
        case login_cmd::LOGIN_CHANGE_PASSWORD:
        {
            // Look up and validate account password
            auto accountInfo = validatePassword(username, password);
            if (!accountInfo)
            {
                sendLoginResult(login_result::LOGIN_ERROR_CHANGE_PASSWORD);
                return;
            }

            auto [accid, status] = *accountInfo;

            if (otpHelpers::doesAccountNeedOTP(username, "TOTP"))
            {
                if (!otpHelpers::validateTOTP(otp, otpHelpers::getAccountSecret(username, "TOTP")))
                {
                    sendLoginResult(login_result::LOGIN_ERROR_CHANGE_PASSWORD);
                    return;
                }
            }

            const auto changePlan = loginHelpers::PlanChangePasswordStatus(status);

            // Banned emit does not return — LSB falls through to the NORMAL check.
            if (changePlan.emitBannedError)
            {
                ShowInfoFmt("login_parse: banned user <{}> detected. Aborting.", username);

                sendLoginResult(login_result::LOGIN_ERROR_CHANGE_PASSWORD);
            }

            if (changePlan.attemptUpdate)
            {
                // Account info verified, update password
                if (loginHelpers::IsEmptyUpdatedPassword(updated_password))
                {
                    ShowWarningFmt("login_parse: Empty password: Could not update password for user <{}>.", username);
                    sendLoginResult(login_result::LOGIN_ERROR_CHANGE_PASSWORD);
                    return;
                }

                db::preparedStmt("UPDATE accounts SET accounts.timelastmodify = NULL WHERE accounts.id = ?", accid);

                const auto rset2 = db::preparedStmt(
                    "UPDATE accounts SET accounts.password = ? WHERE accounts.id = ?",
                    BCrypt::generateHash(updated_password),
                    accid);

                if (!rset2)
                {
                    ShowWarningFmt("login_parse: Error trying to update password in database for user <{}>.", username);
                    sendLoginResult(login_result::LOGIN_ERROR_CHANGE_PASSWORD);
                    return;
                }

                otpHelpers::removeAllTrustTokens(accid);

                json loginErrorChangePasswordReply;
                loginErrorChangePasswordReply["result"]       = loginHelpers::ChangePasswordSuccessResult;
                loginErrorChangePasswordReply["account_id"]   = loginHelpers::ChangePasswordSuccessAccountID;
                loginErrorChangePasswordReply["session_hash"] = loginHelpers::ChangePasswordSuccessSessionHash;

                sendJsonAsBuffer(loginErrorChangePasswordReply);

                ShowInfoFmt("login_parse: password updated for account {} successfully.", accid);
                return;
            }
            break;
        }
        case login_cmd::LOGIN_CREATE_TOTP:
        {
            // Look up and validate account password
            if (!validatePassword(username, password))
            {
                sendJsonOnlyErrorMessage(loginHelpers::FailedCredentialValidationMessage);
                return;
            }

            const auto serverName = settings::get<std::string>("main.SERVER_NAME");
            const auto secret     = otpHelpers::createAccountSecret(username, "TOTP");

            if (loginHelpers::IsCreateTOTPSecretValid(secret))
            {
                const std::string uri = loginHelpers::FormatTOTPURI(serverName, username, secret);

                json sendTOTP;
                sendTOTP["result"]   = login_result::LOGIN_SUCCESS_CREATE_TOTP;
                sendTOTP["TOTP_uri"] = uri;

                sendJsonAsBuffer(sendTOTP);
            }
            else
            {
                sendJsonOnlyErrorMessage(loginHelpers::FailedCredentialValidationMessage);
            }
            break;
        }
        case login_cmd::LOGIN_REMOVE_TOTP:
        {
            // Look up and validate account password
            if (!validatePassword(username, password))
            {
                sendJsonOnlyErrorMessage(loginHelpers::FailedCredentialValidationMessage);
                return;
            }

            const auto secret       = otpHelpers::getAccountSecret(username, "TOTP");
            const auto recoveryCode = otpHelpers::getAccountRecoveryCode(username, "TOTP");

            // TOTP or case-insensitive recovery code (pure dual gate).
            if (loginHelpers::AcceptsOTPOrRecovery(otpHelpers::validateTOTP(otp, secret), otp, recoveryCode))
            {
                // validated
                uint32     accid = loginHelpers::getAccountId(username);
                const auto rset  = db::preparedStmt("DELETE FROM accounts_totp WHERE accounts_totp.accid = ? LIMIT 1", accid);

                otpHelpers::removeAllTrustTokens(accid);

                json sendSuccess;
                sendSuccess["result"] = login_result::LOGIN_SUCCESS_REMOVE_TOTP;

                sendJsonAsBuffer(sendSuccess);
            }
            else
            {
                sendJsonOnlyErrorMessage(loginHelpers::FailedCredentialValidationMessage);
            }
            break;
        }
        case login_cmd::LOGIN_REGENERATE_RECOVERY:
        {
            // Look up and validate account password
            if (!validatePassword(username, password))
            {
                sendJsonOnlyErrorMessage(loginHelpers::FailedCredentialValidationMessage);
                return;
            }

            const auto secret       = otpHelpers::getAccountSecret(username, "TOTP");
            const auto recoveryCode = otpHelpers::getAccountRecoveryCode(username, "TOTP");

            // TOTP or case-insensitive recovery code (pure dual gate).
            if (loginHelpers::AcceptsOTPOrRecovery(otpHelpers::validateTOTP(otp, secret), otp, recoveryCode))
            {
                const auto newRecoveryCode = otpHelpers::regenerateAccountRecoveryCode(username, "TOTP");

                json sendTOTP;

                sendTOTP["result"]        = login_result::LOGIN_SUCCESS_VERIFY_TOTP;
                sendTOTP["recovery_code"] = newRecoveryCode;
                sendJsonAsBuffer(sendTOTP);
            }
            else
            {
                sendJsonOnlyErrorMessage(loginHelpers::FailedCredentialValidationMessage);
            }
            break;
        }
        case login_cmd::LOGIN_VERIFY_TOTP:
        {
            // Look up and validate account password
            if (!validatePassword(username, password))
            {
                sendJsonOnlyErrorMessage(loginHelpers::FailedCredentialValidationMessage);
                return;
            }

            const auto secret = otpHelpers::getAccountSecret(username, "TOTP");

            if (otpHelpers::validateTOTP(otp, secret))
            {
                // validated
                const auto rset = db::preparedStmt("UPDATE accounts_totp SET validated = TRUE WHERE accid = ? LIMIT 1", loginHelpers::getAccountId(username));

                json sendTOTP;
                sendTOTP["result"]        = login_result::LOGIN_SUCCESS_VERIFY_TOTP;
                sendTOTP["recovery_code"] = otpHelpers::getAccountRecoveryCode(username, "TOTP");
                sendJsonAsBuffer(sendTOTP);
            }
            else
            {
                sendJsonOnlyErrorMessage(loginHelpers::FailedCredentialValidationMessage);
            }

            break;
        }
        default:
        {
            ShowError(loginHelpers::FormatUnhandledAuthCode(code, ipAddress));
        }
        break;
    }
}

void auth_session::do_write(std::size_t length)
{
    asio::async_write(
        socket_,
        asio::buffer(buffer_.data(), length),
        [this, self = shared_from_this()](std::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                write_func();
            }
            else
            {
                ShowError(ec.message());
            }
        });
}

Maybe<std::pair<uint32, uint32>> auth_session::validatePassword(std::string username, std::string password)
{
    uint32 accountID = 0;
    uint32 status    = 0;

    auto passHash = [&]() -> std::string
    {
        const auto rset = db::preparedStmt("SELECT accounts.id, accounts.status, accounts.password FROM accounts WHERE accounts.login = ?", username);
        if (rset && rset->rowsCount() != 0 && rset->next())
        {
            accountID = rset->get<uint32>("id");
            status    = rset->get<uint32>("status");
            return rset->get<std::string>("password");
        }
        return "";
    }();

    if (loginHelpers::isBcryptHash(passHash))
    {
        // It's a BCrypt hash, so we can validate it.
        if (!BCrypt::validatePassword(password, passHash))
        {
            return std::nullopt;
        }
    }
    else
    {
        // It's not a BCrypt hash, so we need to use Maria's PASSWORD() to check if the password is actually correct,
        // and then update the password to a BCrypt hash.
        const auto rset = db::preparedStmt("SELECT PASSWORD(?)", password);
        if (rset && rset->rowsCount() != 0 && rset->next())
        {
            if (rset->get<std::string>(0) != passHash)
            {
                return std::nullopt;
            }

            passHash = BCrypt::generateHash(password);
            db::preparedStmt("UPDATE accounts SET accounts.password = ? WHERE accounts.login = ?", passHash, username);
            if (!BCrypt::validatePassword(password, passHash))
            {
                return std::nullopt;
            }
        }
    }
    return std::make_pair(accountID, status);
}
