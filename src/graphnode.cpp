#include "graphedge.h"
#include "graphnode.h"

GraphNode::GraphNode(int id)
{
    _id = id;
}

GraphNode::~GraphNode()
{
    //// STUDENT CODE
    ////

    //delete _chatBot; //TASK0. Bug: this delete _chatBot shouldn't be here. 
                       // Problem: chatbot is deleted twice: 
                       //  - once from chatlogic, which has created it using new --> this is correct
                       //  - once here on GraphNode, which has become it as reference. --> therefore this is wrong
                       // GraphNode does not creates a new  chatBot, it just references
                       // one, therefore it should not delete it. 

    ////
    //// EOF STUDENT CODE
}

void GraphNode::AddToken(std::string token)
{
    _answers.push_back(token);
}

void GraphNode::AddEdgeToParentNode(GraphEdge *edge)
{
    _parentEdges.push_back(edge);
}

//void GraphNode::AddEdgeToChildNode(GraphEdge *edge)
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

//// STUDENT CODE
////
//void GraphNode::MoveChatbotHere(ChatBot *chatbot)
void GraphNode::MoveChatbotHere(ChatBot chatBot)
{
    _chatBot = std::move(chatBot);
    _chatBot.SetCurrentNode(this);
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    newNode->MoveChatbotHere(std::move(_chatBot));
    //_chatBot = nullptr; // invalidate pointer at source //Task5: REMOVED it is not a pointer any more!
}
////
//// EOF STUDENT CODE

GraphEdge *GraphNode::GetChildEdgeAtIndex(int index)
{
    //// STUDENT CODE
    ////

    return _childEdges[index].get();// return raw pointer to owned object(unique_ptr)

    ////
    //// EOF STUDENT CODE
}