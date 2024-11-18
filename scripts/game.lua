local speed = 10

function ready()
	workspace.test:SetPosition(vec3.new(0, 0, 0))
	workspace.test:SetScale(vec2.new(2, 2))
	workspace.test:SetRotation(0)

	engine.renderer.manualResize = false
end

function update()
	local vel = vec3.new(0, 0, 0)

	if input.IsKeyDown("w") then
		vel.y = speed
	end
	if input.IsKeyDown("s") then
		vel.y = -speed
	end
	if input.IsKeyDown("a") then
		vel.x = -speed
	end
	if input.IsKeyDown("d") then
		vel.x = speed
	end

	workspace.test:Translate(vel)
end

function draw()
	ImGui.Begin("Game tools")

	text = utils.GetVSync() and "Turn off VSync" or "Turn on VSync"
	if ImGui.Button(text) then
		utils.ToggleVSync()
	end

	speed = ImGui.SliderInt("Speed", speed, 1, 100)

	ImGui.End()
end
