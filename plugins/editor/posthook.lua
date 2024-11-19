PRIORITY = -100 -- We want postHook to run after preHook, so we give it a negative PRIORITY

function postDraw()
	engine.framebuffer:Unbind()
end

function draw()
	ImGui.Begin("Viewport")
	globals.viewportSize = ImGui.ContentRegion()

	ImGui.Image(engine.framebuffer.texture, ImGui.ImVec2(globals.viewportSize.x, globals.viewportSize.y))

	ImGui.End()
end
