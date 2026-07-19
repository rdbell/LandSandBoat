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

describe('Astral Flow pet mob skill', function()
    it('requires an active elemental pet', function()
        local astralFlow = require('scripts/actions/mobskills/astral_flow_pet')
        local skill = {}

        assert(astralFlow.onMobSkillCheck(nil, { getPet = function() return nil end }, skill) == 1)

        local inactive = false
        local pet = {
            getEcosystem = function() return xi.ecosystem.ELEMENTAL end,
            hasStatusEffect = function() return inactive end,
        }
        local mob = { getPet = function() return pet end }
        assert(astralFlow.onMobSkillCheck(nil, mob, skill) == 0)

        inactive = true
        assert(astralFlow.onMobSkillCheck(nil, mob, skill) == 1)
        pet.getEcosystem = function() return 0 end
        assert(astralFlow.onMobSkillCheck(nil, mob, skill) == 1)
    end)

    it('does not message or act when no pet exists', function()
        local astralFlow = require('scripts/actions/mobskills/astral_flow_pet')
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }

        assert(astralFlow.onMobWeaponSkill({ getPet = function() return nil end }, nil, skill, nil) == nil)
        assert(message == nil)
    end)

    it('returns Astral Flow without using an inactive pet ability', function()
        local astralFlow = require('scripts/actions/mobskills/astral_flow_pet')
        local message, used = nil, nil
        local pet = {
            hasStatusEffect = function(_, effect) return effect == xi.effect.STUN end,
            useMobAbility = function(_, skillId) used = skillId end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(astralFlow.onMobWeaponSkill({ getPet = function() return pet end }, nil, skill, nil) == xi.effect.ASTRAL_FLOW)
        assert(message == xi.msg.basic.USES and used == nil)
    end)

    it('uses the matching avatar skill or the Searing Light default', function()
        local astralFlow = require('scripts/actions/mobskills/astral_flow_pet')
        local cases = {
            { 246, xi.mobSkill.HOWLING_MOON_2 }, { 381, xi.mobSkill.HOWLING_MOON_2 },
            { 248, xi.mobSkill.INFERNO_1 }, { 383, xi.mobSkill.INFERNO_1 },
            { 255, xi.mobSkill.EARTHEN_FURY_1 }, { 388, xi.mobSkill.EARTHEN_FURY_1 },
            { 249, xi.mobSkill.TIDAL_WAVE_1 }, { 384, xi.mobSkill.TIDAL_WAVE_1 },
            { 247, xi.mobSkill.AERIAL_BLAST_1 }, { 382, xi.mobSkill.AERIAL_BLAST_1 },
            { 253, xi.mobSkill.DIAMOND_DUST_1 }, { 387, xi.mobSkill.DIAMOND_DUST_1 },
            { 252, xi.mobSkill.JUDGMENT_BOLT_1 }, { 386, xi.mobSkill.JUDGMENT_BOLT_1 },
            { 243, xi.mobSkill.SEARING_LIGHT_1 }, { 379, xi.mobSkill.SEARING_LIGHT_1 },
            { 999, xi.mobSkill.SEARING_LIGHT_1 },
        }

        for _, case in ipairs(cases) do
            local used = nil
            local pet = {
                hasStatusEffect = function() return false end,
                getSpecies = function() return case[1] end,
                useMobAbility = function(_, skillId) used = skillId end,
            }
            local skill = { setMsg = function() end }
            assert(astralFlow.onMobWeaponSkill({ getPet = function() return pet end }, nil, skill, nil) == xi.effect.ASTRAL_FLOW)
            assert(used == case[2])
        end
    end)
end)
