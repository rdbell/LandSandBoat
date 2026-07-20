require('scripts/actions/mobskills/touchdown')
describe('Touchdown mob skill', function()
    it('always fails skill check, uses none magical plan, and finalizes landing', function()
        local skill = require('scripts/actions/mobskills/touchdown')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, delEffect, skillAtk, anim = nil, nil, nil, nil, nil
        local mob = {
            getMainLvl = function() return 50 end,
            delStatusEffect = function(_, e) delEffect = e end,
            setMobSkillAttack = function(_, v) skillAtk = v end,
            setAnimationSub = function(_, v) anim = v end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 1.50 and params.element == xi.element.NONE and damage == nil)
        skill.onMobSkillFinalize(mob, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(delEffect == xi.effect.ALL_MISS and skillAtk == 0 and anim == 2)
    end)
end)
