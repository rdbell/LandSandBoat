require('scripts/actions/mobskills/demonic_flower')

describe('Demonic Flower mob skill', function()
    it('applies Weakness, damages the caster from its HP, and deals half that damage to the target', function()
        local selfDamage, targetDamage, weakness, message = nil, nil, nil, nil
        local mob = {
            getHP = function() return 1000 end,
            takeDamage = function(_, damage) selfDamage = damage end,
        }
        local target = { takeDamage = function(_, ...) targetDamage = { ... } end }
        local skill = { setMsg = function(_, value) message = value end }
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(...) weakness = { ... }; return 242 end

        local flower = require('scripts/actions/mobskills/demonic_flower')
        assert(flower.onMobSkillCheck({}, {}, {}) == 0)
        assert(flower.onMobWeaponSkill(mob, target, skill, {}) == 120)
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(weakness[3] == xi.effect.WEAKNESS and weakness[4] == 1 and weakness[5] == 0 and weakness[6] == 90)
        assert(message == 242 and selfDamage == 240)
        assert(targetDamage[1] == 120 and targetDamage[2] == mob and targetDamage[3] == xi.attackType.MAGICAL and targetDamage[4] == xi.damageType.ELEMENTAL)
    end)
end)
