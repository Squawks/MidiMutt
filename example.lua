--[[

#example.lua
---
The Lua interface for MidiMutt exposes the following global variables:
1. key
2. mouse
3. wait

#key
---
key.press(key)               e.g. key.press("a"), key.press("PrintScreen")
key.hold(key)                e.g. key.hold("left")
key.release(key)             e.g. key.release("left")
key.type(text, charDelay)    e.g. key.type("Bark bark, woof woof!", 5)

(see the dropdown of keys in the Output-->Key Action menu for valid names of keys - case insensitive)

#mouse
---
mouse.click(buttonID, x, y)              e.g. mouse.click(2), mouse.click(0, -425, 604)
mouse.hold(buttonID, x, y)               e.g. mouse.hold(1, 960, 5)
mouse.release(buttonID, x, y)            e.g. mouse.release(1)
mouse.scroll(direction, amount, x, y)    e.g. mouse.scroll(-1, 50), mouse.scroll(1, 7, -40, 541), 
mouse.move(dx, dy, x, y)                 e.g. mouse.move(5, 5), mouse.move(-6, 540, 966, 0)

(buttonIDs : 0=LeftButton, 1=Rightbutton, 2=MiddleButton)

#wait
---
wait(milliseconds)      e.g. wait(50), wait(1024)

--]]

-- Example:
-- Lua script that moves the mouse -5px in the X axis 15 times with a 6 millisecond delay,
-- then clicks the left mouse button, presses Ctrl+A, presses Up, and then types "Caw, caw!"
for i=1, 15 do
  mouse.move(-5, 0)
  wait(6)
end
mouse.click(0)
wait(50)
key.hold("ctrl")
key.press("a")
key.release("ctrl")
wait(300)
key.press("up")
wait(50)
key.type("Caw, caw!")