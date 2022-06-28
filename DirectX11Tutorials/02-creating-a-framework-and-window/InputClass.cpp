
#include "InputClass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass&)
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	for (unsigned Index = 0; Index < 256; ++Index)
	{
		KeyStatesAsDown[Index] = false;
	}
}

void InputClass::KeyDown(unsigned Index)
{
	KeyStatesAsDown[Index] = true;
}

void InputClass::KeyUp(unsigned Index)
{
	KeyStatesAsDown[Index] = false;
}

bool InputClass::IsKeyDown(unsigned Index)
{
	return KeyStatesAsDown[Index];
}
