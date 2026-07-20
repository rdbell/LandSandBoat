require('scripts/actions/mobskills/sulfurous_breath')
describe('Sulfurous Breath mob skill', function()
    it('rejects behind targets and uses fire breath plan', function()
        local skill = require('scripts/actions/mobskills/sulfurous_breath')
        local move, process = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local params, damage, behind = nil, nil, false
        local mob = {}
        local target = {
            isBehind = function(_, m, angle) assert(angle == 48); return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        behind = true; assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        behind = false; assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.BREATH, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.percentMultipier == 0.20 and params.damageCap == 700 and params.element == xi.element.FIRE and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 200)
    end)
end)
