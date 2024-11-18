PRIORITY = 100

function ready()
	engine.renderer.manualResize = true
end

function preDraw()
	-- Bind the framebuffer
	engine.framebuffer:Bind()

	-- Clear
	utils.clearColor(0.0, 0.0, 0.0, 1.0)
	utils.clear()
end
