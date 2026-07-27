require('scripts/globals/job_utils/monk')

describe('Monk Chi Blast', function()
    it('applies Penance, uses and consumes Boost, and damages the target', function()
        local originalAdjustDamage = xi.ability.adjustDamage
        local originalRandom = math.random
        xi.ability.adjustDamage = function(damage, player, ability, target, attackType, damageType, shadows)
            assert(attackType == xi.attackType.BREATH and damageType == xi.damageType.ELEMENTAL and shadows == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
            return damage + 5
        end
        math.random = function()
            return 0
        end

        local removedBoost
        local penance
        local damaged
        local claimed
        local boost = { getPower = function() return 25 end }
        local player = {
            getMerit = function(_, merit)
                assert(merit == xi.merit.PENANCE)
                return 40
            end,
            getStatusEffect = function(_, effect)
                assert(effect == xi.effect.BOOST)
                return boost
            end,
            getStat = function(_, mod)
                assert(mod == xi.mod.MND)
                return 100
            end,
            delStatusEffect = function(_, effect)
                removedBoost = effect
            end,
        }
        local target = {
            delStatusEffectSilent = function(_, effect)
                assert(effect == xi.effect.INHIBIT_TP)
            end,
            addStatusEffect = function(_, effect, values)
                penance = { effect = effect, values = values }
            end,
            takeDamage = function(_, damage, source, attackType, damageType)
                damaged = { damage = damage, source = source, attackType = attackType, damageType = damageType }
            end,
            updateClaim = function(_, source)
                claimed = source
            end,
        }

        local damage = xi.job_utils.monk.useChiBlast(player, target, {})
        xi.ability.adjustDamage = originalAdjustDamage
        math.random = originalRandom

        assert(damage == 55 and damaged.damage == 55 and damaged.source == player)
        assert(damaged.attackType == xi.attackType.BREATH and damaged.damageType == xi.damageType.ELEMENTAL and claimed == player)
        assert(removedBoost == xi.effect.BOOST)
        assert(penance.effect == xi.effect.INHIBIT_TP and penance.values.power == 25 and penance.values.duration == 40 and penance.values.origin == player)
    end)
end)
