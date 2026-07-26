require('scripts/globals/job_utils/geomancer')

describe('Geomancer Indi spell host', function()
    it('stores base potency, snapshots aura size, applies Bolster, and consumes Entrust', function()
        local localPotency, aura, auraSize, entrustRemoved
        local caster = {
            getSkillLevel = function() return 0 end,
            getMod = function() return 0 end,
	        getObjType = function() return xi.objType.PC end,
	        getMaxGearMod = function() return 0 end,
	        getEquipID = function() return 0 end,
	        getWeaponSkillType = function() return 0 end,
            hasStatusEffect = function(_, effect)
                return effect == xi.effect.BOLSTER or effect == xi.effect.WIDENED_COMPASS or effect == xi.effect.ENTRUST
            end,
            setMod = function(_, _, value) auraSize = value end,
            delStatusEffectSilent = function(_, effect) entrustRemoved = effect end,
        }
        local target = {
            setLocalVar = function(_, _, value) localPotency = value end,
            hasStatusEffect = function(_, effect) return effect == xi.effect.BOLSTER end,
            addStatusEffect = function(_, _, options) aura = options end,
        }
        local spell = { getID = function() return xi.magic.spell.INDI_REGEN end }

        assert(xi.job_utils.geomancer.doIndiSpell(caster, target, spell) == xi.effect.GEO_REGEN)
        assert(localPotency == 1)
        assert(aura.duration == 180 and aura.subType == xi.effect.GEO_REGEN and aura.subPower == 2)
        assert(auraSize == 625)
        assert(entrustRemoved == xi.effect.ENTRUST)
    end)
end)
