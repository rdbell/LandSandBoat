-----------------------------------
-- Pure system tests for enhancing ninjutsu dual-wire (slice 6724).
-- Calls production xi.spells.enhancing pure exports.
-- Goldens match internal/enhancingninjutsu (0879 / 6116).
-----------------------------------

require('scripts/globals/spells/enhancing_ninjutsu')

local e = xi.spells.enhancing

describe('Enhancing ninjutsu pins', function()
    it('constants and catalog size', function()
        assert(e.ninjutsuMigawariSubPower == 100)
        assert(e.ninjutsuUtsusemiSubPowerThreshold == 3)
        assert(e.ninjutsuUtsusemiDurationSeconds == 900)
        assert(e.ninjutsuSneakInvisibleTick == 10)
        assert(e.ninjutsuMsgMagicNoEffect == 75)
        assert(e.ninjutsuMsgMagicGainEffect == 230)
        assert(e.ninjutsuSpellCatalogSize() == 11)
    end)
end)

describe('lookupNinjutsuSpell', function()
    it('returns known catalog rows', function()
        local r = e.lookupNinjutsuSpell(xi.magic.spell.GEKKA_ICHI)
        assert(r and r.tier == 1 and r.effect == xi.effect.ENMITY_BOOST and r.power == 30 and r.duration == 300 and r.alwaysOverwrite)

        r = e.lookupNinjutsuSpell(xi.magic.spell.MIGAWARI_ICHI)
        assert(r and r.effect == xi.effect.MIGAWARI and r.power == 0 and r.duration == 60 and r.alwaysOverwrite)

        r = e.lookupNinjutsuSpell(xi.magic.spell.UTSUSEMI_ICHI)
        assert(r and r.effect == xi.effect.COPY_IMAGE and r.power == 3 and r.duration == 0 and not r.alwaysOverwrite)

        r = e.lookupNinjutsuSpell(xi.magic.spell.UTSUSEMI_NI)
        assert(r and r.power == 4)

        r = e.lookupNinjutsuSpell(xi.magic.spell.UTSUSEMI_SAN)
        assert(r and r.power == 5)

        r = e.lookupNinjutsuSpell(xi.magic.spell.TONKO_NI)
        assert(r and r.tier == 2 and r.effect == xi.effect.INVISIBLE and r.duration == 600)

        r = e.lookupNinjutsuSpell(xi.magic.spell.YAIN_ICHI)
        assert(r and r.effect == xi.effect.PAX and r.power == 15)
    end)

    it('returns nil for unknown', function()
        assert(e.lookupNinjutsuSpell(0) == nil)
        assert(e.lookupNinjutsuSpell(1) == nil)
    end)
end)

describe('calculateNinjutsuPowerFromParams', function()
    it('passthrough table power', function()
        local power, sub = e.calculateNinjutsuPowerFromParams({
            spellId = xi.magic.spell.GEKKA_ICHI,
            spellEffect = xi.effect.ENMITY_BOOST,
            tablePower = 30,
        })
        assert(power == 30 and sub == 0)

        power, sub = e.calculateNinjutsuPowerFromParams({
            spellId = xi.magic.spell.YAIN_ICHI,
            spellEffect = xi.effect.PAX,
            tablePower = 15,
        })
        assert(power == 15 and sub == 0)
    end)

    it('Migawari skill/5', function()
        local power, sub = e.calculateNinjutsuPowerFromParams({
            spellId = xi.magic.spell.MIGAWARI_ICHI,
            spellEffect = xi.effect.MIGAWARI,
            tablePower = 0,
            ninjutsuSkill = 250,
        })
        assert(power == 50 and sub == 100)

        power, sub = e.calculateNinjutsuPowerFromParams({
            spellEffect = xi.effect.MIGAWARI, ninjutsuSkill = 255,
        })
        assert(power == 51 and sub == 100)

        power, sub = e.calculateNinjutsuPowerFromParams({
            spellEffect = xi.effect.MIGAWARI, ninjutsuSkill = 0,
        })
        assert(power == 0 and sub == 100)
    end)

    it('Utsusemi gear and Ni non-NIN penalty', function()
        -- Ichi bare → power 3, COPY_IMAGE_3
        local power, sub = e.calculateNinjutsuPowerFromParams({
            spellId = xi.magic.spell.UTSUSEMI_ICHI,
            spellEffect = xi.effect.COPY_IMAGE,
            tablePower = 3,
            mainJob = xi.job.NIN,
        })
        assert(power == 3 and sub == xi.effect.COPY_IMAGE_3)

        -- Ichi +1 → power 4 → COPY_IMAGE_4
        power, sub = e.calculateNinjutsuPowerFromParams({
            spellId = xi.magic.spell.UTSUSEMI_ICHI,
            spellEffect = xi.effect.COPY_IMAGE,
            tablePower = 3,
            utsusemiBonus = 1,
            mainJob = xi.job.NIN,
        })
        assert(power == 4 and sub == xi.effect.COPY_IMAGE_4)

        -- Ni NIN base 4 → COPY_IMAGE_4
        power, sub = e.calculateNinjutsuPowerFromParams({
            spellId = xi.magic.spell.UTSUSEMI_NI,
            spellEffect = xi.effect.COPY_IMAGE,
            tablePower = 4,
            mainJob = xi.job.NIN,
        })
        assert(power == 4 and sub == xi.effect.COPY_IMAGE_4)

        -- Ni non-NIN → power 3, COPY_IMAGE_3
        power, sub = e.calculateNinjutsuPowerFromParams({
            spellId = xi.magic.spell.UTSUSEMI_NI,
            spellEffect = xi.effect.COPY_IMAGE,
            tablePower = 4,
            mainJob = xi.job.WAR,
        })
        assert(power == 3 and sub == xi.effect.COPY_IMAGE_3)

        -- San base 5 → COPY_IMAGE_4
        power, sub = e.calculateNinjutsuPowerFromParams({
            spellId = xi.magic.spell.UTSUSEMI_SAN,
            spellEffect = xi.effect.COPY_IMAGE,
            tablePower = 5,
            mainJob = xi.job.NIN,
        })
        assert(power == 5 and sub == xi.effect.COPY_IMAGE_4)
    end)
end)

