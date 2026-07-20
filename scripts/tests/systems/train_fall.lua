require('scripts/actions/mobskills/train_fall')
describe('Train Fall mob skill', function()
    it('uses fTP 6 skip-PDIF plan and HP drain after processing', function()
        local skill = require('scripts/actions/mobskills/train_fall')
        local move, process, drain = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, message, drainArgs = nil, nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { getWeaponDmg = function() return 80 end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 480, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobDrainMove = function(...) drainArgs = { ... }; return 123 end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 480)
        assert(params.fTP[1] == 6.0 and params.skipPDIF and message == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, {}, sk, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drain
        assert(message == 123 and drainArgs[3] == xi.mobskills.drainType.HP and drainArgs[4] == 480)
    end)
end)
