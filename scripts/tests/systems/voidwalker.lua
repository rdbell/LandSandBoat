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
