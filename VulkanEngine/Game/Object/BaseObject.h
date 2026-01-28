#pragma once
#include "pch.h"

namespace VWrapper
{
	class BaseObject
	{
	public:

		BaseObject() { std::cerr << "Initialized memory!" << std::endl; };
		~BaseObject() { std::cerr << "Released memory!" << std::endl; };
	

	private:


	};
}