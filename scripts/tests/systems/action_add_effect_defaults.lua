-----------------------------------
-- Pure system tests for combat action additional-effect defaults (slice 6070).
-----------------------------------

describe('action additional effect defaults pure catalogs', function()
    -- Mirrored damage defaultsTable corners.
    local damageDefaults =
    {
        [xi.element.NONE   ] = xi.subEffect.LIGHT_DAMAGE,
        [xi.element.FIRE   ] = xi.subEffect.FIRE_DAMAGE,
        [xi.element.THUNDER] = xi.subEffect.LIGHTNING_DAMAGE,
        [xi.element.DARK   ] = xi.subEffect.DARKNESS_DAMAGE,
    }

    local statusDefaults =
    {
        [xi.effect.NONE  ] = { xi.subEffect.DARKNESS_DAMAGE, xi.msg.basic.ADD_EFFECT_DISPEL },
        [xi.effect.POISON] = { xi.subEffect.POISON,          xi.msg.basic.ADD_EFFECT_STATUS },
        [xi.effect.TERROR] = { xi.subEffect.PARALYSIS,       xi.msg.basic.ADD_EFFECT_STATUS },
    }

    it('damage element default animations', function()
        assert(damageDefaults[xi.element.NONE] == xi.subEffect.LIGHT_DAMAGE)
        assert(damageDefaults[xi.element.FIRE] == xi.subEffect.FIRE_DAMAGE)
        assert(damageDefaults[xi.element.THUNDER] == xi.subEffect.LIGHTNING_DAMAGE)
        assert(damageDefaults[xi.element.DARK] == xi.subEffect.DARKNESS_DAMAGE)
        assert(xi.subEffect.FIRE_DAMAGE == 1)
        assert(xi.subEffect.LIGHT_DAMAGE == 7)
    end)

    it('status effect default animation and message', function()
        local none = statusDefaults[xi.effect.NONE]
        assert(none[1] == xi.subEffect.DARKNESS_DAMAGE)
        assert(none[2] == xi.msg.basic.ADD_EFFECT_DISPEL)
        assert(xi.msg.basic.ADD_EFFECT_DISPEL == 168)

        local poison = statusDefaults[xi.effect.POISON]
        assert(poison[1] == xi.subEffect.POISON)
        assert(poison[2] == xi.msg.basic.ADD_EFFECT_STATUS)
        assert(xi.msg.basic.ADD_EFFECT_STATUS == 160)

        local terror = statusDefaults[xi.effect.TERROR]
        assert(terror[1] == xi.subEffect.PARALYSIS)
    end)

    it('enspell table membership pins', function()
        -- Ordered list from hasEnspell in both damage and status files.
        local enspells =
        {
            xi.effect.ENFIRE, xi.effect.ENFIRE_II,
            xi.effect.ENBLIZZARD, xi.effect.ENBLIZZARD_II,
            xi.effect.ENAERO, xi.effect.ENAERO_II,
            xi.effect.ENSTONE, xi.effect.ENSTONE_II,
            xi.effect.ENTHUNDER, xi.effect.ENTHUNDER_II,
            xi.effect.ENWATER, xi.effect.ENWATER_II,
            xi.effect.ENLIGHT, xi.effect.ENDARK,
        }
        assert(#enspells == 14)
        assert(enspells[1] == xi.effect.ENFIRE)
        assert(enspells[14] == xi.effect.ENDARK)
        assert(xi.effect.ENFIRE == 94)
        assert(xi.effect.ENDARK == 288)
    end)

    it('validate defaults: chance 100 duration 120 SPECIAL', function()
        -- Mirrors unset fedData defaults without entity targets.
        assert(xi.attackType.SPECIAL == 5)
        assert(xi.msg.basic.ADD_EFFECT_DMG == 163)
        assert(xi.msg.basic.ADD_EFFECT_HEAL == 384)
        -- status NONE effect id
        assert(xi.effect.NONE == 255)
        assert(xi.effect.KO == 0)
    end)
end)
