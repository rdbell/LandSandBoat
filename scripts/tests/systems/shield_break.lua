require('scripts/actions/mobskills/shield_break')
describe('Shield Break mob skill', function()
    it('uses slashing physical plan and applies TP-scaled Evasion Down', function()
        local brk = require('scripts/actions/mobskills/shield_break')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1000 end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 60, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(brk.onMobSkillCheck(target, mob, skill) == 0 and brk.onMobWeaponSkill(mob, target, skill, {}) == 60)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        brk.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 60 and statusParams[3] == xi.effect.EVASION_DOWN and statusParams[4] == 40 and statusParams[6] == 180)
    end)
end)
