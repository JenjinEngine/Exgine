-- function update()
-- 	workspace.test:Rotate(5)
--
-- 	gobjs = scene:GetGameObjectsPair()
--
-- 	for i, pair in ipairs(gobjs) do
-- 		print(pair.obj:SetPosition(pair.obj:GetPosition() + vec3.new(0, 0, 0.01)))
-- 	end
-- end

function draw()
	-- ImGui.SetNextWindowPos(32, 180)
	-- ImGui.SetNextWindowSize(180, 90)

	ImGui.ShowDemoWindow(nil)

	ImGui.Begin("Hierarchy")

	for i, pair in ipairs(scene:GetGameObjectsPair()) do
		ImGui.Text(pair.name)

		ImGui.Indent()

		pos = pair.obj:GetPosition()
		pos.x = ImGui.DragFloat("X", pos.x, 0.01)
		pos.y = ImGui.DragFloat("Y", pos.y, 0.01)
		pos.z = ImGui.DragFloat("Z", pos.z, 0.01)
		pair.obj:SetPosition(pos)

		ImGui.Unindent()
	end

	ImGui.End()
end
