require('scripts/actions/mobskills/water_meeble_warble')
describe('Water Meeble Warble mob skill', function()
    it('uses fTP 18 water plan and applies drown then poison after processing', function()
        local skill = require('scripts/actions/mobskills/water_meeble_warble')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, calls = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER }
        end
        xi.mobskills.mobStatusEffectMove = function(...) calls[#calls + 1] = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 18 and params.element == xi.element.WATER and #calls == 0)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100)
        assert(calls[1][3] == xi.effect.DROWN and calls[1][4] == 50 and calls[1][5] == 3 and calls[1][6] == 60)
        assert(calls[2][3] == xi.effect.POISON and calls[2][4] == 50)
    end)
end)
