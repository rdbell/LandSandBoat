describe('Healing Stomp mob skill',function()
 it('sets its buff-host message and returns Regen',function()
  local stomp=require('scripts/actions/mobskills/healing_stomp');local buff=xi.mobskills.mobBuffMove;local call,message=nil,nil;local mob={};local target={};local skill={setMsg=function(_,value)message=value end};xi.mobskills.mobBuffMove=function(...)call={...};return 456 end
  assert(stomp.onMobSkillCheck(target,mob,skill)==0 and stomp.onMobWeaponSkill(mob,target,skill,{})==xi.effect.REGEN);xi.mobskills.mobBuffMove=buff;assert(call[1]==mob and call[2]==xi.effect.REGEN and call[3]==25 and call[4]==3 and call[5]==180 and message==456)
 end)
end)
