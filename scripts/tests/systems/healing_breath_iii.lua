describe('Healing Breath III mob skill',function()
 it('sets its self-heal message and heals from max HP',function()
  local breath=require('scripts/actions/mobskills/healing_breath_iii');local heal=xi.mobskills.mobHealMove;local amount,message=nil,nil;local mob={getMaxHP=function()return 1000 end};local target={};local skill={setMsg=function(_,value)message=value end};xi.mobskills.mobHealMove=function(_,value)amount=value;return 456 end
  assert(breath.onMobSkillCheck(target,mob,skill)==0 and breath.onMobWeaponSkill(mob,target,skill,{})==456);xi.mobskills.mobHealMove=heal;assert(amount==306 and message==xi.msg.basic.SELF_HEAL)
 end)
end)
