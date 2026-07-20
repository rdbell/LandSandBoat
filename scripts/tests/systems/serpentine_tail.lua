require('scripts/actions/mobskills/serpentine_tail')
describe('Serpentine Tail mob skill', function()
    it('requires behind target and uses blunt physical plan', function()
        local tail = require('scripts/actions/mobskills/serpentine_tail')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, behind = nil, nil, false
        local mob = { getWeaponDmg = function() return 80 end }
        local target = {
            isBehind = function(_, m, angle) assert(angle == 48); return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        behind = false; assert(tail.onMobSkillCheck(target, mob, {}) == 1)
        behind = true; assert(tail.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(tail.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.fTP[1] == 4.25 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        tail.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 200)
    end)
end)
