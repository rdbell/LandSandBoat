require('scripts/actions/mobskills/tempest_wing')
describe('Tempest Wing mob skill', function()
    it('rejects behind targets and applies wind magical + blindness', function()
        local skill = require('scripts/actions/mobskills/tempest_wing')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, behind = nil, nil, nil, true
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            isBehind = function(_, m, angle) assert(angle == 55); return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        behind = false; assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 50 and params.fTP[1] == 4.75 and params.element == xi.element.WIND and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.BLINDNESS and statusParams[4] == 50 and statusParams[6] == 60)
    end)
end)
