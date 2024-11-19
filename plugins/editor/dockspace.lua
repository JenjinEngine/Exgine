function draw()
	local dockspace_flags = ImGui.DockNodeFlags_PassthruCentralNode
	local window_flags = ImGuiWindowFlags.MenuBar | ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoCollapse |
		ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoBringToFrontOnFocus |
		ImGuiWindowFlags.NoNavFocus

	local viewport = ImGui.GetMainViewport()
	if viewport == nil then
		return
	end

	print("(" .. tostring(viewport.Pos.x) .. ", " .. tostring(viewport.Pos.y) .. ")")
	print("(" .. tostring(viewport.Size.x) .. ", " .. tostring(viewport.Size.y) .. ")")

	ImGui.SetNextWindowPos(viewport.Pos.x, viewport.Pos.y, ImGuiCond.Always)
	ImGui.SetNextWindowSize(viewport.Size.x, viewport.Size.y, ImGuiCond.Always)
	ImGui.SetNextWindowViewport(viewport.ID)

	ImGui.PushStyleVar(ImGuiStyleVar.WindowRounding, 0.0)
	ImGui.PushStyleVar(ImGuiStyleVar.WindowBorderSize, 0.0)
	ImGui.PushStyleVar(ImGuiStyleVar.WindowPadding, 0.0, 0.0)

	ImGui.Begin("Viewport", true, window_flags)
	ImGui.PopStyleVar(3)

	local dockspace_id = math.floor(ImGui.GetID("MyDockSpace"))

	ImGui.DockSpace(dockspace_id, math.floor(viewport.Size.x), math.floor(viewport.Size.y),
		ImGuiDockNodeFlags.DockSpace | ImGuiDockNodeFlags.PassthruCentralNode)

	if globals.firstTime then
		globals.firstTime = false
		warn("Running first time")

		ImGui.DockBuilderDockWindow("Viewport", dockspace_id)

		local dock_left, dock_right = ImGui.DockBuilderSplitNode(dockspace_id, ImGuiDir.Left, 0.5)
		local dock_left_bottom, dock_right_bottom = ImGui.DockBuilderSplitNode(dock_left, ImGuiDir.Down, 0.5)

		ImGui.DockBuilderDockWindow("Hierarchy", dock_right)
		ImGui.DockBuilderDockWindow("Game tools", dock_right_bottom)
		ImGui.DockBuilderFinish(dockspace_id)
	end

	ImGui.End()
end

