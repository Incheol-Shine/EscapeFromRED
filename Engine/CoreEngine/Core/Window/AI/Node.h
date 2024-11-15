﻿#pragma once
#include "common_include.h"

enum class NodeStatus
{
    Success,
    Failure,
    Running
};

class Node
{
public:
    virtual ~Node()
    {
    };
    virtual NodeStatus tick() = 0;

    virtual void addChild(Ptr<Node> child)
    {
    };
};

class ActionNode : public Node
{
public:
    ActionNode(std::function<NodeStatus()> action) : action(action)
    {
    };

    NodeStatus tick() override
    {
        return action();
    }

private:
    std::function<NodeStatus()> action;
};

class Selector : public Node
{
public:
    JText NodeName;

public:
    Selector();

    Selector(const JText& name)
    {
        NodeName = name;
    }

    void addChild(Ptr<Node> child)
    {
        children.push_back(child);
    }

    NodeStatus tick() override
    {
        for (auto& child : children)
        {
            NodeStatus status = child->tick();
            if (status != NodeStatus::Failure)
                return status;
        }
        return NodeStatus::Failure;
    }

private:
    std::vector<Ptr<Node>> children;
};

class Sequence : public Node
{
public:
    JText NodeName;

public:
    Sequence();

    Sequence(const JText& name)
    {
        NodeName = name;
    }

    void addChild(Ptr<Node> child)
    {
        children.push_back(child);
    }

    NodeStatus tick() override
    {
        for (auto& child : children)
        {
            NodeStatus status = child->tick();
            if (status != NodeStatus::Success)
                return status;
        }
        return NodeStatus::Success;
    }

private:
    std::vector<Ptr<Node>> children;
};
