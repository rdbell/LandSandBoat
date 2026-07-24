-----------------------------------
-- Title Changer NPC Functions
-----------------------------------
xi = xi or {}
xi.titleChanger = xi.titleChanger or {}

local function titleMask(player, titleGroup)
    local returnValue = 0
    local titles = {}

    if titleGroup and titleGroup.title then
        titles = titleGroup.title
    end

    for i = 1, 28 do
        if titles[i] == nil or titles[i] == 0 or not player:hasTitle(titles[i]) then
            returnValue = bit.bor(returnValue, bit.lshift(1, i))
        end
    end

    return returnValue
end

-----------------------------------
-- public title changer functions
-----------------------------------

-- Go pure half: titlechanger.PlanTriggerFromInjects/PlanTriggerStartParams (6605).
xi.titleChanger.onTrigger = function(player, eventId, titleInfo)
    player:startEvent(
        eventId,
        titleMask(player, titleInfo[1]),
        titleMask(player, titleInfo[2]),
        titleMask(player, titleInfo[3]),
        titleMask(player, titleInfo[4]),
        titleMask(player, titleInfo[5]),
        titleMask(player, titleInfo[6]),
        1,
        player:getGil()
    )
end

-- Go pure half: titlechanger.PlanEventFinishWithGil (6588); PlanEventFinishFromInjects getGil (6605).
xi.titleChanger.onEventFinish = function(player, csid, option, eventId, titleInfo)
    if csid == eventId then
        local group = titleInfo[bit.rshift(option, 8) + 1]
        if group then
            local title = group.title[option % 256]
            if title and player:delGil(group.cost) then
                player:setTitle(title)
            end
        end
    end
end
