/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

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

#include <test/test_application.h>

#include <common/lua.h>
#include <common/tracy.h>

#include <cstdlib>
#include <iostream>
#include <memory>

auto runEarthTimeSelfTests() -> bool;
auto runIPPSelfTests() -> bool;
auto runBitpackSelfTests() -> bool;
auto runStringCodecSelfTests() -> bool;
auto runSpatialOrientationSelfTests() -> bool;
auto runNumericUtilsSelfTests() -> bool;
auto runStringUtilsSelfTests() -> bool;
auto runLookParserSelfTests() -> bool;
auto runHashUtilsSelfTests() -> bool;
auto runRngEnginesSelfTests() -> bool;
auto runXiBitsetSelfTests() -> bool;
auto runCircularBufferSelfTests() -> bool;
auto runTimerHelpersSelfTests() -> bool;
auto runSynchronizedSelfTests() -> bool;
auto runTypedFlagsSelfTests() -> bool;
auto runMaybeOptionalSelfTests() -> bool;
auto runUUIDGenerationSelfTests() -> bool;
auto runZlibCompressedSizeSelfTests() -> bool;
auto runArgumentsParserSelfTests() -> bool;
auto runVersionInfoSelfTests() -> bool;
auto runSingletonHelperSelfTests() -> bool;
auto runDatabaseQueryHelpersSelfTests() -> bool;
auto runLoggingUntrustedStringsSelfTests() -> bool;
auto runLoggingBacktraceBufferSelfTests() -> bool;
auto runLoginMalformedStringSelfTests() -> bool;
auto runLoginDisplayBitmasksSelfTests() -> bool;
auto runLoginErrorPacketSelfTests() -> bool;
auto runLoginOTPHelpersSelfTests() -> bool;
auto runSettingsNumberHelperSelfTests() -> bool;
auto runSettingsValueConversionsSelfTests() -> bool;
auto runLoginPacketHashSelfTests() -> bool;
auto runLoginJSONGetSelfTests() -> bool;
auto runMariaDBConnectionLossSelfTests() -> bool;
auto runMariaDBResultCellConversionsSelfTests() -> bool;
auto runMariaDBResultSetMetadataSelfTests() -> bool;
auto runResultSetTypedGettersSelfTests() -> bool;
auto runDatabaseBindingSelfTests() -> bool;
auto runActiveDatabaseDispatchSelfTests() -> bool;
auto runDatabaseConvenienceHelpersSelfTests() -> bool;
auto runCachingDatabaseSelfTests() -> bool;
auto runIPCChannelSelfTests() -> bool;
auto runZmqEndpointOpenedSelfTests() -> bool;
auto runFilewatcherQueueSelfTests() -> bool;
auto runLoggingJsonRendererSelfTests() -> bool;
auto runLoggingContextStackSelfTests() -> bool;
auto runXirandHelpersSelfTests() -> bool;
auto runIPCMessageTypeSelfTests() -> bool;
auto runIPCChatPayloadSelfTests() -> bool;
auto runIPCSessionPayloadSelfTests() -> bool;
auto runIPCPartyPayloadSelfTests() -> bool;
auto runIPCLinkshellPayloadSelfTests() -> bool;
auto runIPCGMCallPayloadSelfTests() -> bool;
auto runIPCRegionalPayloadSelfTests() -> bool;
auto runIPCStandardMessagePayloadSelfTests() -> bool;
auto runIPCMessageFramingSelfTests() -> bool;
auto runTracyHexHelperSelfTests() -> bool;
auto runMapEnumPrimitiveSelfTests() -> bool;
auto runLoginErrorCodeSelfTests() -> bool;
auto runLoginAuthConstantSelfTests() -> bool;
auto runLoginPacketHelperSelfTests() -> bool;
auto runLoginWorldListPacketSelfTests() -> bool;
auto runMapActionEnumPrimitiveSelfTests() -> bool;
auto runSearchPacketBufferSelfTests() -> bool;
auto runMapPacketBasicHeaderSelfTests() -> bool;
auto runC2SPacketValidationScalarSelfTests() -> bool;
auto runC2SPacketIDEnumSelfTests() -> bool;
auto runC2SPacketRateLimiterSelfTests() -> bool;
auto runS2CPacketIDEnumSelfTests() -> bool;
auto runC2SPacketBaseHelperSelfTests() -> bool;
auto runMapEmoteEnumSelfTests() -> bool;
auto runMapFourCCExdataEnumSelfTests() -> bool;
auto runMapMsgBasicEnumSelfTests() -> bool;
auto runMapKeyItemEnumSelfTests() -> bool;
auto runC2SGameOKPacketSelfTests() -> bool;
auto runC2SNetEndPacketSelfTests() -> bool;
auto runC2SCLStatPacketSelfTests() -> bool;
auto runC2SZoneTransitionPacketSelfTests() -> bool;
auto runC2SCharReqPacketSelfTests() -> bool;
auto runC2SCharReq2PacketSelfTests() -> bool;
auto runC2SHeaderOnlyRequestPacketSelfTests() -> bool;
auto runC2SCLIStatusPacketSelfTests() -> bool;
auto runC2SFragmentsPacketSelfTests() -> bool;
auto runC2SPosPacketSelfTests() -> bool;
auto runC2SActionPacketSelfTests() -> bool;
auto runC2SFriendPassUnknownPacketSelfTests() -> bool;
auto runC2SItemDumpMovePacketSelfTests() -> bool;
auto runC2SItemLookupPacketSelfTests() -> bool;
auto runC2SPlayerTradePacketSelfTests() -> bool;
auto runC2SItemActionPacketSelfTests() -> bool;
auto runC2SItemContainerOpsPacketSelfTests() -> bool;
auto runC2SBlacklistPacketSelfTests() -> bool;
auto runC2STrophyPacketSelfTests() -> bool;
auto runC2SEventUpdatePacketSelfTests() -> bool;
auto runC2SRecipeMiscPacketSelfTests() -> bool;
auto runC2SMotionMapRectPacketSelfTests() -> bool;
auto runC2SScenarioItemPacketSelfTests() -> bool;
auto runC2SGroupPacketSelfTests() -> bool;
auto runC2SShopPacketSelfTests() -> bool;
auto runC2SCombineAskPacketSelfTests() -> bool;
auto runC2SChocoboRaceReqPacketSelfTests() -> bool;
auto runC2SDicePacketSelfTests() -> bool;
auto runC2SGuildPacketSelfTests() -> bool;
auto runC2SSwitchNominatePacketSelfTests() -> bool;
auto runC2SChatAssistPacketSelfTests() -> bool;
auto runC2SPlayerPointsPacketSelfTests() -> bool;
auto runC2SComlinkMyRoomPacketSelfTests() -> bool;
auto runC2SHelpdeskMapPacketSelfTests() -> bool;
auto runC2SConfigPacketSelfTests() -> bool;
auto runC2SDungeonInspectPacketSelfTests() -> bool;
auto runC2SUserLinkshellMessagePacketSelfTests() -> bool;
auto runC2SLogoutStancePacketSelfTests() -> bool;
auto runC2SRescueBuffSubmapPacketSelfTests() -> bool;
auto runC2STrackingPacketSelfTests() -> bool;
auto runC2SMyRoomFurniturePacketSelfTests() -> bool;
auto runC2SMyRoomPlantPacketSelfTests() -> bool;
auto runC2SMyRoomJobPacketSelfTests() -> bool;
auto runC2SExtendedJobPacketSelfTests() -> bool;
auto runC2SBazaarBuyerPacketSelfTests() -> bool;
auto runC2SBazaarSellerPacketSelfTests() -> bool;
auto runC2SROEPacketSelfTests() -> bool;
auto runC2SCurrencyRequestPacketSelfTests() -> bool;
auto runC2SBattlefieldSitChairPacketSelfTests() -> bool;
auto runC2SUnityPacketSelfTests() -> bool;
auto runC2STailMiscPacketSelfTests() -> bool;
auto runC2SGMPacketSelfTests() -> bool;
auto runC2SLockstylePacketSelfTests() -> bool;
auto runC2SEquipmentPacketSelfTests() -> bool;
auto runC2SFishingPacketSelfTests() -> bool;
auto runC2SPBXPacketSelfTests() -> bool;
auto runC2SAUCPacketSelfTests() -> bool;
auto runC2SLoginPacketSelfTests() -> bool;
auto runS2CHeaderOnlyPacketSelfTests() -> bool;
auto runS2CBallistaPacketSelfTests() -> bool;
auto runS2CAssistPacketSelfTests() -> bool;
auto runS2CAUCPacketSelfTests() -> bool;
auto runS2CBattle2PacketSelfTests() -> bool;
auto runS2CBattlefieldPacketSelfTests() -> bool;
auto runS2CBattleMessagePacketSelfTests() -> bool;
auto runS2CBlackEditPacketSelfTests() -> bool;
auto runS2CBlackListPacketSelfTests() -> bool;
auto runS2CChatStdPacketSelfTests() -> bool;
auto runS2CChocoboRacingPacketSelfTests() -> bool;
auto runS2CDigPacketSelfTests() -> bool;
auto runS2CEffectPacketSelfTests() -> bool;
auto runS2CEventPacketSelfTests() -> bool;
auto runS2CEventUCOffPacketSelfTests() -> bool;
auto runS2CEntityVisPacketSelfTests() -> bool;
auto runS2CFragmentsServmesPacketSelfTests() -> bool;
auto runS2CSwitchPacketSelfTests() -> bool;
auto runS2CEventStrPacketSelfTests() -> bool;
auto runS2CEventNumPacketSelfTests() -> bool;
auto runS2CLinkConciergeHeaderPacketSelfTests() -> bool;
auto runS2CMagicSchedulorPacketSelfTests() -> bool;
auto runS2CMapSchedulorPacketSelfTests() -> bool;
auto runS2CLogoutPacketSelfTests() -> bool;
auto runS2CWeatherPacketSelfTests() -> bool;
auto runS2CShopPacketSelfTests() -> bool;
auto runS2CReqSubmapNumPacketSelfTests() -> bool;
auto runS2CMusicPacketSelfTests() -> bool;
auto runS2CMusicVolumePacketSelfTests() -> bool;
auto runS2CEquipClearPacketSelfTests() -> bool;
auto runS2CEquipListPacketSelfTests() -> bool;
auto runS2CDefaultPacketSelfTests() -> bool;
auto runS2CGroupList2PacketSelfTests() -> bool;
auto runS2CItemSearchPacketSelfTests() -> bool;
auto runS2CItemListPacketSelfTests() -> bool;
auto runS2CItemNumPacketSelfTests() -> bool;
auto runS2CItemSubcontainerPacketSelfTests() -> bool;
auto runS2CMyRoomPacketSelfTests() -> bool;
auto runS2CPacketControlPacketSelfTests() -> bool;
auto runS2CPBXResultPacketSelfTests() -> bool;
auto runS2CPartyReqPacketSelfTests() -> bool;
auto runS2CPendingNumPacketSelfTests() -> bool;
auto runS2CPendingStrPacketSelfTests() -> bool;
auto runS2CFAQGMParamPacketSelfTests() -> bool;
auto runS2CFishPacketSelfTests() -> bool;
auto runS2CFriendPassPacketSelfTests() -> bool;
auto runS2CRegistrationPacketSelfTests() -> bool;
auto runS2CGMPacketSelfTests() -> bool;
auto runS2CGMSupportPacketSelfTests() -> bool;
auto runS2CMessagePacketSelfTests() -> bool;
auto runS2CSchedulorPacketSelfTests() -> bool;
auto runS2CSystemMesPacketSelfTests() -> bool;
auto runS2CTalkNumPacketSelfTests() -> bool;
auto runS2CTalkNumWorkPacketSelfTests() -> bool;
auto runS2CTalkNumWork2PacketSelfTests() -> bool;
auto runS2CTalkNumNamePacketSelfTests() -> bool;
auto runS2CTrackingStatePacketSelfTests() -> bool;
auto runS2CTranslatePacketSelfTests() -> bool;
auto runS2CWPosPacketSelfTests() -> bool;

