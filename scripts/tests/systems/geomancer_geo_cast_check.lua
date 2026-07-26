require('scripts/globals/job_utils/geomancer')

describe('Geomancer GEO casting check host', function()
    it('applies luopan, pet, area, and main-job gates in order', function()
        local function check(petID, canUsePet, mainJob)
            local pet
            if petID then
                pet = { getPetID = function() return petID end }
            end
            local caster = {
                getPet = function() return pet end,
                canUseMisc = function(_, misc)
                    assert(misc == xi.zoneMisc.PET)
                    return canUsePet
                end,
                getMainJob = function() return mainJob end,
            }
            return xi.job_utils.geomancer.geoOnMagicCastingCheck(caster, {}, {})
        end

        assert(check(xi.petId.LUOPAN, false, 0) == xi.msg.basic.LUOPAN_ALREADY_PLACED)
        assert(check(0, false, 0) == xi.msg.basic.ALREADY_HAS_A_PET)
        assert(check(nil, false, 0) == xi.msg.basic.CANT_BE_USED_IN_AREA)
        assert(check(nil, true, 0) == xi.msg.basic.MAGIC_CANNOT_CAST)
        assert(check(nil, true, xi.job.GEO) == 0)
    end)
end)
