require('scripts/globals/mobs')

describe('Mob additional effect catalog', function()
    it('exposes a definition for every xi.mob.ae value', function()
        for name, effect in pairs(xi.mob.ae) do
            assert(xi.mob.additionalEffectData(effect) ~= nil, name)
        end
    end)

    it('has no definition for an unknown effect', function()
        assert(xi.mob.additionalEffectData(9999) == nil)
    end)

    it('defines exactly sixteen effects', function()
        local count = 0
        for _ in pairs(xi.mob.ae) do
            count = count + 1
        end

        assert(count == 16, count)
    end)

    it('gives every status entry an effect and every immediate entry a mod', function()
        for name, effect in pairs(xi.mob.ae) do
            local ae = xi.mob.additionalEffectData(effect)

            if ae.applyEffect then
                assert(ae.eff ~= nil, name)
                assert(ae.msg == xi.msg.basic.ADD_EFFECT_STATUS, name)
            else
                assert(ae.mod ~= nil, name)
                assert(ae.code ~= nil, name)
            end

            assert(ae.chance ~= nil and ae.sub ~= nil, name)
            assert(ae.negMsg == nil, name)
        end
    end)

    it('describes the HP drain immediate effect', function()
        local ae = xi.mob.additionalEffectData(xi.mob.ae.HP_DRAIN)

        assert(ae.chance == 10)
        assert(ae.ele == xi.element.DARK)
        assert(ae.sub == xi.subEffect.HP_DRAIN)
        assert(ae.mod == xi.mod.INT)
        assert(not ae.applyEffect)
        assert(ae.code ~= nil)
    end)

    it('describes the poison status effect with its tick', function()
        local ae = xi.mob.additionalEffectData(xi.mob.ae.POISON)

        assert(ae.chance == 25)
        assert(ae.applyEffect)
        assert(ae.eff == xi.effect.POISON)
        assert(ae.power == 1 and ae.duration == 30)
        assert(ae.minDuration == 1 and ae.maxDuration == 30)
        assert(ae.tick == 3)
    end)

    it('leaves stun and terror without duration bounds', function()
        for _, effect in ipairs({ xi.mob.ae.STUN, xi.mob.ae.TERROR }) do
            local ae = xi.mob.additionalEffectData(effect)

            assert(ae.duration == 5)
            assert(ae.minDuration == nil and ae.maxDuration == nil)
        end
    end)
end)

describe('Mob additional effect proc chance', function()
    it('prefers the params chance, then the effect chance, then 100', function()
        assert(xi.mob.addEffectProcChance(40, 25, 0) == 40)
        assert(xi.mob.addEffectProcChance(nil, 25, 0) == 25)
        assert(xi.mob.addEffectProcChance(nil, nil, 0) == 100)
    end)

    it('leaves the chance alone for a target at or below the mob level', function()
        assert(xi.mob.addEffectProcChance(nil, 25, 0) == 25)
        assert(xi.mob.addEffectProcChance(nil, 25, -10) == 25)
    end)

    it('loses five points per level the target is above the mob', function()
        assert(xi.mob.addEffectProcChance(nil, 50, 1) == 45)
        assert(xi.mob.addEffectProcChance(nil, 50, 4) == 30)
    end)

    it('clamps the penalised chance to 5..95', function()
        assert(xi.mob.addEffectProcChance(nil, 50, 20) == 5)
        assert(xi.mob.addEffectProcChance(nil, 100, 1) == 95)
    end)
end)

describe('Mob additional effect status application', function()
    it('requires a weak resist and an unafflicted target', function()
        assert(xi.mob.addEffectStatusApplies(1, false))
        assert(xi.mob.addEffectStatusApplies(0.51, false))

        assert(not xi.mob.addEffectStatusApplies(0.5, false))
        assert(not xi.mob.addEffectStatusApplies(0.25, false))
        assert(not xi.mob.addEffectStatusApplies(1, true))
    end)

    it('clamps the duration then scales it by the resist', function()
        assert(xi.mob.addEffectStatusDuration(nil, 30, 1, 30, 1) == 30)
        assert(xi.mob.addEffectStatusDuration(nil, 30, 1, 45, 0.5) == 15)
        assert(xi.mob.addEffectStatusDuration(nil, 90, 1, 45, 1) == 45)
        assert(xi.mob.addEffectStatusDuration(nil, 0, 1, 45, 1) == 1)
    end)

    it('prefers the params duration over the effect duration', function()
        assert(xi.mob.addEffectStatusDuration(20, 30, 1, 45, 1) == 20)
    end)

    it('leaves an unbounded duration unclamped', function()
        assert(xi.mob.addEffectStatusDuration(nil, 5, nil, nil, 1) == 5)
        assert(xi.mob.addEffectStatusDuration(nil, 5, nil, nil, 0.6) == 3)
    end)
end)

describe('Mob additional effect immediate power', function()
    it('uses the raw stat delta up to twenty', function()
        assert(xi.mob.addEffectDMod(20, 0) == 20)
        assert(xi.mob.addEffectDMod(10, 0) == 10)
    end)

    it('halves the stat delta past twenty', function()
        assert(xi.mob.addEffectDMod(40, 0) == 30)
        assert(xi.mob.addEffectDMod(120, 0) == 70)
    end)

    it('floors a negative stat delta at zero', function()
        assert(xi.mob.addEffectDMod(0, 10) == 0)
        assert(xi.mob.addEffectDMod(0, 100) == 0)
    end)

    it('adds the level gap and half the triggering damage', function()
        assert(xi.mob.addEffectBasePower(20, 75, 75, 100) == 70)
        assert(xi.mob.addEffectBasePower(0, 80, 75, 0) == 5)
        assert(xi.mob.addEffectBasePower(0, 70, 75, 0) == -5)
    end)
end)

describe('Mob additional effect negative power', function()
    it('keeps a positive power and its message', function()
        local power, message = xi.mob.addEffectNegativeAdjust(50, 100, 200)
        assert(power == 50 and message == 100)
    end)

    it('flips a negative power onto the healing message', function()
        local power, message = xi.mob.addEffectNegativeAdjust(-50, 100, 200)
        assert(power == 50 and message == 200)
    end)

    it('drops a negative power without a healing message', function()
        local power, message = xi.mob.addEffectNegativeAdjust(-50, 100, nil)
        assert(power == 0 and message == 100)
    end)

    it('leaves a zero power alone', function()
        local power, message = xi.mob.addEffectNegativeAdjust(0, 100, 200)
        assert(power == 0 and message == 100)
    end)
end)
