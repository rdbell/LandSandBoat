require('scripts/actions/mobskills/empty_thrash')

describe('Empty Thrash mob skill', function()
    it('rejects notorious mobs and uses its physical plan otherwise', function()
        local thrash = require('scripts/actions/mobskills/empty_thrash')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local normalMob = { isMobType = function() return false end, getWeaponDmg = function() return 77 end }
        local notoriousMob = { isMobType = function(_, kind) assert(kind == xi.mobType.NOTORIOUS); return true end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end

        assert(thrash.onMobSkillCheck({}, notoriousMob, {}) == 1)
        assert(thrash.onMobSkillCheck({}, normalMob, {}) == 0)
        assert(thrash.onMobWeaponSkill(normalMob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(thrash.onMobWeaponSkill(normalMob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == normalMob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
    end)
end)
