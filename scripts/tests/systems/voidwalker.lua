require('scripts/globals/voidwalker')

describe('Voidwalker nearest mob', function()
    it('returns the closest candidate with its key item and distance', function()
        local candidates =
        {
            { mobId = 100, keyItem = 200 },
            { mobId = 101, keyItem = 201 },
            { mobId = 102, keyItem = 202 }
        }
        local distances = { [100] = 20, [101] = 5, [102] = 10 }
        local nearest = xi.voidwalker.nearestMob(candidates, function(mobId) return distances[mobId] end)

        assert(nearest.mobId == 101 and nearest.keyItem == 201 and nearest.distance == 5)
        assert(xi.voidwalker.nearestMob({}, function() return 1 end) == nil)
        assert(xi.voidwalker.nearestMob(nil, function() return 1 end) == nil)
        assert(xi.voidwalker.nearestMob(candidates, nil) == nil)
    end)
end)

describe('Voidwalker healing range', function()
    it('spawns nearby mobs, hints at intermediate range, and rejects distant mobs', function()
        assert(xi.voidwalker.healingRangeOutcome(4) == 'spawn')
        assert(xi.voidwalker.healingRangeOutcome(4.1) == 'hint')
        assert(xi.voidwalker.healingRangeOutcome(299.9) == 'hint')
        assert(xi.voidwalker.healingRangeOutcome(300) == 'too_far')
    end)
end)

describe('Voidwalker original-popper upgrade', function()
    it('requires a killer, an existing out-of-alliance popper, and a missing key item', function()
        assert(xi.voidwalker.shouldUpgradeOriginalPopper(true, true, false, false))
        assert(not xi.voidwalker.shouldUpgradeOriginalPopper(false, true, false, false))
        assert(not xi.voidwalker.shouldUpgradeOriginalPopper(true, false, false, false))
        assert(not xi.voidwalker.shouldUpgradeOriginalPopper(true, true, true, false))
        assert(not xi.voidwalker.shouldUpgradeOriginalPopper(true, true, false, true))
    end)
end)

describe('Voidwalker killer upgrade', function()
    it('requires the pop abyssite and no next abyssite', function()
        assert(xi.voidwalker.shouldUpgradeKiller(true, false))
        assert(not xi.voidwalker.shouldUpgradeKiller(false, false))
        assert(not xi.voidwalker.shouldUpgradeKiller(true, true))
    end)
end)

describe('Voidwalker pop abyssite consumption', function()
    it('retains Clear and Colorful abyssites but consumes later tiers', function()
        assert(not xi.voidwalker.shouldConsumePopAbyssite(xi.keyItem.CLEAR_ABYSSITE))
        assert(not xi.voidwalker.shouldConsumePopAbyssite(xi.keyItem.COLORFUL_ABYSSITE))
        assert(xi.voidwalker.shouldConsumePopAbyssite(xi.keyItem.BLUE_ABYSSITE))
        assert(xi.voidwalker.shouldConsumePopAbyssite(xi.keyItem.BLACK_ABYSSITE))
    end)
end)

describe('Voidwalker upgrade messages', function()
    it('chooses messages by current and next abyssite', function()
        assert(xi.voidwalker.upgradeMessageKind(xi.keyItem.CLEAR_ABYSSITE, xi.keyItem.COLORFUL_ABYSSITE) == 'upgrade_1')
        assert(xi.voidwalker.upgradeMessageKind(xi.keyItem.COLORFUL_ABYSSITE, xi.keyItem.BLUE_ABYSSITE) == 'upgrade_2')
        assert(xi.voidwalker.upgradeMessageKind(xi.keyItem.BLUE_ABYSSITE, xi.keyItem.BLACK_ABYSSITE) == 'obtain')
        assert(xi.voidwalker.upgradeMessageKind(xi.keyItem.BLUE_ABYSSITE, xi.keyItem.ORANGE_ABYSSITE) == nil)
    end)
end)

