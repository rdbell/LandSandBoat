require('scripts/actions/mobskills/trinary_absorption')
describe('Trinary Absorption mob skill', function()
    it('admits NM/battlefield and drains HP after processing', function()
        local skill = require('scripts/actions/mobskills/trinary_absorption')
        local move, process, drain = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, message = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = {
            isMobType = function(_, t) return false end,
            getMainLvl = function() return 50 end,
        }
        local target = { hasStatusEffect = function() return false end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        mob.isMobType = function(_, t) return t == xi.mobType.NOTORIOUS end
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobDrainMove = function() return 123 end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.baseDamage == 50 and params.fTP[1] == 5.0 and params.skipMagicBonusDiff and message == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, sk, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drain
        assert(message == 123)
    end)
end)
