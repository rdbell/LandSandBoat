require('scripts/actions/mobskills/bomb_toss')

describe('Bomb Toss mob skill', function()
    it('uses Fire magical parameters and damages only after processing succeeds', function()
        local bombToss = require('scripts/actions/mobskills/bomb_toss')
        local oldMove, oldProcess = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(bombToss.onMobSkillCheck(target, mob, skill) == 0)
        assert(bombToss.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(bombToss.onMobWeaponSkill(mob, target, skill, action) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = oldMove, oldProcess
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
    end)
end)
