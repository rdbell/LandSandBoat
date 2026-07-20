require('scripts/actions/mobskills/lightning_roar')

describe('Lightning Roar mob skill', function()
    it('uses a Thunder magical plan and damages only after processing succeeds', function()
        local roar = require('scripts/actions/mobskills/lightning_roar')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.processDamage = function() return false end

        assert(roar.onMobSkillCheck(target, mob, {}) == 0)
        assert(roar.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.5 and params.fTP[2] == 1.5 and params.fTP[3] == 1.5)
        assert(params.element == xi.element.THUNDER and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.THUNDER)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(roar.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.THUNDER)
    end)
end)
