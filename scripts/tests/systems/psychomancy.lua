require('scripts/actions/mobskills/psychomancy')

describe('Psychomancy mob skill', function()
    it('requires staff form and drains MP after processing', function()
        local psycho = require('scripts/actions/mobskills/psychomancy')
        local move, process, drain = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local animSub, params, drainArgs, message = 0, nil, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getMainLvl = function() return 50 end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        animSub = 0
        assert(psycho.onMobSkillCheck({}, mob, skill) == 1)
        animSub = 3
        assert(psycho.onMobSkillCheck({}, mob, skill) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobDrainMove = function(...)
            drainArgs = { ... }
            return 456
        end
        xi.mobskills.processDamage = function() return false end
        assert(psycho.onMobWeaponSkill(mob, {}, skill, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.7 and params.skipDamageAdjustment)
        assert(params.skipMagicBonusDiff and params.skipStoneSkin and drainArgs == nil and message == nil)
        xi.mobskills.processDamage = function() return true end
        psycho.onMobWeaponSkill(mob, {}, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drain
        assert(drainArgs[3] == xi.mobskills.drainType.MP and drainArgs[4] == 123 and message == 456)
    end)
end)
