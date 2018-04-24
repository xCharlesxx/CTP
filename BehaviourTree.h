#pragma once

#include <iostream>
#include <List>

class BehaviourTree
{
public: 

	class Node
	{
	public:
		virtual int run()
		{
			//Attempt at function pointers between classes
			/*UE_LOG(LogTemp, Warning, TEXT("Leaf Run"));
			if (func != nullptr)
				return (func )();
			UE_LOG(LogTemp, Warning, TEXT("Failed Leaf Call"));*/
			return 0; 
		}
		//void AddLeaf(bool(CDecisionClass::*f)(), CDecisionClass c) { (c.*(c.*f)()); }
	private:
		/*bool(CDecisionClass::*func)() = NULL; */
	};

	class CompositeNode : public Node
	{
	private:
		std::list<Node*> children;
	public:
		const std::list<Node*>& GetChildren() const { return children; }
		void AddChild(Node* child) { children.emplace_back(child); }
	};

	//Decorator Nodes have one child which the decorator can repeat, terminate or invert the child's result
	class DecoratorNode : public Node
	{
	private:
		Node* child;
	protected: 
		Node* GetChild() const { return child; }
	public:
		void SetChild(Node* newChild) { child = newChild; }
	};

	class Selector : public CompositeNode
	{
	public:
		virtual int run() override 
		{
			//If one child succeeds run() succeeds
			for (Node* child : GetChildren())
			{
				int result = child->run(); 

				if (result != 0)
					return result; 
			}
			
			//All children failed so run() fails
			return 0;
		}
	};

	class Sequence : public CompositeNode
	{
	public:
		virtual int run() override 
		{
			//If one child fails run() fails
			for (Node* child : GetChildren()) 
				if (child->run() == 0)  
					return false;
			
			return true;  //All children suceeded
		}
	};

	class AllyState : public Node
	{
	private:
		const int* alState; 
	public:
		AllyState(const int* allystate) : alState(allystate) {}
	private:
		virtual int run() override
		{
			switch (*alState)
			{
				//Downed
			case 2:
				//Revive
				return 6;
				break;
				//Dead
			case 3:
				//Chase
				return 4;
				break; 
				//Engaged
			case 5:
				//Charge
				return 1;
				break;
				//FoundFoe
			case 7: 
				//Stalk
				return 9;
				break;
			default:
				return 0;
				break;
			}
		}
	};

	class Root : public Node 
	{
	private:
		Node * child;
		friend class BehaviourTree;
		void setChild(Node* newChild) { child = newChild; }
		virtual int run() override { return child->run(); }
	};


private:
	Root * root;

public:
	BehaviourTree() : root(new Root) {}
	void setRootChild(Node* rootChild) const { root->setChild(rootChild); }
	int run() const { return root->run(); }
};

