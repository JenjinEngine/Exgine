PRIORITY = 100

function ready()
	engine.renderer.manualResize = true
end

function preDraw()
	if not globals.viewportSize then
		return
	end

	local width, height = math.floor(globals.viewportSize.x), math.floor(globals.viewportSize.y)

	-- Resize the renderer
	engine.renderer.width = width
	engine.renderer.height = height

	-- Resize the framebuffer
	engine.framebuffer:Resize(width, height)

	-- Bind the framebuffer
	engine.framebuffer:Bind()

	-- Clear
	utils.ClearColor(0.0, 0.0, 0.0, 1.0)
	utils.Clear()
end
