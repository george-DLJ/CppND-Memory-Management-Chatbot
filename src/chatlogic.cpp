#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <tuple>
#include <algorithm>

#include "graphedge.h"
#include "graphnode.h"
#include "chatbot.h"
#include "chatlogic.h"


ChatLogic::ChatLogic()
{
    //// STUDENT CODE
    ////

    // create instance of chatbot
    //_chatBot = new ChatBot("../images/chatbot.png"); //Task5: Remove ownership relation of ChatBot

    // add pointer to chatlogic so that chatbot answers can be passed on to the GUI
    _chatBot->SetChatLogicHandle(this);

    ////
    //// EOF STUDENT CODE
}

ChatLogic::~ChatLogic()
{
    //// STUDENT CODE
    ////

    // delete chatbot instance
    delete _chatBot;

    // delete all nodes
    // REMOVE: now delete are handled by the smart pointer, on loosing scope.
    //for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it) //REMOVE
    //{ delete *it; } // REMOVE

    // delete all edges
    // REMOVE: Reason: edge elements in _edges are now smart pointers.
    // BEFORE: 
    //  - edge objects where created here: GraphEdge *edge = new Graphedge(id) 
    //  - therefore they should be delted on this class destructor.
    // NOW: 
    //  - edge are smart pointers (unique_ptr) 
    //  - For this reason, chatlogic does not need to keep track of created objects (with new)
    //    to delete them on the destructor.
    //  - In fact the delete on destructor generates an Memory error due to try of deleting
    //    freed memory!
    for (auto it = std::begin(_edges); it != std::end(_edges); ++it)
    {
        //delete *it; //is not the task of Chatlogic to delete the edges, because they
                    // are now smpart pointers.
    }

    ////
    //// EOF STUDENT CODE
}

template <typename T>
void ChatLogic::AddAllTokensToElement(std::string tokenID, tokenlist &tokens, T &element)
{
    // find all occurences for current node
    auto token = tokens.begin();
    while (true)
    {
        token = std::find_if(token, tokens.end(), [&tokenID](const std::pair<std::string, std::string> &pair) { return pair.first == tokenID;; });
        if (token != tokens.end())
        {
            element.AddToken(token->second); // add new keyword to edge
            token++;                         // increment iterator to next element
        }
        else
        {
            break; // quit infinite while-loop
        }
    }
}

