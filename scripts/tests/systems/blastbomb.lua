require('scripts/actions/mobskills/blastbomb')
describe('Blastbomb mob skill', function()
    it('uses Fire plan, breakBind false, and TP-scaled Bind', function()
        local skill = require('scripts/actions/mobskills/blastbomb')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, flags, effect = nil, nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local sk = { getTP = function() return 1000 end }
        local target = {
            takeDamage = function(_, v, s, a, d, opts)
                damage = { v, s, a, d }
                flags = opts
            end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 90)
        assert(params.baseDamage == 52 and params.fTP[1] == 3.0 and damage == nil and effect == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 90)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 90 and flags.breakBind == false)
        assert(effect[1] == xi.effect.BIND and effect[4] == 30)
    end)
end)
