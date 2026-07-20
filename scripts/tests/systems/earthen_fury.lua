require('scripts/actions/mobskills/earthen_fury')

describe('Earthen Fury mob skill', function()
    it('uses Titan Earth magical parameters and damages only after processing succeeds', function()
        local fury = require('scripts/actions/mobskills/earthen_fury')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function() return false end

        assert(fury.onMobSkillCheck(target, mob, skill) == 0)
        assert(fury.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 9 and params.fTP[2] == 9 and params.fTP[3] == 9)
        assert(params.element == xi.element.EARTH and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.EARTH)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 2 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(fury.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.EARTH)
    end)
end)
