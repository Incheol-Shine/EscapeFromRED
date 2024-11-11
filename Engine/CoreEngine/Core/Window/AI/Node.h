#pragma once
#include <unordered_map>
#include <vector>
#include "common_include.h"

enum class ENodeState: uint8_t
{
    Success,
    Failure,
    running
};

struct Node : public std::enable_shared_from_this<Node>
{
public:
    Node();
    Node(JText InNodeName);
    Node(JText InNodeName, const std::vector<NodeSPtr>& InChildNodes);
    virtual ~Node();

public:
    virtual ENodeState Evaluate();

public:
    void SetNodes(const std::vector<NodeSPtr>& InChildNodes);

    void Attach(NodeSPtr InNode);

    void SetData(const JText& InName, UObject* Value);


    UObject* GetData(const JText& InKey);
    bool ClearData(const JText& InKey);

    NodeSPtr GetRoot();

public:
    AActor*                             Owner;
    JText                               NodeName;
    NodeWPtr                            ParentNode;
    ENodeState                          NodeState = ENodeState::Failure;
    std::vector<NodeSPtr>               ChildNodes;
    std::unordered_map<JText, UObject*> DataContext;

};
