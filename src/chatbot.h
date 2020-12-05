#ifndef CHATBOT_H_
#define CHATBOT_H_

#include <wx/bitmap.h>
#include <string>

class GraphNode; // forward declaration
class ChatLogic; // forward declaration

class ChatBot
{
private:
    // data handles (owned)
    wxBitmap *_image; // avatar image

    // data handles (not owned)
    GraphNode *_currentNode;
    GraphNode *_rootNode;
    ChatLogic *_chatLogic;

    // proprietary functions
    int ComputeLevenshteinDistance(std::string s1, std::string s2);

public:
    // constructors / destructors
    ChatBot();                     // constructor WITHOUT memory allocation
    ChatBot(std::string filename); // constructor WITH memory allocation
    ~ChatBot();     // Rule of Three/Five 1. Destructor 

    //// STUDENT CODE
    ////
    // Rule of Three: implement destructor, copy constructor or copy assignment operator:
    ChatBot(const ChatBot &source); // Rule of Three 2: copy constructor
    ChatBot &operator=(const ChatBot &source);     // Rule of Three 3: copy-asignment Operator  
    // Rule of five: rule of three + implement move assigment operator,  
    ChatBot(ChatBot &&source);  // Rule of Five 4: Move Constructor
    ChatBot &operator=(ChatBot &&source);  // Rule of Five 5: Move Assignment Operator

    
    ////
    //// EOF STUDENT CODE

    // getters / setters
    void SetCurrentNode(GraphNode *node);
    void SetRootNode(GraphNode *rootNode) { _rootNode = rootNode; }
    void SetChatLogicHandle(ChatLogic *chatLogic) { _chatLogic = chatLogic; }
    wxBitmap *GetImageHandle() { return _image; }

    // communication
    void ReceiveMessageFromUser(std::string message);
};

#endif /* CHATBOT_H_ */