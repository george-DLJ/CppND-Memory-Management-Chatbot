#include "graphedge.h"
#include "graphnode.h"

GraphNode::GraphNode(int id)
{
    _id = id;
}

GraphNode::~GraphNode()
{}

void GraphNode::AddToken(std::string token)
{
    _answers.push_back(token);
}

void GraphNode::AddEdgeToParentNode(GraphEdge *edge)
{
    _parentEdges.push_back(edge);
}

/**
 * NOTE: Grpahnode takes ownership of incomming edges 
 * NOTICE: the unique_ptr is moved twice: 
 *  - once from caller to function and 
 *  - once inside the function to vector
 *  for detailed answer see: AddEdgeToChildNode 
 */
void GraphNode::AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge) //function takes ownoership of the object
{
    //_childEdges.push_back(edge);
    _childEdges.emplace_back(std::move(edge));
}

void GraphNode::MoveChatbotHere(ChatBot chatBot)
{
    _chatBot = std::move(chatBot);
    _chatBot.SetCurrentNode(this);
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    newNode->MoveChatbotHere(std::move(_chatBot));
}

GraphEdge *GraphNode::GetChildEdgeAtIndex(int index)
{
    return _childEdges[index].get();
}