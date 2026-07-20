require('scripts/actions/mobskills/meteor')

describe('Meteor mob skill', function()
    it('uses its non-elemental magical plan, damages only after processing, then finalizes death', function()
        local meteor = require('scripts/actions/mobskills/meteor')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, animSub, timerMs, unkillable, finalized = nil, nil, nil, nil, nil, nil
        local mob = {
            getMainLvl = function() return 75 end,
            setAnimationSub = function(_, value) animSub = value end,
            timer = function(self, ms, cb)
                timerMs = ms
                cb(self)
            end,
            setUnkillable = function(_, value) unkillable = value end,
            setHP = function(_, value) finalized = value end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end

        assert(meteor.onMobSkillCheck(target, mob, skill) == 0 and meteor.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 75 and params.fTP[1] == 32 and params.fTP[2] == 32 and params.fTP[3] == 32)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        meteor.onMobWeaponSkill(mob, target, skill, {})
        meteor.onMobSkillFinalize(mob, skill)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.NONE)
        assert(animSub == 1 and timerMs == 6000 and unkillable == false and finalized == 0)
    end)
end)
