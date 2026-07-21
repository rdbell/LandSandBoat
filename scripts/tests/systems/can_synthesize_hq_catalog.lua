-----------------------------------
-- Pure system tests for canSynthesizeHQ skill→Mod catalog (slice 6103).
-- Source: src/map/utils/synthutils.cpp canSynthesizeHQ (~462–497)
-- Mod IDs: src/map/modifier.h SYNTH_ANTI_HQ_* = 144…151
-----------------------------------

describe('canSynthesizeHQ skill to Mod catalog', function()
    local SKILL_WOODWORKING  = 49
    local SKILL_SMITHING     = 50
    local SKILL_GOLDSMITHING = 51
    local SKILL_CLOTHCRAFT   = 52
    local SKILL_LEATHERCRAFT = 53
    local SKILL_BONECRAFT    = 54
    local SKILL_ALCHEMY      = 55
    local SKILL_COOKING      = 56

    local MOD_NONE                      = 0
    local MOD_SYNTH_ANTI_HQ_WOODWORKING  = 144
    local MOD_SYNTH_ANTI_HQ_SMITHING     = 145
    local MOD_SYNTH_ANTI_HQ_GOLDSMITHING = 146
    local MOD_SYNTH_ANTI_HQ_CLOTHCRAFT   = 147
    local MOD_SYNTH_ANTI_HQ_LEATHERCRAFT = 148
    local MOD_SYNTH_ANTI_HQ_BONECRAFT    = 149
    local MOD_SYNTH_ANTI_HQ_ALCHEMY      = 150
    local MOD_SYNTH_ANTI_HQ_COOKING      = 151

    -- Pure skillID → Mod catalog (switch in canSynthesizeHQ).
    local function antiHQModID(skillID)
        if skillID == SKILL_WOODWORKING then
            return MOD_SYNTH_ANTI_HQ_WOODWORKING
        elseif skillID == SKILL_SMITHING then
            return MOD_SYNTH_ANTI_HQ_SMITHING
        elseif skillID == SKILL_GOLDSMITHING then
            return MOD_SYNTH_ANTI_HQ_GOLDSMITHING
        elseif skillID == SKILL_CLOTHCRAFT then
            return MOD_SYNTH_ANTI_HQ_CLOTHCRAFT
        elseif skillID == SKILL_LEATHERCRAFT then
            return MOD_SYNTH_ANTI_HQ_LEATHERCRAFT
        elseif skillID == SKILL_BONECRAFT then
            return MOD_SYNTH_ANTI_HQ_BONECRAFT
        elseif skillID == SKILL_ALCHEMY then
            return MOD_SYNTH_ANTI_HQ_ALCHEMY
        elseif skillID == SKILL_COOKING then
            return MOD_SYNTH_ANTI_HQ_COOKING
        end
        return MOD_NONE
    end

    -- Pure gate after getMod inject.
    local function canSynthesizeHQ(antiHQMod)
        return antiHQMod == 0
    end

    -- Full inject: skill + getMod table.
    local function canSynthesizeHQLookup(skillID, mods)
        local modID = antiHQModID(skillID)
        local value = mods[modID] or 0
        return canSynthesizeHQ(value)
    end

    it('maps each craft skill to sequential SYNTH_ANTI_HQ mods 144-151', function()
        local skills = {
            SKILL_WOODWORKING, SKILL_SMITHING, SKILL_GOLDSMITHING, SKILL_CLOTHCRAFT,
            SKILL_LEATHERCRAFT, SKILL_BONECRAFT, SKILL_ALCHEMY, SKILL_COOKING,
        }
        for i = 1, #skills do
            assert(antiHQModID(skills[i]) == 143 + i)
        end
    end)

    it('non-craft skill maps to Mod::NONE', function()
        assert(antiHQModID(0) == MOD_NONE)
        assert(antiHQModID(48) == MOD_NONE)
        assert(antiHQModID(57) == MOD_NONE)
        assert(antiHQModID(1) == MOD_NONE)
    end)

    it('zero mods allow HQ for all crafts', function()
        local mods = {}
        for skill = SKILL_WOODWORKING, SKILL_COOKING do
            assert(canSynthesizeHQLookup(skill, mods) == true)
        end
    end)

    it('smithing anti-HQ ring blocks only smithing', function()
        local mods = { [MOD_SYNTH_ANTI_HQ_SMITHING] = 1 }
        assert(canSynthesizeHQLookup(SKILL_SMITHING, mods) == false)
        assert(canSynthesizeHQLookup(SKILL_COOKING, mods) == true)
        assert(canSynthesizeHQLookup(SKILL_WOODWORKING, mods) == true)
    end)

    it('non-craft with empty mods allows HQ (ModNone → 0)', function()
        assert(canSynthesizeHQLookup(0, {}) == true)
    end)

    it('canSynthesizeHQ pure gate', function()
        assert(canSynthesizeHQ(0) == true)
        assert(canSynthesizeHQ(1) == false)
        assert(canSynthesizeHQ(-1) == false)
    end)
end)
