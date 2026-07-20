require('scripts/actions/mobskills/everyones_grudge')

describe("Everyone's Grudge mob skill", function()
    it('rejects NM use and handles normal, pet, NM, and necklace damage branches', function()
        local grudge = require('scripts/actions/mobskills/everyones_grudge')
        local random = math.random
        local mob = { isNM = function() return false end }
        local nm = { isNM = function() return true end }
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
        assert(grudge.onMobSkillCheck({}, mob, {}) == 0 and grudge.onMobSkillCheck({}, nm, {}) == 1)
        local normal = {}; assert(grudge.onMobWeaponSkill(mob, target(normal), {}, {}) == 0)
        normal.kills = 12; local normalTarget = target(normal)
        assert(grudge.onMobWeaponSkill(mob, normalTarget, {}, {}) == 60)
        assert(normal.damage[1] == 60 and normal.damage[2] == mob and normal.damage[3] == xi.attackType.MAGICAL and normal.damage[4] == xi.damageType.ELEMENTAL)
        math.random = function(low, high) assert(low == 30 and high == 100); return 30 end
        local pet = { kills = 12, pet = true }; assert(grudge.onMobWeaponSkill(mob, target(pet), {}, {}) == 150)
        math.random = random
        local nmTarget = { kills = 12 }; assert(grudge.onMobWeaponSkill(nm, target(nmTarget), {}, {}) == 600)
        local necklace = { kills = 12, pc = true, necklace = true, tp = 1500 }
        assert(grudge.onMobWeaponSkill(mob, target(necklace), {}, {}) == 30 and necklace.setTP == 0)
        local negate = { kills = 12, pc = true, necklace = true, tp = 3000 }
        assert(grudge.onMobWeaponSkill(mob, target(negate), {}, {}) == 0 and negate.setTP == 0)
    end)
end)
