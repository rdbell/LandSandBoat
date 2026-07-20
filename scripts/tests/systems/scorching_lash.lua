require('scripts/actions/mobskills/scorching_lash')
describe('Scorching Lash mob skill', function()
    it('requires behind target and uses threefold slashing plan', function()
        local lash = require('scripts/actions/mobskills/scorching_lash')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, behind = nil, nil, false
        local mob = { getWeaponDmg = function() return 80 end }
        local target = {
            isBehind = function(_, m, angle) assert(angle == 48); return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        behind = false; assert(lash.onMobSkillCheck(target, mob, {}) == 1)
        behind = true; assert(lash.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 120, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(lash.onMobWeaponSkill(mob, target, {}, {}) == 120)
        assert(params.numHits == 3 and params.fTP[1] == 1.3 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_4 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        lash.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 120)
    end)
end)
