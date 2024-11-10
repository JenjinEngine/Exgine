# Exgine
This is an experimental game engine that is simply used to test out new workflows.

## API rules
- All API functions/methods are in PascalCase.
- The verb `Draw` should be used over `Render` for functions that draw to the screen.
- All APIs that take in strings should be sanitized if accessible from Lua.
- All APIs that take strings should be `const std::string&` to allow Lua usage and R-Value strings.
- Every part of the engine should allow adaptation at a later stage.
- Engine renderring APIs should *never* manage the window state, e.g. clearing the screen, swapping buffers, polling events, etc.
- References are to be used over pointers where possible.
- All pointers should be `std::shared_ptr` unless there is a good reason not to.
- Fields should be camelCase.
- GLAD and the Window must be setup externally to the engine.
