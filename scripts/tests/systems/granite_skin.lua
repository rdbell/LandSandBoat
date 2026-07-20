describe('Granite Skin mob skill',function()
 it('uses TP-scaled Defense Boost, forwards its message, and sets subpower only when the buff exists',function()
  local skin=require('scripts/actions/mobskills/granite_skin');local duration,buff=xi.mobskills.calculateDuration,xi.mobskills.mobBuffMove;local args,message,subpower=nil,nil,nil;local effect={setSubPower=function(_,v)subpower=v end};local mob={getStatusEffect=function(_,id)assert(id==xi.effect.DEFENSE_BOOST);return effect end};local skill={getTP=function()return 1500 end,setMsg=function(_,v)message=v end};xi.mobskills.calculateDuration=function(tp,min,max)assert(tp==1500 and min==60 and max==90);return 75 end;xi.mobskills.mobBuffMove=function(...)args={...};return 456 end
  assert(skin.onMobSkillCheck({},mob,skill)==0 and skin.onMobWeaponSkill(mob,{},skill,{})==xi.effect.DEFENSE_BOOST);assert(args[1]==mob and args[2]==xi.effect.DEFENSE_BOOST and args[3]==0 and args[4]==0 and args[5]==75 and message==456 and subpower==90)
  subpower=nil;mob.getStatusEffect=function()return nil end;skin.onMobWeaponSkill(mob,{},skill,{})
  xi.mobskills.calculateDuration,xi.mobskills.mobBuffMove=duration,buff;assert(subpower==nil)
 end)
end)
