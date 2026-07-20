require('scripts/actions/mobskills/blizzard_meeble_warble')
describe('Blizzard Meeble Warble mob skill', function()
    it('uses Ice plan and processed Paralysis plus Frost', function()
        local skill = require('scripts/actions/mobskills/blizzard_meeble_warble')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 150, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            table.insert(effects, { status, power, tick, duration })
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 150)
        assert(params.fTP[1] == 9.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and #effects == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 150)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 150)
        assert(effects[1][1] == xi.effect.PARALYSIS and effects[1][2] == 50)
        assert(effects[2][1] == xi.effect.FROST and effects[2][2] == 50 and effects[2][3] == 3)
    end)
end)
