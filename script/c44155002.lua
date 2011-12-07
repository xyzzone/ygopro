--魔轟神獣ユニコール
function c44155002.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,aux.FilterBoolFunction(Card.IsSetCard,0x35),aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--disable and destroy
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_CHAIN_ACTIVATING)
	e1:SetOperation(c44155002.disop)
	c:RegisterEffect(e1)
end
function c44155002.disop(e,tp,eg,ep,ev,re,r,rp)
	if ep==tp or Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)~=Duel.GetFieldGroupCount(tp,0,LOCATION_HAND) then return end
	local loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	local rc=re:GetHandler()
	if Duel.GetChainInfo(ev,CHAININFO_TYPE)==TYPE_MONSTER or re:IsHasType(EFFECT_TYPE_ACTIVATE) then
		Duel.NegateEffect(ev)
		if not rc:IsLocation(0x31) then
			Duel.Destroy(rc,REASON_EFFECT)
		end
	end
end