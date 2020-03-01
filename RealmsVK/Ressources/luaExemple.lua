StatsComponent = {
	health = "integer",
	health_max = "integer",
	health_regen = "integer",
	
	mana = "integer",
	mana_max = "integer",
	mana_regen = "integer"

	attack = "integer",
	defence = "integer"
}

CombatSystem = {
	update = function ( dt )
		for v in ComponentManager.getComponentType("StatsComponent") do
			v.health = math.min(v.health + v.health_regen, v.health_max)
			v.mana = math.min(v.mana + v.mana_regen, v.mana_max)
		end
	end
}

