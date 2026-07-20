require('scripts/actions/mobskills/tebbad_wing')
describe('Tebbad Wing mob skill', function()
    it('multi-gates admission and applies fire magical + plague', function()
        local skill = require('scripts/actions/mobskills/tebbad_wing')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, behind = nil, nil, nil, false
        local mob = {
            hasStatusEffect = function(_, e) return false end,
            getAnimationSub = function() return 0 end,
            getMainLvl = function() return 50 end,
        }
        local target = {
            isBehind = function(_, m, angle) assert(angle == 48); return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        behind = true; assert(skill.onMobSkillCheck(target, mob, {}) == 1); behind = false
        mob.getAnimationSub = function() return 1 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getAnimationSub = function() return 0 end
        mob.hasStatusEffect = function(_, e) return e == xi.effect.MIGHTY_STRIKES end
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.hasStatusEffect = function() return false end
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 52 and params.fTP[1] == 4.00 and params.element == xi.element.FIRE and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.PLAGUE and statusParams[4] == 10 and statusParams[6] == 120)
    end)
end)
