PRIORITY = -100 -- We want postHook to run after preHook, so we give it a negative PRIORITY

function postDraw()
	engine.framebuffer:Unbind()
end

function draw()
	ImGui.Begin("Viewport")

	local windowSize = ImGui.ContentRegion()

	engine.renderer.width = math.floor(windowSize.x)
	engine.renderer.height = math.floor(windowSize.y)
	engine.framebuffer:Resize(engine.renderer.width, engine.renderer.height)

	ImGui.Image(engine.framebuffer.texture, ImGui.ImVec2(windowSize.x, windowSize.y))

	ImGui.End()
end
