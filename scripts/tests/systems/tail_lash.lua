require('scripts/actions/mobskills/tail_lash')
describe('Tail Lash mob skill', function()
    it('requires behind target and applies amnesia after processing', function()
        local skill = require('scripts/actions/mobskills/tail_lash')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, behind = nil, nil, nil, false
        local mob = { getWeaponDmg = function() return 80 end }
        local target = {
            isBehind = function(_, m, angle) assert(angle == 48); return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        behind = false; assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        behind = true; assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 3.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.AMNESIA and statusParams[6] == 60)
    end)
end)
