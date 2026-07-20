require('scripts/actions/mobskills/hungry_crunch')
describe('Hungry Crunch mob skill', function()
    it('drains HP resets TP and deletes food when processed', function()
        local skill = require('scripts/actions/mobskills/hungry_crunch')
        local magicalMove, processDamage, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, damage, msg, tp, food = nil, nil, nil, 1000, true
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, v) damage = v end,
            setTP = function(_, v) tp = v end,
            delStatusEffectSilent = function(_, e) if e == xi.effect.FOOD then food = false end end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=80, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.NONE } end
        xi.mobskills.mobDrainMove = function() return 55 end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 80)
        assert(params.skipMagicBonusDiff and params.fTP[1] == 2.0 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 80)
        assert(damage == 80 and msg == 55 and tp == 0 and food == false)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = magicalMove, processDamage, drainMove
    end)
end)
