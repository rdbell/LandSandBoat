require('scripts/actions/mobskills/bilgestorm')
describe('Bilgestorm mob skill', function()
    it('uses Blunt plan and three random-power downs when processed', function()
        local skill = require('scripts/actions/mobskills/bilgestorm')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local origRandom = math.random
        local params, damage, effects, rolls = nil, nil, {}, {22, 23, 24}
        local ri = 0
        math.random = function(a, b)
            assert(a == 20 and b == 25)
            ri = ri + 1
            return rolls[ri]
        end
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            table.insert(effects, { status, power, tick, duration })
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 2.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and #effects == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        math.random = origRandom
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 100)
        assert(effects[1][1] == xi.effect.ACCURACY_DOWN and effects[1][2] == 22)
        assert(effects[2][1] == xi.effect.ATTACK_DOWN and effects[2][2] == 23)
        assert(effects[3][1] == xi.effect.DEFENSE_DOWN and effects[3][2] == 24)
    end)
end)
