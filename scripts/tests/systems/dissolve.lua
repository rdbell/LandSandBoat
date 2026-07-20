require('scripts/actions/mobskills/dissolve')

describe('Dissolve mob skill', function()
    it('transfers mob-target enmity and updates claims/resets for PCs only', function()
        local dissolve = require('scripts/actions/mobskills/dissolve')
        local transfers, claims, resets = {}, {}, {}
        local player = { isPC = function() return true end }
        local pet = { isPC = function() return false end }
        local target = {
            isMob = function() return true end,
            getEnmityList = function() return {
                { entity = player, ce = 12, ve = 34 },
                { entity = pet, ce = 56, ve = 78 },
            } end,
            resetEnmity = function(_, entity) resets[#resets + 1] = entity end,
        }
        local mob = {
            addEnmity = function(_, entity, ce, ve) transfers[#transfers + 1] = { entity, ce, ve } end,
            updateClaim = function(_, entity) claims[#claims + 1] = entity end,
        }

        assert(dissolve.onMobSkillCheck({}, {}, {}) == 0)
        assert(dissolve.onMobWeaponSkill(mob, target, {}, {}) == 0)
        assert(#transfers == 2 and transfers[1][1] == player and transfers[1][2] == 12 and transfers[1][3] == 34)
        assert(transfers[2][1] == pet and transfers[2][2] == 56 and transfers[2][3] == 78)
        assert(#claims == 1 and claims[1] == player and #resets == 1 and resets[1] == player)

        target.isMob = function() return false end
        assert(dissolve.onMobWeaponSkill(mob, target, {}, {}) == 0)
        assert(#transfers == 2 and #claims == 1 and #resets == 1)
    end)
end)
