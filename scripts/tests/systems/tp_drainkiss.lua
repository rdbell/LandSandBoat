require('scripts/actions/mobskills/tp_drainkiss')
describe('TP Drainkiss mob skill', function()
    it('uses target TP base and TP drain after processing', function()
        local skill = require('scripts/actions/mobskills/tp_drainkiss')
        local move, process, drain = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, message, drainArgs = nil, nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = {}
        local target = { getTP = function() return 3000 end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 1500, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobDrainMove = function(...) drainArgs = { ... }; return 123 end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 1500)
        assert(params.baseDamage == 3000 and params.fTP[1] == 0.500 and params.skipDamageAdjustment and message == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, sk, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drain
        assert(message == 123 and drainArgs[3] == xi.mobskills.drainType.TP and drainArgs[4] == 1500)
    end)
end)
