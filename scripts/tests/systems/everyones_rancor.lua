require('scripts/actions/mobskills/everyones_rancor')

describe("Everyone's Rancor mob skill", function()
    it('admits an eligible NM once and handles damage, pet, and necklace branches', function()
        local rancor = require('scripts/actions/mobskills/everyones_rancor')
        local random = math.random
        local used = 0
        local mob = {
            isNM = function() return true end,
            getHP = function() return 25 end,
            getMaxHP = function() return 100 end,
            getLocalVar = function() return used end,
            setLocalVar = function(_, _, value) used = value end,
        }
        local function target(opts)
            return {
                getCharVar = function() return opts.kills or 0 end,
                isPet = function() return opts.pet or false end,
                isPC = function() return opts.pc or false end,
                getEquipID = function() return opts.necklace and xi.item.UGGALEPIH_NECKLACE or 0 end,
                getTP = function() return opts.tp or 0 end,
                setTP = function(_, value) opts.setTP = value end,
                takeDamage = function(_, ...) opts.damage = { ... } end,
            }
        end
        assert(rancor.onMobSkillCheck({}, mob, {}) == 0 and used == 1)
        assert(rancor.onMobSkillCheck({}, mob, {}) == 1)
        local normal = { kills = 12 }; local normalTarget = target(normal)
        assert(rancor.onMobWeaponSkill(mob, normalTarget, {}, {}) == 600)
        assert(normal.damage[1] == 600 and normal.damage[2] == mob and normal.damage[3] == xi.attackType.MAGICAL and normal.damage[4] == xi.damageType.ELEMENTAL)
        math.random = function(low, high) assert(low == 50 and high == 100); return 50 end
        local pet = { kills = 12, pet = true }; assert(rancor.onMobWeaponSkill(mob, target(pet), {}, {}) == 2500)
        math.random = random
        local necklace = { kills = 12, pc = true, necklace = true, tp = 1500 }
        assert(rancor.onMobWeaponSkill(mob, target(necklace), {}, {}) == 300 and necklace.setTP == 0)
        local negate = { kills = 12, pc = true, necklace = true, tp = 3000 }
        assert(rancor.onMobWeaponSkill(mob, target(negate), {}, {}) == 0 and negate.setTP == 0)
    end)
end)