describe('Voidwalker NPC update', function()
    it('returns the purchase response only for the correct event option', function()
        assert(xi.voidwalker.npcUpdateOutcome(10120, 3, false, false) == 'no_gil')
        assert(xi.voidwalker.npcUpdateOutcome(10120, 3, true, true) == 'has_clear_abyssite')
        assert(xi.voidwalker.npcUpdateOutcome(10120, 3, true, false) == 'purchase_available')
        assert(xi.voidwalker.npcUpdateOutcome(10119, 3, true, false) == nil)
        assert(xi.voidwalker.npcUpdateOutcome(10120, 2, true, false) == nil)
    end)
end)

describe('Voidwalker fight despawn', function()
    it('requires a spawned mob and uses strict timeout and distance boundaries', function()
        assert(not xi.voidwalker.shouldDespawnOnFight(false, 7201, 0, 26))
        assert(not xi.voidwalker.shouldDespawnOnFight(true, 7200, 0, 25))
        assert(xi.voidwalker.shouldDespawnOnFight(true, 7201, 0, 25))
        assert(xi.voidwalker.shouldDespawnOnFight(true, 7200, 0, 25.1))
    end)
end)

describe('Voidwalker NPC finish', function()
    it('selects purchase and selected-abyssite removal actions', function()
        local purchase = xi.voidwalker.npcFinishAction(10120, 1)
        assert(purchase.kind == 'buy_clear' and purchase.keyItem == xi.keyItem.CLEAR_ABYSSITE)

        local remove = xi.voidwalker.npcFinishAction(10120, bit.lshift(2, 4) + 2)
        assert(remove.kind == 'remove_abyssite' and remove.keyItem == xi.keyItem.COLORFUL_ABYSSITE)
        assert(xi.voidwalker.npcFinishAction(10119, 1) == nil)
    end)
end)

