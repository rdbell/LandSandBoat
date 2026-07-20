require('scripts/actions/mobskills/catastrophe')
describe('Catastrophe mob skill', function()
    it('uses physical plan and drains HP when processed', function()
        local skill = require('scripts/actions/mobskills/catastrophe')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, healed = nil, nil, nil
        local origRandom = math.random
        math.random = function(a, b)
            if a == 30 and b == 70 then return 50 end
            return origRandom(a, b)
        end
        local mob = {
            getWeaponDmg = function() return 50 end,
            addHP = function(_, v) healed = v end,
        }
        local target = {
            getHP = function() return 200 end,
            isUndead = function() return false end,
            takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 2.75 and healed == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage[1] == 100 and healed == 50)
        math.random = origRandom
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
