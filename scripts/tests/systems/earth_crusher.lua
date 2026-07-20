require('scripts/actions/mobskills/earth_crusher')

describe('Earth Crusher mob skill', function()
    it('uses TP-scaled Earth magical parameters and damages only after processing succeeds', function()
        local earthCrusher = require('scripts/actions/mobskills/earth_crusher')
        local originalMagicalMove = xi.mobskills.mobMagicalMove
        local originalProcessDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function() return false end

        assert(earthCrusher.onMobSkillCheck(target, mob, skill) == 0)
        assert(earthCrusher.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 1 and params.fTP[2] == 2.3125 and params.fTP[3] == 3.625)
        assert(params.element == xi.element.EARTH and params.attackType == xi.attackType.MAGICAL)
        assert(params.damageType == xi.damageType.EARTH and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(params.dStatMultiplier == 1 and params.dStatAttackerMod == xi.mod.INT and params.dStatDefenderMod == xi.mod.INT)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(earthCrusher.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobMagicalMove = originalMagicalMove
        xi.mobskills.processDamage = originalProcessDamage

        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.EARTH)
    end)
end)
