require('scripts/actions/mobskills/auroral_wind')
describe('Auroral Wind mob skill', function()
    it('uses Light plan with random level mult and processed Silence', function()
        local skill = require('scripts/actions/mobskills/auroral_wind')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local origRandom = math.random
        local params, damage, effect = nil, nil, nil
        math.random = function(a, b)
            assert(a == 2 and b == 3)
            return 3
        end
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.baseDamage == 150 and params.fTP[1] == 1.0 and params.element == xi.element.LIGHT)
        assert(damage == nil and effect == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        math.random = origRandom
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 90 and effect[1] == xi.effect.SILENCE and effect[4] == 120)
    end)
end)
