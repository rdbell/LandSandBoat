require('scripts/globals/job_utils/dragoon')

describe('Dragoon Angon availability check', function()
    it('requires Angon in the ammo slot', function()
        local ammoID
        local player = {
            getEquipID = function(_, slot)
                assert(slot == xi.slot.AMMO)
                return ammoID
            end,
        }

        assert(xi.job_utils.dragoon.abilityCheckAngon(player, {}, {}) == xi.msg.basic.CANNOT_PERFORM)

        ammoID = xi.item.ANGON
        assert(xi.job_utils.dragoon.abilityCheckAngon(player, {}, {}) == 0)
    end)
end)
