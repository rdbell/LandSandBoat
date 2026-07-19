require('scripts/actions/mobskills/astral_flow')

describe('Astral Flow mob skill', function()
    it('uses a positive avatar offset, sends USES, and applies Astral Flow', function()
        local astralFlow = require('scripts/actions/mobskills/astral_flow')
        local originalCallPets = xi.mob.callPets
        local calls = {}
        local message = nil

        xi.mob.callPets = function(mob, avatarId, params)
            calls.mob = mob
            calls.avatarId = avatarId
            calls.params = params
        end

        local mob = {
            getID = function() return 100 end,
            getMobMod = function(_, mod)
                assert(mod == xi.mobMod.ASTRAL_PET_OFFSET)
                return 7
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(astralFlow.onMobSkillCheck(nil, mob, skill) == 0)
        assert(astralFlow.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.ASTRAL_FLOW)

        xi.mob.callPets = originalCallPets

        assert(calls.mob == mob and calls.avatarId == 107)
        assert(calls.params.noAnimation)
        assert(message == xi.msg.basic.USES)
    end)

    it('uses the two-ID fallback for zero and negative offsets', function()
        local astralFlow = require('scripts/actions/mobskills/astral_flow')
        local originalCallPets = xi.mob.callPets
        local avatarIds = {}

        xi.mob.callPets = function(_, avatarId) table.insert(avatarIds, avatarId) end

        for _, offset in ipairs({ 0, -3 }) do
            local mob = {
                getID = function() return 100 end,
                getMobMod = function() return offset end,
            }
            astralFlow.onMobWeaponSkill(mob, nil, { setMsg = function() end }, nil)
        end

        xi.mob.callPets = originalCallPets

        assert(avatarIds[1] == 102 and avatarIds[2] == 102)
    end)
end)

describe('Astral Flow II mob skill', function()
    it('always allows use, sends USES, and marks an existing pet', function()
        local astralFlow = require('scripts/actions/mobskills/astral_flow_2')
        local message, used = nil, nil
        local pet = { setLocalVar = function(_, key, value) used = { key, value } end }
        local mob = { getPet = function() return pet end }
        local skill = { setMsg = function(_, value) message = value end }

        assert(astralFlow.onMobSkillCheck(nil, mob, skill) == 0)
        assert(astralFlow.onMobWeaponSkill(mob, nil, skill, nil) == nil)
        assert(message == xi.msg.basic.USES)
        assert(used[1] == 'astralFlowUsed' and used[2] == 1)
    end)

    it('sends USES but does nothing when there is no pet', function()
        local astralFlow = require('scripts/actions/mobskills/astral_flow_2')
        local message = nil
        local mob = { getPet = function() return nil end }
        local skill = { setMsg = function(_, value) message = value end }

        assert(astralFlow.onMobWeaponSkill(mob, nil, skill, nil) == nil)
        assert(message == xi.msg.basic.USES)
    end)
end)
