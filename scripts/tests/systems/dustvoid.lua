require('scripts/actions/mobskills/dustvoid')

describe('Dustvoid mob skill', function()
    it('uses its Wind magical plan and unequips only processed PC targets', function()
        local dustvoid = require('scripts/actions/mobskills/dustvoid')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, unequipped = nil, nil, {}
        local mob = { getMainLvl = function() return 75 end }
        local target = { isPC = function() return true end, takeDamage = function(_, ...) damage = { ... } end, unequipItem = function(_, slot) table.insert(unequipped, slot) end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND } end
        xi.mobskills.processDamage = function() return false end
        assert(dustvoid.onMobSkillCheck({}, {}, {}) == 0)
        assert(dustvoid.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 0.5 and params.fTP[2] == 0.5 and params.fTP[3] == 0.5)
        assert(params.element == xi.element.WIND and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.WIND and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 1)
        assert(damage == nil and #unequipped == 0)
        xi.mobskills.processDamage = function() return true end
        assert(dustvoid.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.WIND)
        assert(#unequipped == xi.slot.BACK - xi.slot.MAIN + 1 and unequipped[1] == xi.slot.MAIN and unequipped[#unequipped] == xi.slot.BACK)
        target.isPC = function() return false end
        unequipped = {}
        assert(dustvoid.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
        assert(#unequipped == 0)
    end)
end)