namespace
{
    auto runOmegaSelfTests() -> bool
    {
        bool ok = true;
        ok = runEarthTimeSelfTests() && ok;
        ok = runIPPSelfTests() && ok;
        ok = runBitpackSelfTests() && ok;
        ok = runStringCodecSelfTests() && ok;
        ok = runSpatialOrientationSelfTests() && ok;
        ok = runNumericUtilsSelfTests() && ok;
        ok = runStringUtilsSelfTests() && ok;
        ok = runLookParserSelfTests() && ok;
        ok = runHashUtilsSelfTests() && ok;
        ok = runRngEnginesSelfTests() && ok;
        ok = runXiBitsetSelfTests() && ok;
        ok = runCircularBufferSelfTests() && ok;
        ok = runTimerHelpersSelfTests() && ok;
        ok = runSynchronizedSelfTests() && ok;
        ok = runTypedFlagsSelfTests() && ok;
        ok = runMaybeOptionalSelfTests() && ok;
        ok = runUUIDGenerationSelfTests() && ok;
        ok = runZlibCompressedSizeSelfTests() && ok;
        ok = runArgumentsParserSelfTests() && ok;
        ok = runVersionInfoSelfTests() && ok;
        ok = runSingletonHelperSelfTests() && ok;
        ok = runDatabaseQueryHelpersSelfTests() && ok;
        ok = runLoggingUntrustedStringsSelfTests() && ok;
        ok = runLoggingBacktraceBufferSelfTests() && ok;
        ok = runLoginMalformedStringSelfTests() && ok;
        ok = runLoginDisplayBitmasksSelfTests() && ok;
        ok = runLoginErrorPacketSelfTests() && ok;
        ok = runLoginOTPHelpersSelfTests() && ok;
        ok = runSettingsNumberHelperSelfTests() && ok;
        ok = runSettingsValueConversionsSelfTests() && ok;
        ok = runLoginPacketHashSelfTests() && ok;
        ok = runLoginJSONGetSelfTests() && ok;
        ok = runMariaDBConnectionLossSelfTests() && ok;
        ok = runMariaDBResultCellConversionsSelfTests() && ok;
        ok = runMariaDBResultSetMetadataSelfTests() && ok;
        ok = runResultSetTypedGettersSelfTests() && ok;
        ok = runDatabaseBindingSelfTests() && ok;
        ok = runActiveDatabaseDispatchSelfTests() && ok;
        ok = runDatabaseConvenienceHelpersSelfTests() && ok;
        ok = runCachingDatabaseSelfTests() && ok;
        ok = runIPCChannelSelfTests() && ok;
        ok = runZmqEndpointOpenedSelfTests() && ok;
        ok = runFilewatcherQueueSelfTests() && ok;
        ok = runLoggingJsonRendererSelfTests() && ok;
        ok = runLoggingContextStackSelfTests() && ok;
        ok = runXirandHelpersSelfTests() && ok;
        ok = runIPCMessageTypeSelfTests() && ok;
        ok = runIPCChatPayloadSelfTests() && ok;
        ok = runIPCSessionPayloadSelfTests() && ok;
        ok = runIPCPartyPayloadSelfTests() && ok;
        ok = runIPCLinkshellPayloadSelfTests() && ok;
        ok = runIPCGMCallPayloadSelfTests() && ok;
        ok = runIPCRegionalPayloadSelfTests() && ok;
        ok = runIPCStandardMessagePayloadSelfTests() && ok;
        ok = runIPCMessageFramingSelfTests() && ok;
        ok = runTracyHexHelperSelfTests() && ok;
        ok = runMapEnumPrimitiveSelfTests() && ok;
        ok = runLoginErrorCodeSelfTests() && ok;
        ok = runLoginAuthConstantSelfTests() && ok;
        ok = runLoginPacketHelperSelfTests() && ok;
        ok = runLoginWorldListPacketSelfTests() && ok;
        ok = runMapActionEnumPrimitiveSelfTests() && ok;
        ok = runSearchPacketBufferSelfTests() && ok;
        ok = runMapPacketBasicHeaderSelfTests() && ok;
        ok = runC2SPacketValidationScalarSelfTests() && ok;
        ok = runC2SPacketIDEnumSelfTests() && ok;
        ok = runC2SPacketRateLimiterSelfTests() && ok;
        ok = runS2CPacketIDEnumSelfTests() && ok;
        ok = runC2SPacketBaseHelperSelfTests() && ok;
        ok = runMapEmoteEnumSelfTests() && ok;
        ok = runMapFourCCExdataEnumSelfTests() && ok;
        ok = runMapMsgBasicEnumSelfTests() && ok;
        ok = runMapKeyItemEnumSelfTests() && ok;
        ok = runC2SGameOKPacketSelfTests() && ok;
        ok = runC2SNetEndPacketSelfTests() && ok;
        ok = runC2SCLStatPacketSelfTests() && ok;
        ok = runC2SZoneTransitionPacketSelfTests() && ok;
        ok = runC2SCharReqPacketSelfTests() && ok;
        ok = runC2SCharReq2PacketSelfTests() && ok;
        ok = runC2SHeaderOnlyRequestPacketSelfTests() && ok;
        ok = runC2SCLIStatusPacketSelfTests() && ok;
        ok = runC2SFragmentsPacketSelfTests() && ok;
        ok = runC2SPosPacketSelfTests() && ok;
        ok = runC2SActionPacketSelfTests() && ok;
        ok = runC2SFriendPassUnknownPacketSelfTests() && ok;
        ok = runC2SItemDumpMovePacketSelfTests() && ok;
        ok = runC2SItemLookupPacketSelfTests() && ok;
        ok = runC2SPlayerTradePacketSelfTests() && ok;
        ok = runC2SItemActionPacketSelfTests() && ok;
        ok = runC2SItemContainerOpsPacketSelfTests() && ok;
        ok = runC2SBlacklistPacketSelfTests() && ok;
        ok = runC2STrophyPacketSelfTests() && ok;
        ok = runC2SEventUpdatePacketSelfTests() && ok;
        ok = runC2SRecipeMiscPacketSelfTests() && ok;
        ok = runC2SMotionMapRectPacketSelfTests() && ok;
        ok = runC2SScenarioItemPacketSelfTests() && ok;
        ok = runC2SGroupPacketSelfTests() && ok;
        ok = runC2SShopPacketSelfTests() && ok;
        ok = runC2SCombineAskPacketSelfTests() && ok;
        ok = runC2SChocoboRaceReqPacketSelfTests() && ok;
        ok = runC2SDicePacketSelfTests() && ok;
        ok = runC2SGuildPacketSelfTests() && ok;
        ok = runC2SSwitchNominatePacketSelfTests() && ok;
        ok = runC2SChatAssistPacketSelfTests() && ok;
        ok = runC2SPlayerPointsPacketSelfTests() && ok;
        ok = runC2SComlinkMyRoomPacketSelfTests() && ok;
        ok = runC2SHelpdeskMapPacketSelfTests() && ok;
        ok = runC2SConfigPacketSelfTests() && ok;
        ok = runC2SDungeonInspectPacketSelfTests() && ok;
        ok = runC2SUserLinkshellMessagePacketSelfTests() && ok;
        ok = runC2SLogoutStancePacketSelfTests() && ok;
        ok = runC2SRescueBuffSubmapPacketSelfTests() && ok;
        ok = runC2STrackingPacketSelfTests() && ok;
        ok = runC2SMyRoomFurniturePacketSelfTests() && ok;
        ok = runC2SMyRoomPlantPacketSelfTests() && ok;
        ok = runC2SMyRoomJobPacketSelfTests() && ok;
        ok = runC2SExtendedJobPacketSelfTests() && ok;
        ok = runC2SBazaarBuyerPacketSelfTests() && ok;
        ok = runC2SBazaarSellerPacketSelfTests() && ok;
        ok = runC2SROEPacketSelfTests() && ok;
        ok = runC2SCurrencyRequestPacketSelfTests() && ok;
        ok = runC2SBattlefieldSitChairPacketSelfTests() && ok;
        ok = runC2SUnityPacketSelfTests() && ok;
        ok = runC2STailMiscPacketSelfTests() && ok;
        ok = runC2SGMPacketSelfTests() && ok;
        ok = runC2SLockstylePacketSelfTests() && ok;
        ok = runC2SEquipmentPacketSelfTests() && ok;
        ok = runC2SFishingPacketSelfTests() && ok;
        ok = runC2SPBXPacketSelfTests() && ok;
        ok = runC2SAUCPacketSelfTests() && ok;
        ok = runC2SLoginPacketSelfTests() && ok;
        ok = runS2CHeaderOnlyPacketSelfTests() && ok;
        ok = runS2CBallistaPacketSelfTests() && ok;
        ok = runS2CAssistPacketSelfTests() && ok;
        ok = runS2CAUCPacketSelfTests() && ok;
        ok = runS2CBattle2PacketSelfTests() && ok;
        ok = runS2CBattlefieldPacketSelfTests() && ok;
        ok = runS2CBattleMessagePacketSelfTests() && ok;
        ok = runS2CBlackEditPacketSelfTests() && ok;
        ok = runS2CBlackListPacketSelfTests() && ok;
        ok = runS2CChatStdPacketSelfTests() && ok;
        ok = runS2CChocoboRacingPacketSelfTests() && ok;
        ok = runS2CDigPacketSelfTests() && ok;
        ok = runS2CEffectPacketSelfTests() && ok;
        ok = runS2CEventPacketSelfTests() && ok;
        ok = runS2CEventUCOffPacketSelfTests() && ok;
        ok = runS2CEntityVisPacketSelfTests() && ok;
        ok = runS2CFragmentsServmesPacketSelfTests() && ok;
        ok = runS2CSwitchPacketSelfTests() && ok;
        ok = runS2CEventStrPacketSelfTests() && ok;
        ok = runS2CEventNumPacketSelfTests() && ok;
        ok = runS2CLinkConciergeHeaderPacketSelfTests() && ok;
        ok = runS2CMagicSchedulorPacketSelfTests() && ok;
        ok = runS2CMapSchedulorPacketSelfTests() && ok;
        ok = runS2CLogoutPacketSelfTests() && ok;
        ok = runS2CWeatherPacketSelfTests() && ok;
        ok = runS2CShopPacketSelfTests() && ok;
        ok = runS2CReqSubmapNumPacketSelfTests() && ok;
        ok = runS2CMusicPacketSelfTests() && ok;
        ok = runS2CMusicVolumePacketSelfTests() && ok;
        ok = runS2CEquipClearPacketSelfTests() && ok;
        ok = runS2CEquipListPacketSelfTests() && ok;
        ok = runS2CDefaultPacketSelfTests() && ok;
        ok = runS2CGroupList2PacketSelfTests() && ok;
        ok = runS2CItemSearchPacketSelfTests() && ok;
        ok = runS2CItemListPacketSelfTests() && ok;
        ok = runS2CItemNumPacketSelfTests() && ok;
        ok = runS2CItemSubcontainerPacketSelfTests() && ok;
        ok = runS2CMyRoomPacketSelfTests() && ok;
        ok = runS2CPacketControlPacketSelfTests() && ok;
        ok = runS2CPBXResultPacketSelfTests() && ok;
        ok = runS2CPartyReqPacketSelfTests() && ok;
        ok = runS2CPendingNumPacketSelfTests() && ok;
        ok = runS2CPendingStrPacketSelfTests() && ok;
        ok = runS2CFAQGMParamPacketSelfTests() && ok;
        ok = runS2CFishPacketSelfTests() && ok;
        ok = runS2CFriendPassPacketSelfTests() && ok;
        ok = runS2CRegistrationPacketSelfTests() && ok;
        ok = runS2CGMPacketSelfTests() && ok;
        ok = runS2CGMSupportPacketSelfTests() && ok;
        ok = runS2CMessagePacketSelfTests() && ok;
        ok = runS2CSchedulorPacketSelfTests() && ok;
        ok = runS2CSystemMesPacketSelfTests() && ok;
        ok = runS2CTalkNumPacketSelfTests() && ok;
        ok = runS2CTalkNumWorkPacketSelfTests() && ok;
        ok = runS2CTalkNumWork2PacketSelfTests() && ok;
        ok = runS2CTalkNumNamePacketSelfTests() && ok;
        ok = runS2CTrackingStatePacketSelfTests() && ok;
        ok = runS2CTranslatePacketSelfTests() && ok;
        ok = runS2CWPosPacketSelfTests() && ok;

        return ok;
    }
}

int main(int argc, char** argv)
{
    TracySetThreadName("Test Thread");

    const auto omegaSelfTestsSuccess = runOmegaSelfTests();

    auto testApp = std::make_unique<TestApplication>(argc, argv);

    const auto success = testApp->run();

    const auto exitCode = success && omegaSelfTestsSuccess ? EXIT_SUCCESS : EXIT_FAILURE;

    // Explicitly destroy TestApplication before the lua state get cleaned up
    testApp.reset();

    // TODO: This should be in ~Application but it needs more testing for xi_map
    // TODO: This wouldn't be needed if lua wasn't global
    lua_cleanup();

#ifdef TRACY_ENABLE
    // TODO: Tracy profiler exits when program is done
    // Is there an option to keep it running despite the program exiting?
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
#endif

    return exitCode;
}
