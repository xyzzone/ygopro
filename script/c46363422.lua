--熟練の白魔導師
function c46363422.initial_effect(c)
	c:EnableCounterPermit(0x3001)
	c:SetCounterLimit(0x3001,3)
	--add counter
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e1:SetCode(EVENT_CHAIN_SOLVING)
	e1:SetProperty(EFFECT_FLAG_DELAY)
	e1:SetRange(LOCATION_MZONE)
	e1:SetOperation(c46363422.acop)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(46363422,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCost(c46363422.spcost)
	e2:SetTarget(c46363422.sptg)
	e2:SetOperation(c46363422.spop)
	c:RegisterEffect(e2)
end
function c46363422.acop(e,tp,eg,ep,ev,re,r,rp)
	if re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:IsActiveType(TYPE_SPELL) then
		e:GetHandler():AddCounter(0x3001,1)
	end
end
function c46363422.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetCounter(0x3001)==3 and e:GetHandler():IsReleaseable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c46363422.filter(c,e,tp)
	return c:IsCode(78193831) and not c:IsHasEffect(EFFECT_NECRO_VALLEY) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c46363422.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c46363422.filter,tp,0x13,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,0,tp,0x13)
end
function c46363422.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c46363422.filter,tp,0x13,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
