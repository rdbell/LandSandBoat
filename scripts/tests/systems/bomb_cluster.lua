require('scripts/mixins/families/bomb_cluster')

describe('Bomb Cluster mob-skill selector', function()
    local function choose(animationSub, hpp, rolls)
        local mob = {
            getAnimationSub = function()
                return animationSub
            end,
            getHPP = function()
                return hpp
            end,
        }
        local originalRandom = math.random
        math.random = function()
            return table.remove(rolls, 1)
        end
        local result = xi.mix.clusters.onMobMobskillChoose(mob, nil)
        math.random = originalRandom
        return result
    end

    it('selects the exact candidates for every animation family and threshold', function()
        -- First roll is the 5% death-version check; second selects from tpList.
        assert(choose(3, 66, { 6, 2 }) == xi.mobSkill.SLING_BOMB_1) -- strict <66
        assert(choose(4, 65, { 5, 5 }) == xi.mobSkill.SELF_DESTRUCT_CLUSTER_3_DEATH)
        assert(choose(5, 32, { 6, 5 }) == xi.mobSkill.SELF_DESTRUCT_CLUSTER_2)
        assert(choose(6, 20, { 6, 1 }) == xi.mobSkill.REFUELING_1) -- strict <20
        assert(choose(6, 19, { 6, 2 }) == xi.mobSkill.SELF_DESTRUCT_CLUSTER_1_DEATH)
        assert(choose(11, 65, { 6, 5 }) == xi.mobSkill.SELF_DESTRUCT_CLUSTER_3)
        assert(choose(12, 65, { 5, 5 }) == xi.mobSkill.SELF_DESTRUCT_CLUSTER_3_DEATH)
        assert(choose(13, 32, { 5, 5 }) == xi.mobSkill.SELF_DESTRUCT_CLUSTER_2_DEATH)
        assert(choose(14, 19, { 6, 2 }) == xi.mobSkill.SELF_DESTRUCT_CLUSTER_1_DEATH)
        assert(choose(99, 1, { 1, 1 }) == xi.mobSkill.REFUELING_1)
    end)
end)
