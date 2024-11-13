function ready()
	workspace.test:SetPosition(vec3.new(0, 0, 0))
	workspace.test:SetScale(vec2.new(2, 2))
	workspace.test:SetRotation(0)
end

function update()
	local x = math.cos(-utils.GetTime() * 3) * 7
	local y = math.sin(-utils.GetTime() * 3) * 7

	workspace.test:SetPosition(vec3.new(x, y, 0))
	workspace.test:SetRotation(utils.GetTime() * 360)
end

function draw()
	ImGui.Begin("Game tools")

	text = utils.GetVSync() and "Turn off VSync" or "Turn on VSync"
	if ImGui.Button(text) then
		utils.ToggleVSync()
	end

	ImGui.End()
end