void ChatLogic::LoadAnswerGraphFromFile(std::string filename)
{
    // load file with answer graph elements
    std::ifstream file(filename);

    // check for file availability and process it line by line
    if (file)
    {
        // loop over all lines in the file
        std::string lineStr;
        while (getline(file, lineStr))
        {
            // extract all tokens from current line
            tokenlist tokens;
            while (lineStr.size() > 0)
            {
                // extract next token
                int posTokenFront = lineStr.find("<");
                int posTokenBack = lineStr.find(">");
                if (posTokenFront < 0 || posTokenBack < 0)
                    break; // quit loop if no complete token has been found
                std::string tokenStr = lineStr.substr(posTokenFront + 1, posTokenBack - 1);

                // extract token type and info
                int posTokenInfo = tokenStr.find(":");
                if (posTokenInfo != std::string::npos)
                {
                    std::string tokenType = tokenStr.substr(0, posTokenInfo);
                    std::string tokenInfo = tokenStr.substr(posTokenInfo + 1, tokenStr.size() - 1);

                    // add token to vector
                    tokens.push_back(std::make_pair(tokenType, tokenInfo));
                }

                // remove token from current line
                lineStr = lineStr.substr(posTokenBack + 1, lineStr.size());
            }

            // process tokens for current line
            auto type = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "TYPE"; });
            if (type != tokens.end())
            {
                // check for id
                auto idToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "ID"; });
                if (idToken != tokens.end())
                {
                    // extract id from token
                    int id = std::stoi(idToken->second);

                    // node-based processing
                    if (type->second == "NODE")
                    {
                        //// STUDENT CODE
                        ////

                        // check if node with this ID exists already
                        //auto existingNode = std::find_if(_nodes.begin(), _nodes.end(), [&id](GraphNode *node) { return node->GetID() == id; });
                        
                        // REPLACED with equivalent for smart pointers:
                        auto existingNode = std::find_if(_nodes.begin(), _nodes.end(), [&id](const std::unique_ptr<GraphNode> &node) { return node->GetID() == id; });

                        // create new element if ID does not yet exist
                        if (existingNode == _nodes.end())
                        {
                            // add new element: 
                            // Alternative 1: emplace_back(new unique_ptr)
                            _nodes.emplace_back(std::unique_ptr<GraphNode>(new GraphNode(id)));
                            // Alternative 2: Create(unique_ptr<>), push_back(make_unique<>())
                            // 1. create unique pointer to a class: // DON'T DO THIS!!
                            // std::unique_ptr<GraphNode> newNodeElement(new GraphNode(id)); //DON'T DO THIS!!!
                            //_nodes.push_back(newNodeElement); // unique_ptr cannot be copied!
                            // Alternative 3: 
                            // 1. add it to the vector:
                            //_nodes.push_back(std::make_unique<GraphNode>(id));

                            // Set iterator to the new last element (the added node)
                            existingNode = _nodes.end() - 1; 

                            // add all answers to current node
                            AddAllTokensToElement("ANSWER", tokens, **existingNode); //(!) renamed newNode to existingNode, to avoid confusion. 
                        }

                        ////
                        //// EOF STUDENT CODE
                    }

                    // edge-based processing
                    if (type->second == "EDGE")
                    {
                        //// STUDENT CODE
                        ////
                        // see KB: https://knowledge.udacity.com/questions/353634

                        // find tokens for incoming (parent) and outgoing (child) node
                        auto parentToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "PARENT"; });
                        auto childToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "CHILD"; });

                        if (parentToken != tokens.end() && childToken != tokens.end())
                        {
                            // get iterator on incoming and outgoing node via ID search
                            auto parentNode = std::find_if(_nodes.begin(), _nodes.end(), [&parentToken](const std::unique_ptr<GraphNode> &node) { return node->GetID() == std::stoi(parentToken->second); });
                            auto childNode = std::find_if(_nodes.begin(), _nodes.end(), [&childToken](const std::unique_ptr<GraphNode> &node) { return node->GetID() == std::stoi(childToken->second); });

                            // create new edge
                            std::unique_ptr<GraphEdge> edge(new GraphEdge(id));
                            // alternative: std::unique_ptr<GraphEdge> edge = std::make_unique<GraphEdge>(id);
                            edge->SetChildNode(childNode->get());
                            edge->SetParentNode(parentNode->get());
                            _edges.push_back(edge.get()); //NOTE: this line keeps track of created edges. 
                            // But is not necessary any more after changing the edge instances  from raw pointer to 
                            // smart_pointer. 
                            // BEFORE: 
                            //  - edge objects where created here: GraphEdge *edge = new Graphedge(id) 
                            //  - therefore they should be delted on this class destructor.
                            // NOW: 
                            //  - edge are smart pointers (unique_ptr) 
                            //  - For this reason, chatlogic does not need to keep track of created objects (with new)
                            //    to delete them on the destructor.
                            //  - In fact the delete on destructor generates an Memory error/Memory Leak?

                            // find all keywords for current node
                            AddAllTokensToElement("KEYWORD", tokens, *edge);

                            // store reference in child node and parent node
                            // Task4 changes:
                            //  1- Edge is now a unique_ptr 
                            //  2- childnode gets a raw pointer through MySmartPtr.get()
                            //  3- Ownership is passed to GraphNode instance through AddEdgeChildNode function 
                            //     Therefore a std::move(myUniquePtr) is mandatory.
                            (*childNode)->AddEdgeToParentNode(edge.get()); //DONE: changed to accept raw pointer
                            (*parentNode)->AddEdgeToChildNode(std::move(edge)); //DONE: changed to move unique_ptr
                        }

                        ////
                        //// EOF STUDENT CODE
                    }
                }
                else
                {
                    std::cout << "Error: ID missing. Line is ignored!" << std::endl;
                }
            }
        } // eof loop over all lines in the file

        file.close();

    } // eof check for file availability
    else
    {
        std::cout << "File could not be opened!" << std::endl;
        return;
    }

    //// STUDENT CODE
    ////

    // identify root node
    // TODO: make rootnode to point ot a smartpointer. 

    // Original code (using raw pointers): 
    // GraphNode *rootNode = nullptr;
    // for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it)
    // {
    //     // search for nodes which have no incoming edges
    //     if ((*it)->GetNumberOfParents() == 0)
    //     {

    //         if (rootNode == nullptr)
    //         {
    //             rootNode = *it; // assign current node to root
    //         }
    //         else
    //         {
    //             std::cout << "ERROR : Multiple root nodes detected" << std::endl;
    //         }
    //     }
    // }


    // First approach: 
    //   rootNode is a (raw)pointer to a smartPointer(unique) on the vector:
    //   Ausmption: "it" is a pointer to a smartPointer
    // Question: is it secure use here the raw Pointer or should I use here a shared_ptr?
    GraphNode *rootNode = nullptr;
    for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it)
    {
        // search for nodes which have no incoming edges
        if ((*it)->GetNumberOfParents() == 0)
        {
            if (rootNode == nullptr)
            {
                rootNode = (*it).get(); // assign current node to root
            }
            else
            {
                std::cout << "ERROR : Multiple root nodes detected" << std::endl;
            }
        }
    }

    // // Second approach: 
    // //   set rootnode as Shared_ptr:
    // //   Ausmption: "it" is a pointer to a smartPointer
    // //   Question: is it secure use here the raw Pointer or should I use here a shared_ptr?
    // std::shared_ptr<GraphNode> rootNode;
    // for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it)
    // {
    //     // search for nodes which have no incoming edges
    //     if ((it)->GetNumberOfParents() == 0)
    //     {
    //         if (!rootNode)
    //         {
    //             rootNode = std::move(*it); // share the resource (vector element) with vector.
    //         }
    //         else
    //         {
    //             std::cout << "ERROR : Multiple root nodes detected" << std::endl;
    //         }
    //     }
    // }

    // Task 5: code to replace:
    // add chatbot to graph root node
    //_chatBot->SetRootNode(rootNode);  //REMOVE
    //rootNode->MoveChatbotHere(_chatBot); //REMOVE
    
    //Task 5: 
    // 1. create a local ChatBot instance on the stack
    ChatBot chatBot  = new ChatBot("../images/chatbot.png");
    
    // alternative: std::unique_ptr<ChatBot> chatBot = std::make_unique<ChatBot>("../images/chatbot.png");
    // 2. use move semantics to pass the ChatBot instance into the root node.
    // (?) It makes no sens pass a pointer to a local variable!
    //std::unique_ptr<ChatBot> chatBotptr = std::make_unique<ChatBot>(chatBot);
    //_chatBot = chatBotptr.get();
    //rootNode->MoveChatbotHere(std::make_unique<ChatBot>(chatBot)); //Alternative?
    chatBot->SetRootNode(rootNode); 
    rootNode->MoveChatbotHere(std::move(chatBot));

    // 3. The member _chatBot remains so it van be used as a communication handle
    //    what should I do with _chatBot? now contains nothing and was not initialized!


    ////
    //// EOF STUDENT CODE
}

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog)
{
    _panelDialog = panelDialog;
}

void ChatLogic::SetChatbotHandle(ChatBot *chatbot)
{
    _chatBot = chatbot;
}

void ChatLogic::SendMessageToChatbot(std::string message)
{
    _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(std::string message)
{
    _panelDialog->PrintChatbotResponse(message);
}

wxBitmap *ChatLogic::GetImageFromChatbot()
{
    return _chatBot->GetImageHandle();
}
