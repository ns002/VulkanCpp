#pragma once
#include "pch.h"
namespace VWrapper
{
	struct Vertex {
	private:
		Vertex& operator=(const Vertex&) = delete;	//will prevent an initialized varible from getting altered
	public:

		glm::vec2 pos {};
		glm::vec3 color {};
	};

	//ideally this isn't hard coded and static lol and 'global data' bad
	static std::vector<uint16_t> indices = {
		0, 1, 2, 0, 2, 3
	};
	static std::vector<Vertex> vertices = {
		{{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},	//UL red
		{{1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},	//UR green
		{{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},		//BR blue
		{{-1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}		//BL white
	};  //might later want to seperate vertices from this class
}