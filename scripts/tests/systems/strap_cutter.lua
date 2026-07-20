require('scripts/actions/mobskills/strap_cutter')
describe('Strap Cutter mob skill', function()
    it('no-effects non-PC and applies encumbrance after unequip on PC', function()
        local cutter = require('scripts/actions/mobskills/strap_cutter')
        local isPC, message, unequipped, effects = false, nil, {}, {}
        local target = {
            isPC = function() return isPC end,
            hasSlotEquipped = function(_, slot) return slot == xi.slot.MAIN or slot == xi.slot.HEAD end,
            unequipItem = function(_, slot) unequipped[#unequipped+1] = slot end,
            getStatusEffect = function(_, e) return effects[e] end,
            delStatusEffectSilent = function(_, e) effects[e] = nil end,
            addStatusEffect = function(_, e, opts) effects[e] = opts end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        assert(cutter.onMobSkillCheck(target, {}, skill) == 0)
        cutter.onMobWeaponSkill({}, target, skill, {})
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
        isPC = true
        local random, shuffle = math.random, utils.shuffle
        math.random = function(a, b) assert(a == 3 and b == 5); return 3 end
        utils.shuffle = function(t) return { xi.slot.MAIN, xi.slot.HEAD, xi.slot.BODY, xi.slot.BACK } end
        cutter.onMobWeaponSkill({}, target, skill, {})
        math.random, utils.shuffle = random, shuffle
        assert(message == xi.msg.basic.USES)
        assert(unequipped[1] == xi.slot.MAIN and unequipped[2] == xi.slot.SUB)
        assert(effects[xi.effect.ENCUMBRANCE_I] and effects[xi.effect.ENCUMBRANCE_I].duration == 60)
    end)
end)