describe('Voidwalker spawn modifiers', function()
    it('provides status, mod, and immunity plans by mob name', function()
        local krabkatoa = xi.voidwalker.spawnModifierPlan('Krabkatoa')
        assert(#krabkatoa == 2)
        assert(krabkatoa[1].kind == 'status' and krabkatoa[1].effect == xi.effect.REGAIN and krabkatoa[1].power == 10)
        assert(krabkatoa[2].kind == 'mod' and krabkatoa[2].mod == xi.mod.DOUBLE_ATTACK and krabkatoa[2].value == 10)

        local tammuz = xi.voidwalker.spawnModifierPlan('Tammuz')
        assert(#tammuz == 1 and tammuz[1].kind == 'status' and tammuz[1].effect == xi.effect.MIGHTY_STRIKES and tammuz[1].power == 1)

        for _, mobName in ipairs({ 'Erebus', 'Raker_Bee' }) do
            local immunities = xi.voidwalker.spawnModifierPlan(mobName)
            assert(#immunities == 2)
            assert(immunities[1].immunity == xi.immunity.GRAVITY and immunities[2].immunity == xi.immunity.BIND)
        end

        local gjenganger = xi.voidwalker.spawnModifierPlan('Gjenganger')
        assert(#gjenganger == 1 and gjenganger[1].kind == 'immunity' and gjenganger[1].immunity == xi.immunity.STUN)
        assert(xi.voidwalker.spawnModifierPlan('Unknown') == nil)
    end)
end)

describe('Voidwalker Capricornus Recoil Dive', function()
    it('requires Mighty Strikes and an idle mob', function()
        assert(xi.voidwalker.shouldCapricornusUseRecoilDive(true, false))
        assert(not xi.voidwalker.shouldCapricornusUseRecoilDive(false, false))
        assert(not xi.voidwalker.shouldCapricornusUseRecoilDive(true, true))
    end)
end)

describe('Voidwalker Jyeshtha mob-skill reset', function()
    it('clears only a pending use without Mighty Strikes', function()
        assert(xi.voidwalker.shouldResetJyeshthaMobSkillUse(1, false))
        assert(not xi.voidwalker.shouldResetJyeshthaMobSkillUse(0, false))
        assert(not xi.voidwalker.shouldResetJyeshthaMobSkillUse(1, true))
    end)
end)

describe('Voidwalker Erebus Hundred Fists', function()
    it('requires Blood Weapon and no existing Hundred Fists', function()
        assert(xi.voidwalker.shouldErebusApplyHundredFists(true, false))
        assert(not xi.voidwalker.shouldErebusApplyHundredFists(false, false))
        assert(not xi.voidwalker.shouldErebusApplyHundredFists(true, true))
    end)
end)

describe('Voidwalker fixed-HPP mob skills', function()
    it('provides every scheduled skill plan and its status gate', function()
        local capricornus = xi.voidwalker.mobSkillEveryHPPPlan('Capricornus')
        assert(capricornus.every == 20 and capricornus.start == 80 and capricornus.mobSkill == xi.mobSkill.MIGHTY_STRIKES_1 and capricornus.absentEffect == xi.effect.MIGHTY_STRIKES)

        local yacumama = xi.voidwalker.mobSkillEveryHPPPlan('Yacumama')
        assert(yacumama.mobSkill == xi.mobSkill.HUNDRED_FISTS_1 and yacumama.absentEffect == xi.effect.HUNDRED_FISTS)

        for _, mobName in ipairs({ 'Shoggoth', 'Skuld' }) do
            local plan = xi.voidwalker.mobSkillEveryHPPPlan(mobName)
            assert(plan.every == 20 and plan.start == 80 and plan.mobSkill == xi.mobSkill.CHAINSPELL_1 and plan.absentEffect == xi.effect.CHAINSPELL)
        end

        local blobdingnag = xi.voidwalker.mobSkillEveryHPPPlan('Blobdingnag')
        assert(blobdingnag.every == 20 and blobdingnag.start == 82 and blobdingnag.mobSkill == xi.mobSkill.CYTOKINESIS and blobdingnag.absentEffect == nil)

        for _, mobName in ipairs({ 'Farruca_Fly', 'Dawon' }) do
            local plan = xi.voidwalker.mobSkillEveryHPPPlan(mobName)
            assert(plan.every == 20 and plan.start == 80 and plan.mobSkill == xi.mobSkill.PERFECT_DODGE_1 and plan.absentEffect == xi.effect.PERFECT_DODGE)
        end

        assert(xi.voidwalker.mobSkillEveryHPPPlan('Unknown') == nil)
    end)
end)

describe('Voidwalker random mob skills', function()
    it('provides every random-skill chance, cooldown, status gate, and skill', function()
        local lamprey = xi.voidwalker.randomMobSkillPlan('Lamprey_Lord')
        assert(lamprey.chance == 10 and lamprey.between == 60 and lamprey.statusGate == xi.effect.BLOOD_WEAPON and lamprey.mobSkill == xi.mobSkill.BLOOD_WEAPON_1)

        local jyeshtha = xi.voidwalker.randomMobSkillPlan('Jyeshtha')
        assert(jyeshtha.chance == 30 and jyeshtha.between == 60 and jyeshtha.statusGate == xi.mobSkill.MIGHTY_STRIKES_1 and jyeshtha.mobSkill == xi.mobSkill.MIGHTY_STRIKES_1)

        local erebus = xi.voidwalker.randomMobSkillPlan('Erebus')
        assert(erebus.chance == 30 and erebus.between == 60 and erebus.statusGate == xi.effect.BLOOD_WEAPON and erebus.mobSkill == xi.mobSkill.BLOOD_WEAPON_1)

        local feuerunke = xi.voidwalker.randomMobSkillPlan('Feuerunke')
        assert(feuerunke.chance == 30 and feuerunke.between == 60 and feuerunke.statusGate == xi.effect.HUNDRED_FISTS and feuerunke.mobSkill == xi.mobSkill.HUNDRED_FISTS_1)
        assert(xi.voidwalker.randomMobSkillPlan('Unknown') == nil)
    end)
end)

describe('Voidwalker local-var reset', function()
    it('resets the complete shared local-var plan', function()
        local names = xi.voidwalker.resetLocalVars()
        assert(#names == 6)
        assert(names[1] == '[VoidWalker]PopedBy' and names[2] == '[VoidWalker]checkPopedBy')
        assert(names[3] == '[VoidWalker]PopedWith' and names[4] == '[VoidWalker]PopedAt')
        assert(names[5] == 'MOBSKILL_USE' and names[6] == 'MOBSKILL_TIME')
    end)
end)

describe('Voidwalker spawn presentation', function()
    it('starts mobs invisible, hidden, and untargetable', function()
        local plan = xi.voidwalker.spawnPresentationPlan()
        assert(plan.status == xi.status.INVISIBLE and plan.hideHP and plan.hideName and plan.untargetable)
    end)
end)

describe('Voidwalker healing admission', function()
    it('requires the feature, an abyssite, and zone Voidwalker mobs', function()
        assert(xi.voidwalker.shouldHandleHealing(true, 1, true))
        assert(not xi.voidwalker.shouldHandleHealing(false, 1, true))
        assert(not xi.voidwalker.shouldHandleHealing(true, 0, true))
        assert(not xi.voidwalker.shouldHandleHealing(true, 1, false))
    end)
end)

describe('Voidwalker spawn slot occupancy', function()
    local function slots()
        return
        {
            { pos = { 1, 2, 3 } },
            { pos = { 4, 5, 6 }, mobId = 100 },
            { pos = { 7, 8, 9 }, mobId = 101 },
        }
    end

    it('claims a slot and reports its position', function()
        local positions = slots()
        local pos       = xi.voidwalker.claimPosSlot(positions, 1, 200)

        assert(pos[1] == 1 and pos[2] == 2 and pos[3] == 3)
        assert(positions[1].mobId == 200)
    end)

    it('reports no position for a slot outside the table', function()
        local positions = slots()

        assert(xi.voidwalker.claimPosSlot(positions, 4, 200) == nil)
        assert(xi.voidwalker.claimPosSlot(positions, 0, 200) == nil)
    end)

    it('releases the slot a mob holds', function()
        local positions = slots()

        assert(xi.voidwalker.releasePosSlots(positions, 100) == 1)
        assert(positions[2].mobId == nil)
        assert(positions[3].mobId == 101)
    end)

    it('releases every slot a mob holds', function()
        local positions = slots()
        positions[1].mobId = 101

        assert(xi.voidwalker.releasePosSlots(positions, 101) == 2)
        assert(positions[1].mobId == nil and positions[3].mobId == nil)
    end)

    it('releases nothing for an unknown mob', function()
        local positions = slots()

        assert(xi.voidwalker.releasePosSlots(positions, 999) == 0)
        assert(positions[2].mobId == 100 and positions[3].mobId == 101)
    end)

    it('counts the free slots that bound searchEmptyPos', function()
        local positions = slots()
        assert(xi.voidwalker.emptyPosSlotCount(positions) == 1)

        xi.voidwalker.releasePosSlots(positions, 100)
        assert(xi.voidwalker.emptyPosSlotCount(positions) == 2)

        xi.voidwalker.claimPosSlot(positions, 1, 200)
        assert(xi.voidwalker.emptyPosSlotCount(positions) == 1)
    end)

    it('round-trips a despawn and respawn through the same registry', function()
        local positions = slots()

        assert(xi.voidwalker.releasePosSlots(positions, 101) == 1)
        assert(xi.voidwalker.claimPosSlot(positions, 3, 101) ~= nil)
        assert(positions[3].mobId == 101)
        assert(xi.voidwalker.emptyPosSlotCount(positions) == 1)
    end)
end)

describe('Voidwalker mob fight mixin dispatch', function()
    it('runs the scheduled skill step for every fixed-HPP mob', function()
        for _, mobName in ipairs({ 'Capricornus', 'Yacumama', 'Shoggoth', 'Blobdingnag', 'Farruca_Fly', 'Skuld', 'Dawon' }) do
            local mixin = xi.voidwalker.mobFightMixinPlan(mobName)
            assert(mixin ~= nil, mobName)
            assert(mixin.fixedHPPSkill and not mixin.randomSkill, mobName)
        end
    end)

    it('runs the random skill step for every random-skill mob', function()
        for _, mobName in ipairs({ 'Lamprey_Lord', 'Jyeshtha', 'Erebus', 'Feuerunke' }) do
            local mixin = xi.voidwalker.mobFightMixinPlan(mobName)
            assert(mixin ~= nil, mobName)
            assert(mixin.randomSkill and not mixin.fixedHPPSkill, mobName)
        end
    end)

    it('attaches the special step only to its three mobs', function()
        assert(xi.voidwalker.mobFightMixinPlan('Capricornus').special == 'recoil_dive')
        assert(xi.voidwalker.mobFightMixinPlan('Jyeshtha').special == 'mob_skill_use_reset')
        assert(xi.voidwalker.mobFightMixinPlan('Erebus').special == 'hundred_fists')

        assert(xi.voidwalker.mobFightMixinPlan('Yacumama').special == nil)
        assert(xi.voidwalker.mobFightMixinPlan('Lamprey_Lord').special == nil)
    end)

    it('has no mixin for spawn-modifier-only mobs or unknown names', function()
        assert(xi.voidwalker.mobFightMixinPlan('Krabkatoa') == nil)
        assert(xi.voidwalker.mobFightMixinPlan('Tammuz') == nil)
        assert(xi.voidwalker.mobFightMixinPlan('Raker_Bee') == nil)
        assert(xi.voidwalker.mobFightMixinPlan('Gjenganger') == nil)
        assert(xi.voidwalker.mobFightMixinPlan('Not_A_Voidwalker') == nil)
    end)

    it('never selects both skill steps for one mob', function()
        for _, mobName in ipairs({
            'Capricornus', 'Yacumama', 'Lamprey_Lord', 'Shoggoth', 'Jyeshtha',
            'Blobdingnag', 'Farruca_Fly', 'Skuld', 'Erebus', 'Feuerunke', 'Dawon'
        }) do
            local mixin = xi.voidwalker.mobFightMixinPlan(mobName)
            assert(not (mixin.fixedHPPSkill and mixin.randomSkill), mobName)
        end
    end)
end)

describe('Voidwalker popper alliance scan', function()
    it('matches the popper against the killer alliance roster', function()
        assert(xi.voidwalker.popperInAlliance({ 10, 20, 30 }, 20, true))
        assert(not xi.voidwalker.popperInAlliance({ 10, 20, 30 }, 40, true))
        assert(not xi.voidwalker.popperInAlliance({}, 20, true))
    end)

    it('reports no match when the popper has left the server', function()
        assert(not xi.voidwalker.popperInAlliance({ 10, 20, 30 }, nil, false))
    end)
end)

describe('Voidwalker mob death upgrade dispatch', function()
    it('rolls for both the absent popper and the killer', function()
        local plan = xi.voidwalker.mobDeathUpgradePlan(true, true, false, false, true, false)
        assert(plan.upgradePopper and plan.upgradeKiller)
    end)

    it('skips the popper roll when the reporting player is not the killer', function()
        local plan = xi.voidwalker.mobDeathUpgradePlan(false, true, false, false, true, false)
        assert(not plan.upgradePopper and plan.upgradeKiller)
    end)

    it('skips the popper roll when the popper shares the alliance', function()
        local plan = xi.voidwalker.mobDeathUpgradePlan(true, true, true, false, true, false)
        assert(not plan.upgradePopper and plan.upgradeKiller)
    end)

    it('skips the popper roll when the popper already holds the next abyssite', function()
        local plan = xi.voidwalker.mobDeathUpgradePlan(true, true, false, true, true, false)
        assert(not plan.upgradePopper)
    end)

    it('skips the killer roll without the pop abyssite or with the next one', function()
        assert(not xi.voidwalker.mobDeathUpgradePlan(true, false, false, false, false, false).upgradeKiller)
        assert(not xi.voidwalker.mobDeathUpgradePlan(true, false, false, false, true, true).upgradeKiller)
    end)

    it('reports booleans when the popper lookup returned nil', function()
        local plan = xi.voidwalker.mobDeathUpgradePlan(true, false, false, nil, false, false)
        assert(plan.upgradePopper == false and plan.upgradeKiller == false)
    end)
end)

describe('Voidwalker key-item upgrade plan', function()
    it('requires a same-zone player and the winning roll', function()
        assert(xi.voidwalker.keyItemUpgradePlan(false, 5, xi.keyItem.CLEAR_ABYSSITE, true, xi.keyItem.COLORFUL_ABYSSITE) == nil)
        assert(xi.voidwalker.keyItemUpgradePlan(true, 4, xi.keyItem.CLEAR_ABYSSITE, true, xi.keyItem.COLORFUL_ABYSSITE) == nil)
        assert(xi.voidwalker.keyItemUpgradePlan(true, 6, xi.keyItem.CLEAR_ABYSSITE, true, xi.keyItem.COLORFUL_ABYSSITE) == nil)
        assert(xi.voidwalker.keyItemUpgradePlan(true, 5, xi.keyItem.CLEAR_ABYSSITE, true, xi.keyItem.COLORFUL_ABYSSITE) ~= nil)
    end)

    it('spends the current abyssite and grants the next with its message', function()
        local plan = xi.voidwalker.keyItemUpgradePlan(true, 5, xi.keyItem.CLEAR_ABYSSITE, true, xi.keyItem.COLORFUL_ABYSSITE)

        assert(plan.currentKeyItem == xi.keyItem.CLEAR_ABYSSITE)
        assert(plan.delKeyItem == xi.keyItem.CLEAR_ABYSSITE)
        assert(plan.addKeyItem == xi.keyItem.COLORFUL_ABYSSITE)
        assert(plan.message == 'upgrade_1')
    end)

    it('skips the removal when the player no longer holds the current abyssite', function()
        local plan = xi.voidwalker.keyItemUpgradePlan(true, 5, xi.keyItem.COLORFUL_ABYSSITE, false, xi.keyItem.BLUE_ABYSSITE)

        assert(plan.delKeyItem == nil)
        assert(plan.addKeyItem == xi.keyItem.BLUE_ABYSSITE and plan.message == 'upgrade_2')
    end)

    it('still spends the current abyssite when there is no next one to grant', function()
        local plan = xi.voidwalker.keyItemUpgradePlan(true, 5, xi.keyItem.BLACK_ABYSSITE, true, nil)

        assert(plan.delKeyItem == xi.keyItem.BLACK_ABYSSITE)
        assert(plan.addKeyItem == nil and plan.message == nil)
    end)

    it('leaves the message absent for ordinary mid-tier upgrades', function()
        local plan = xi.voidwalker.keyItemUpgradePlan(true, 5, xi.keyItem.BLUE_ABYSSITE, true, xi.keyItem.ORANGE_ABYSSITE)

        assert(plan.addKeyItem == xi.keyItem.ORANGE_ABYSSITE and plan.message == nil)
    end)

    it('reports the obtain message for the final Black abyssite', function()
        local plan = xi.voidwalker.keyItemUpgradePlan(true, 5, xi.keyItem.GREY_ABYSSITE, true, xi.keyItem.BLACK_ABYSSITE)

        assert(plan.message == 'obtain')
    end)
end)

describe('Voidwalker healing outcome plan', function()
    it('reports the first abyssite when no candidate mob remains', function()
        local plan = xi.voidwalker.healingOutcomePlan(nil, xi.keyItem.BLUE_ABYSSITE, 0, 0)
        assert(plan.kind == 'no_mob' and plan.keyItem == xi.keyItem.BLUE_ABYSSITE)
    end)

    it('consumes later-tier abyssites on spawn and leaves the HP bar hidden', function()
        local nearest = { mobId = 17715201, keyItem = xi.keyItem.BLUE_ABYSSITE, distance = 4 }
        local plan    = xi.voidwalker.healingOutcomePlan(nearest, xi.keyItem.BLUE_ABYSSITE, 0, 0)

        assert(plan.kind == 'spawn')
        assert(plan.mobId == 17715201 and plan.keyItem == xi.keyItem.BLUE_ABYSSITE and plan.distance == 4)
        assert(plan.consume and not plan.showHP)
    end)

    it('retains Clear abyssites on spawn and reveals the HP bar', function()
        local nearest = { mobId = 17715202, keyItem = xi.keyItem.CLEAR_ABYSSITE, distance = 0 }
        local plan    = xi.voidwalker.healingOutcomePlan(nearest, xi.keyItem.CLEAR_ABYSSITE, 0, 0)

        assert(plan.kind == 'spawn' and not plan.consume and plan.showHP)
    end)

    it('reports the too-far message at and beyond the distance limit', function()
        local nearest = { mobId = 17715203, keyItem = xi.keyItem.BLACK_ABYSSITE, distance = 300 }
        local plan    = xi.voidwalker.healingOutcomePlan(nearest, xi.keyItem.CLEAR_ABYSSITE, 0, 0)

        assert(plan.kind == 'too_far')
        assert(plan.keyItem == xi.keyItem.BLACK_ABYSSITE and plan.distance == 300)
    end)

    it('hints with the abyssite resonance tier and player-to-mob direction', function()
        local nearest = { mobId = 17715204, keyItem = xi.keyItem.BLACK_ABYSSITE, distance = 50 }
        local plan    = xi.voidwalker.healingOutcomePlan(nearest, xi.keyItem.CLEAR_ABYSSITE, -1, 0)

        assert(plan.kind == 'hint')
        assert(plan.keyItem == xi.keyItem.BLACK_ABYSSITE and plan.distance == 50)
        assert(plan.tier == 3 and plan.direction == 4)
    end)

    it('leaves the hint tier absent for Purple abyssites', function()
        local nearest = { mobId = 17715205, keyItem = xi.keyItem.PURPLE_ABYSSITE, distance = 50 }
        local plan    = xi.voidwalker.healingOutcomePlan(nearest, xi.keyItem.CLEAR_ABYSSITE, 1, 0)

        assert(plan.kind == 'hint' and plan.tier == nil and plan.direction == 0)
    end)
end)

describe('Voidwalker direction', function()
    it('classifies cardinal and diagonal offsets', function()
        assert(xi.voidwalker.direction(1, 0) == 0)
        assert(xi.voidwalker.direction(0, -1) == 2)
        assert(xi.voidwalker.direction(-1, 0) == 4)
        assert(xi.voidwalker.direction(0, 1) == 6)

        assert(xi.voidwalker.direction(1, -1) == 1)
        assert(xi.voidwalker.direction(-1, -1) == 3)
        assert(xi.voidwalker.direction(-1, 1) == 5)
        assert(xi.voidwalker.direction(1, 1) == 7)
    end)

    it('uses 20 and 70 degrees as the horizontal and vertical cutoffs', function()
        assert(xi.voidwalker.direction(1, math.tan(math.rad(19))) == 0)
        assert(xi.voidwalker.direction(1, math.tan(math.rad(20))) == 0)
        assert(xi.voidwalker.direction(1, math.tan(math.rad(21))) == 7)
        assert(xi.voidwalker.direction(1, math.tan(math.rad(69))) == 7)
        assert(xi.voidwalker.direction(1, math.tan(math.rad(70))) == 6)
        assert(xi.voidwalker.direction(1, math.tan(math.rad(71))) == 6)
    end)

    it('retains the northeast fallback for coincident positions', function()
        assert(xi.voidwalker.direction(0, 0) == 7)
    end)
end)
