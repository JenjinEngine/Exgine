function update()
	-- Update rotation
	local currentRotation = workspace.test:GetRotation()
	workspace.test:SetRotation((currentRotation + 1) % 360)

	-- Print out the rotation
	print(tostring(ws.test:GetRotation()))
end
