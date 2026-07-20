require('scripts/actions/mobskills/typhoon_wing')
describe('Typhoon Wing mob skill', function()
    it('gates behind/flying and applies wind magical + blindness', function()
        local skill = require('scripts/actions/mobskills/typhoon_wing')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, behind = nil, nil, nil, false
        local mob = { getAnimationSub = function() return 0 end, getMainLvl = function() return 50 end }
        local target = {
            isBehind = function(_, m, a) assert(a == 48); return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        behind = true; assert(skill.onMobSkillCheck(target, mob, {}) == 1); behind = false
        mob.getAnimationSub = function() return 1 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getAnimationSub = function() return 0 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 4.50 and params.fTP[3] == 5.50 and params.element == xi.element.WIND and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.BLINDNESS and statusParams[4] == 60 and statusParams[6] == 30)
    end)
end)
