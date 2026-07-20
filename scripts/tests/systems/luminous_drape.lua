require('scripts/actions/mobskills/luminous_drape')

describe('Luminous Drape mob skill', function()
    it('misses non-player targets without calling the charm hosts', function()
        local luminousDrape = require('scripts/actions/mobskills/luminous_drape')
        local status = xi.mobskills.mobStatusEffectMove
        local random = math.random
        local message, statusCalled, randomCalled = nil, false, false
        local mob = {
            charm = function() error('charm should not be called') end,
            resetEnmity = function() error('resetEnmity should not be called') end,
        }
        local target = { isPC = function() return false end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function() statusCalled = true end
        math.random = function() randomCalled = true end

        assert(luminousDrape.onMobSkillCheck(target, mob, skill) == 0)
        assert(luminousDrape.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.CHARM_I)
        assert(message == xi.msg.basic.SKILL_MISS and not statusCalled and not randomCalled)

        xi.mobskills.mobStatusEffectMove = status
        math.random = random
    end)

    it('uses a random Charm duration and charms only after enfeebling success', function()
        local luminousDrape = require('scripts/actions/mobskills/luminous_drape')
        local status = xi.mobskills.mobStatusEffectMove
        local random = math.random
        local calls, message, statusResult = {}, nil, 123
        local mob = {
            charm = function(_, target) table.insert(calls, { 'charm', target }) end,
            resetEnmity = function(_, target) table.insert(calls, { 'reset', target }) end,
        }
        local target = { isPC = function() return true end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...)
            table.insert(calls, { 'status', ... })
            return statusResult
        end
        math.random = function(minimum, maximum)
            assert(minimum == 5 and maximum == 25)
            return 17
        end

        assert(luminousDrape.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.CHARM_I)
        assert(calls[1][1] == 'status' and calls[1][2] == mob and calls[1][3] == target and calls[1][4] == xi.effect.CHARM_I and calls[1][5] == 0 and calls[1][6] == 3 and calls[1][7] == 17)
        assert(#calls == 1 and message == 123)

        statusResult = xi.msg.basic.SKILL_ENFEEB_IS
        assert(luminousDrape.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.CHARM_I)
        assert(calls[2][1] == 'status' and calls[3][1] == 'charm' and calls[3][2] == target and calls[4][1] == 'reset' and calls[4][2] == target)
        assert(message == xi.msg.basic.SKILL_ENFEEB_IS)

        xi.mobskills.mobStatusEffectMove = status
        math.random = random
    end)
end)
