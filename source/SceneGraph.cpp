#include "pch.h"
#include "SceneGraph.h"

void SceneGraph::AddObjectToGraph(Primitive* shape)
{
	m_Shapes.push_back(shape);
}

Primitive* SceneGraph::GetShape(const int& index)
{
	return m_Shapes[index];
}

const int SceneGraph::GetShapeCount()
{
	return (int(m_Shapes.size()));
}

SceneGraph::~SceneGraph()
{
	for (int i = 0; i < m_Shapes.size(); i++)
	{
		delete m_Shapes[i];
	}
}


