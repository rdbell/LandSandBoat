require('scripts/globals/job_utils/geomancer')

describe('Geomancer Indi casting check host', function()
    it('allows self casts and Entrust casts, but rejects other targets otherwise', function()
        local caster = {
            hasStatusEffect = function(_, effect)
                assert(effect == xi.effect.ENTRUST)
                return false
            end,
        }
        local target = {}
        assert(xi.job_utils.geomancer.indiOnMagicCastingCheck(caster, caster, {}) == 0)
        assert(xi.job_utils.geomancer.indiOnMagicCastingCheck(caster, target, {}) == xi.msg.basic.MAGIC_CANNOT_BE_CAST)

        caster.hasStatusEffect = function() return true end
        assert(xi.job_utils.geomancer.indiOnMagicCastingCheck(caster, target, {}) == 0)
    end)
end)
