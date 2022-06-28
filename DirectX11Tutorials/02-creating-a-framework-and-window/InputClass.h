#pragma once

#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();

	void KeyDown(unsigned);
	void KeyUp(unsigned);

	bool IsKeyDown(unsigned);

private:
	bool KeyStatesAsDown[256];
};

#endif