function draw()
	ImGui.Begin("Hierarchy")

	for i, pair in ipairs(scene:GetGameObjectsPair()) do
		ImGui.PushID(pair.name)

		if ImGui.CollapsingHeader(pair.name) then
			local gobj = pair.obj
			ImGui.Indent()

			do -- Transform
				ImGui.Text("Transform")
				ImGui.Separator()

				local pos = gobj:GetPosition()
				pos = ImGui.DragFloat2("Position", { pos.x, pos.y }, 0.1)
				gobj:SetPosition(vec3.new(pos[1], pos[2], 0))

				local scale = gobj:GetScale()
				scale = ImGui.DragFloat2("Scale", { scale.x, scale.y }, 0.1)
				gobj:SetScale(vec2.new(scale[1], scale[2]))

				local rot = gobj:GetRotation()
				gobj:SetRotation(ImGui.DragFloat("Rotation", rot, 0.1))
			end

			ImGui.Unindent()
		end

		ImGui.PopID()
	end

	ImGui.End()
end
