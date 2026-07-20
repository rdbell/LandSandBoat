require('scripts/actions/mobskills/exuviation')

describe('Exuviation mob skill', function()
    it('erases effects through NONE, sets Self Heal, and heals from level and count', function()
        local exuviation = require('scripts/actions/mobskills/exuviation')
        local healMove = xi.mobskills.mobHealMove
        local erased, healAmount, message = 0, nil, nil
        local effects = { 3, 4, xi.effect.NONE }
        local mob = {
            eraseStatusEffect = function()
                erased = erased + 1
                return table.remove(effects, 1)
            end,
            getMainLvl = function() return 80 end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobHealMove = function(source, amount)
            assert(source == mob)
            healAmount = amount
            return 1437
        end
        assert(exuviation.onMobSkillCheck({}, mob, skill) == 0)
        assert(exuviation.onMobWeaponSkill(mob, {}, skill, {}) == 1437)
        xi.mobskills.mobHealMove = healMove
        assert(erased == 3 and healAmount == 1598 and message == xi.msg.basic.SELF_HEAL)
    end)
end)
