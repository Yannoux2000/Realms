Prototypes = {
	StatsComponent = {
		health = 100,
		health_max = 100,
		health_regen = 1,
		
		mana = 100,
		mana_max = 100,
		mana_regen = 1,

		attack = 10,
		defence = 0
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

a = 1