require('scripts/actions/mobskills/di_trample')

describe('Di Trample mob skill', function()
    it('uses its ranged physical plan and applies sampled MP reduction after processing', function()
        local params, damage, mp = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = {
            getMP = function() return 999 end,
            setMP = function(_, value) mp = value end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local random = math.random
        math.random = function(minimum, maximum) assert(minimum == 10 and maximum == 30); return 25 end
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end

        local trample = require('scripts/actions/mobskills/di_trample')
        assert(trample.onMobSkillCheck({}, {}, {}) == 0)
        assert(trample.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and params.primaryMessage == xi.msg.basic.HIT_DMG)
        assert(damage == nil and mp == nil)

        xi.mobskills.processDamage = function() return true end
        assert(trample.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        math.random = random
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.RANGED and damage[4] == xi.damageType.PIERCING)
        assert(mp == 749.25)
    end)
end)
