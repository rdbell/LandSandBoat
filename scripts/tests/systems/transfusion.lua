require('scripts/actions/mobskills/transfusion')
describe('Transfusion mob skill', function()
    it('uses none magical drain plan after processing', function()
        local skill = require('scripts/actions/mobskills/transfusion')
        local move, process, drain = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, message, drainArgs = nil, nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { getMainLvl = function() return 50 end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobDrainMove = function(...) drainArgs = { ... }; return 123 end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 100)
        assert(params.baseDamage == 52 and params.fTP[1] == 3.00 and params.skipMagicBonusDiff and message == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, {}, sk, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drain
        assert(message == 123 and drainArgs[3] == xi.mobskills.drainType.HP)
    end)
end)
