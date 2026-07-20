require('scripts/actions/mobskills/amon_drive')
describe('Amon Drive mob skill', function()
    it('uses physical plan and processed Paralyze Petrify Poison', function()
        local skill = require('scripts/actions/mobskills/amon_drive')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local origRandom = math.random
        local params, damage, effects = nil, nil, {}
        math.random = function(a, b)
            assert(a == 8 and b == 15)
            return 10
        end
        local mob = {
            getWeaponDmg = function() return 80 end,
            getMainLvl = function() return 50 end,
        }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 150, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            table.insert(effects, { status, power, tick, duration })
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 150)
        assert(params.fTP[1] == 2.5 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and #effects == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 150)
        math.random = origRandom
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 150)
        assert(effects[1][1] == xi.effect.PARALYSIS and effects[1][2] == 25 and effects[1][4] == 60)
        assert(effects[2][1] == xi.effect.PETRIFICATION and effects[2][4] == 10 + 50 / 3)
        assert(effects[3][1] == xi.effect.POISON and effects[3][2] == 10 and effects[3][3] == 3 and effects[3][4] == 60)
    end)
end)
