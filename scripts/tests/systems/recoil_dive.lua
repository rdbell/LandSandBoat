require('scripts/actions/mobskills/recoil_dive')
describe('Recoil Dive mob skill', function()
    it('uses its slashing physical plan with crit and damages only after processing', function()
        local dive = require('scripts/actions/mobskills/recoil_dive')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(dive.onMobSkillCheck(target, mob, {}) == 0 and dive.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 2 and params.canCrit and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        dive.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
