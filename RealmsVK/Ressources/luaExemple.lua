{
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
}

CombatSystem = {
	update = function ( dt )
		comps = ComponentManager.getComponentType("StatsComponent")
		for i = 1, #comps do
			c = comps[i]
			c.health = math.min(c.health + c.health_regen, c.health_max)
			c.mana = math.min(c.mana + c.mana_regen, c.mana_max)
		end
	end
}

