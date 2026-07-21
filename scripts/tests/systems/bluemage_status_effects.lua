-----------------------------------
-- Pure system tests for Blue Mage status-effect scripts.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        jp = opts.jp or {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
        end,
    }
end

describe('Blue Mage status-effect pure plans', function()
    it('unbridled wisdom applies CONSERVE_MP equal to JP times 3', function()
        local script = require('scripts/effects/unbridled_wisdom')
        local target = stubTarget({ jp = { [xi.jp.UNBRIDLED_WISDOM_EFFECT] = 4 } })
        script.onEffectGain(target, {})
        assert(target.mods[xi.mod.CONSERVE_MP] == 12)
        script.onEffectLose(target, {})
        assert(target.mods[xi.mod.CONSERVE_MP] == 0)
    end)

    it('azure lore affinities diffusion and unbridled learning are empty', function()
        for _, name in ipairs({
            'azure_lore', 'chain_affinity', 'burst_affinity',
            'diffusion', 'unbridled_learning',
        }) do
            local script = require('scripts/effects/' .. name)
            local target = stubTarget()
            script.onEffectGain(target, {})
            script.onEffectLose(target, {})
            local count = 0
            for _ in pairs(target.mods) do
                count = count + 1
            end
            assert(count == 0, name)
        end
    end)
end)
