describe('Healing Breeze mob skill',function()
 it('clamps skill TP, floors its two-stage max-HP heal, and sets Self Heal',function()
  local breeze=require('scripts/actions/mobskills/healing_breeze');local heal=xi.mobskills.mobHealMove;local amount,message=nil,nil;local tp=3000;local mob={getMaxHP=function()return 1000 end};local target={};local skill={getTP=function()return tp end,setMsg=function(_,value)message=value end};xi.mobskills.mobHealMove=function(_,value)amount=value;return 456 end
  assert(breeze.onMobSkillCheck(target,mob,skill)==0 and breeze.onMobWeaponSkill(mob,target,skill,{})==456);assert(amount==250 and message==xi.msg.basic.SELF_HEAL);tp=0;breeze.onMobWeaponSkill(mob,target,skill,{});xi.mobskills.mobHealMove=heal;assert(amount==167)
 end)
end)
