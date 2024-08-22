#pragma once
#include "Primitive.h"
#include <vector>


class SceneGraph
{
public:
	SceneGraph() {};
	void AddObjectToGraph(Primitive* shape);
	Primitive* GetShape(const int& index);
	
	const int GetShapeCount();
	~SceneGraph();
protected:
	
private:
	std::vector<Primitive*> m_Shapes;
	SceneGraph(SceneGraph const&) = delete;
	void operator=(SceneGraph const&) = delete;
};