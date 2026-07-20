require('scripts/actions/mobskills/soul_drain')
describe('Soul Drain mob skill', function()
    it('uses none-element magical plan and drains HP after processing', function()
        local drain = require('scripts/actions/mobskills/soul_drain')
        local move, process, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, drainArgs, message = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 75, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobDrainMove = function(...) drainArgs = { ... }; return 456 end
        xi.mobskills.processDamage = function() return false end
        assert(drain.onMobSkillCheck({}, mob, skill) == 0 and drain.onMobWeaponSkill(mob, {}, skill, {}) == 75)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.5 and params.skipMagicBonusDiff and drainArgs == nil)
        xi.mobskills.processDamage = function() return true end
        drain.onMobWeaponSkill(mob, {}, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drainMove
        assert(drainArgs[3] == xi.mobskills.drainType.HP and drainArgs[4] == 75 and message == 456)
    end)
end)
