require('scripts/actions/mobskills/shadow_burst')
describe('Shadow Burst mob skill', function()
    it('requires anim sub 0 and applies Curse after processing', function()
        local burst = require('scripts/actions/mobskills/shadow_burst')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, anim = nil, nil, nil, 1
        local mob = {
            getAnimationSub = function() return anim end,
            getMainLvl = function() return 40 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(burst.onMobSkillCheck(target, mob, {}) == 1)
        anim = 0; assert(burst.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(burst.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 42 and params.fTP[1] == 2 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        burst.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.CURSE_I and statusParams[4] == 50 and statusParams[6] == 300)
    end)
end)