describe('useEnhancingNinjutsuFromParams', function()
    it('alwaysOverwrite Gekka del Pax', function()
        local plan = e.useEnhancingNinjutsuFromParams({
            spellEffect = xi.effect.ENMITY_BOOST,
            duration = 300,
            alwaysOverwrite = true,
            tablePower = 30,
            power = 30,
            subPower = 0,
        })
        assert(plan.delPax and plan.delSpellEffect and plan.applyEffect)
        assert(not plan.setMsg)
        assert(plan.power == 30 and plan.duration == 300)
    end)

    it('Yain del EnmityBoost', function()
        local plan = e.useEnhancingNinjutsuFromParams({
            spellEffect = xi.effect.PAX,
            duration = 300,
            alwaysOverwrite = true,
            power = 15,
        })
        assert(plan.delEnmityBoost and plan.applyEffect)
    end)

    it('Sneak tick 10 default add', function()
        local plan = e.useEnhancingNinjutsuFromParams({
            spellEffect = xi.effect.SNEAK,
            duration = 420,
            alwaysOverwrite = false,
            power = 0,
            addStatusOK = true,
        })
        assert(plan.tick == 10 and plan.applyEffect and plan.msg == 230 and plan.setMsg)

        plan = e.useEnhancingNinjutsuFromParams({
            spellEffect = xi.effect.SNEAK,
            duration = 420,
            alwaysOverwrite = false,
            addStatusOK = false,
        })
        assert(not plan.applyEffect and plan.msg == 75)
    end)

    it('Utsusemi apply and overwrite gate', function()
        -- Fresh Utsusemi Ichi: paramThree = 3-2 = 1, dur 900, swapped power/sub
        local plan = e.useEnhancingNinjutsuFromParams({
            spellEffect = xi.effect.COPY_IMAGE,
            duration = 0,
            alwaysOverwrite = false,
            tablePower = 3,
            power = 3,
            subPower = xi.effect.COPY_IMAGE_3,
            hasCopyImage = false,
            hasThirdEye = true,
        })
        assert(plan.delThirdEye)
        assert(plan.applyEffect and plan.msg == 230)
        assert(plan.power == 1 and plan.duration == 900 and plan.tick == 0)
        assert(plan.icon == xi.effect.COPY_IMAGE_3 and plan.subPower == 3)

        -- Stronger existing blocks
        plan = e.useEnhancingNinjutsuFromParams({
            spellEffect = xi.effect.COPY_IMAGE,
            duration = 0,
            alwaysOverwrite = false,
            tablePower = 3,
            power = 3,
            subPower = xi.effect.COPY_IMAGE_3,
            hasCopyImage = true,
            existingCopyImagePower = 2,
        })
        assert(not plan.applyEffect and plan.msg == 75)

        -- Equal existing allows (existingPower <= paramThree)
        plan = e.useEnhancingNinjutsuFromParams({
            spellEffect = xi.effect.COPY_IMAGE,
            duration = 0,
            alwaysOverwrite = false,
            tablePower = 3,
            power = 3,
            subPower = xi.effect.COPY_IMAGE_3,
            hasCopyImage = true,
            existingCopyImagePower = 1,
        })
        assert(plan.applyEffect and plan.msg == 230)
    end)
end)
