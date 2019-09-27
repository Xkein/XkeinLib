#pragma once

#include "DataStruct.h"

XKEINNAMESPACE_START
	template<class _Ty>
	class Node;

	template<class _Ty>
	class Edge;

	template<class _Ty>
	class Graph;

	template<class _Ty>
	class DirectedEdge;

	template<class _Ty>
	class UndirectedEdge;

	template<class _Ty = void*>
	using DirectedOrUndirectedEdge = UndirectedEdge<_Ty>;

	template<class _Ty>
	ABSTRACT class Base
	{
	protected:
		Base() = default;
	public:

		template <class _Ty>
		_Ty GetNext() _NOEXCEPT
		{
			return static_cast<_Ty>(next);
		}

		explicit operator _Ty&() _NOEXCEPT
		{
			return (object);
		}

		_Ty object;
		void* next;
	};

	template<class _Ty>
	ABSTRACT class Edge : public Base<_Ty>
	{
		USING_SIMPLE_NAME(Graph);
	protected:
		friend class Graph;
		Edge() = default;
	public:
		int weight;
	};

	template<class _Ty>
	class DirectedEdge : public Edge<_Ty>
	{
		USING_SIMPLE_NAME(Node);
	public:
#pragma warning(push)
#pragma warning(disable: 4201)
		union {
			struct {
				Node* from;
				Node* to;
			};
			struct {
				Node* parent;
				Node* child;
			};
		};
#pragma warning(push)
	};

	template<class _Ty>
	class UndirectedEdge : public Edge<_Ty>
	{
		USING_SIMPLE_NAME(Node);
	public:
		Node* pNode[2];
	};

	template<class _Ty>
	class Node : public Base<_Ty>
	{
		USING_SIMPLE_NAME(Edge);
		USING_SIMPLE_NAME(Graph);
	protected:
		friend class Graph;
		Node() = default;
	public:
		size_t index;
		DynamicArray<Edge*> edges;
		size_t indegree;
		size_t outdegree;
	};

	template<class _Ty = void*>
	class Ring
	{
		USING_SIMPLE_NAME(Edge);
		USING_SIMPLE_NAME(Node);
		USING_SIMPLE_NAME(DirectedOrUndirectedEdge);

	protected:
		const Edge* const* pEdges;
		size_t edgeCount;
	public:
		Ring(const Edge* const* pEdges, size_t edgeCount) _NOEXCEPT : pEdges(pEdges), edgeCount(edgeCount) {};
		Ring(const Edge* const* pEdges) _NOEXCEPT : pEdges(pEdges)
		{
			const Edge* const* pCur = pEdges + 1;
			while (*pCur != *pEdges)
				++pCur;
			edgeCount = pCur - pEdges;
		};

		bool operator==(const Ring& ring) const _NOEXCEPT
		{
			if (edgeCount != ring.edgeCount) return false;
			size_t offset = 0;
			while (*pEdges != ring.pEdges[offset]) ++offset;
			for (size_t index = 0; index < edgeCount; ++index) {
				if (pEdges[index] != ring.pEdges[(offset + index) % edgeCount])
					return false;
			}
			return true;
		}

		bool IsMe(const Edge* const* _first, const Edge* const* _end, const Edge* _last) const _NOEXCEPT
		{
			if (int(edgeCount) != _end - _first + 1) return false;
			size_t offset = 0;
			while (*_first != pEdges[offset]) {
				if (++offset >= edgeCount)
					return false;
			}
			for (size_t index = 0; index < edgeCount - 1; ++index) {
				if (_first[index] != pEdges[(offset + index) % edgeCount])
					return false;
			}
			return pEdges[(offset + edgeCount - 1) % edgeCount] == _last;
		}

		Ring GetNextRing() const _NOEXCEPT
		{
			return Ring(pEdges + edgeCount + 1);
		}

		bool IsInRing(const Node* pNode) const _NOEXCEPT
		{
			const DirectedOrUndirectedEdge* const* end = static_cast<const DirectedOrUndirectedEdge* const*>(static_cast<void*>(const_cast<Edge**>(pEdges + edgeCount)));
			for (const DirectedOrUndirectedEdge* const* pCur = static_cast<const DirectedOrUndirectedEdge* const*>(static_cast<void*>(const_cast<Edge**>(pEdges)))
				;pCur < end; ++pCur)
			{
				if ((*pCur)->pNode[0] == pNode || (*pCur)->pNode[0] == pNode)
					return true;
			}
			return false;
		}

		size_t get_edgeCount() const _NOEXCEPT
		{
			return edgeCount;
		}

		const Edge* const* get_pEdges() const _NOEXCEPT
		{
			return pEdges;
		}
	};

	template<class _Ty>
	class Graph : public Base<_Ty>
	{
		USING_SIMPLE_NAME(Edge);
		USING_SIMPLE_NAME(Node);
		USING_SIMPLE_NAME(Ring);
		USING_SIMPLE_NAME(DirectedOrUndirectedEdge);
	protected:
		struct GraphStatues
		{
		private:
			GraphStatues() _NOEXCEPT {};
			friend class Graph;
		public:
			struct {
				Node** pTopoSequence;
				DynamicArray<Edge*> ringEdgeList;
				int* pLongestPath;
				int* pShortestPath;
			} uninitialized;
			bool isForest;
			bool isConnected;
			size_t nodeCount;
			size_t edgeCount;
			size_t ringCount;
			size_t nagativeRingCount;
		};
		enum RingStatues : unsigned char
		{
			NoRing, PRingOnly, NRingOnly, PNRingBoth = PRingOnly|NRingOnly
		};

		enum GraphType : unsigned char
		{
			Directed, Undirected
		};

		Graph() = default;
	public:
		virtual ~Graph() _NOEXCEPT
		{
			for (Node* pNode : nodes)
				delete pNode;
			for (Edge* pEdge : edges)
				delete pEdge;
			nodes.Clear();
			edges.Clear();
		}

		virtual Node* CreateNode() _NOEXCEPT
		{
			Node* pNode = new Node;
			pNode->indegree = 0;
			pNode->outdegree = 0;
			pNode->index = nodes.length();
			nodes.PushBack(pNode);
			return pNode;
		}

		virtual void DestroyNode(Node* pNode) _NOEXCEPT
		{
			while (pNode->edges.length())
			{
				DirectedOrUndirectedEdge* pEdge = static_cast<DirectedOrUndirectedEdge*>(pNode->edges.back());
				GetLinkedNode(pNode, pEdge)->edges.Erase(reinterpret_cast<Edge*&>(pEdge), 1);
				pNode->edges.PopBack();
				CutEdge(pEdge, true);
			}
			nodes.Erase(pNode->index);
			for (size_t index = pNode->index; index < nodes.length(); index++)
			{
				nodes[index]->index--;
			}
			delete pNode;
		}

		static inline Node* GetLinkedNode(const Node* pNode, const Edge* pEdge) _NOEXCEPT
		{
			const DirectedOrUndirectedEdge* _pEdge = static_cast<const DirectedOrUndirectedEdge*>(pEdge);
			return _pEdge->pNode[0] != pNode ? _pEdge->pNode[0] : _pEdge->pNode[1];
		}

		virtual bool IsForest(GraphStatues* pGraphStatues = nullptr) const _NOEXCEPT
		{
			if (nodes.length() > 1)
			{
				for (Node* pNode : nodes) {
					if (pNode->indegree == 0 && pNode->outdegree == 0) {
						if (pGraphStatues)
							pGraphStatues->isForest = true;
						return true;
					}
				}
			}

			if (pGraphStatues)
				pGraphStatues->isForest = false;
			return false;
		}
		virtual RingStatues HasRing(DynamicArray<Edge*>* = nullptr, GraphStatues* = nullptr) const _NOEXCEPT = 0;

		virtual Node** TopoSort(GraphStatues* = nullptr) const _NOEXCEPT
		{
			return nullptr;
		}

		virtual Edge* LinkNode(Node*, Node*, int) _NOEXCEPT = 0;

		virtual GraphStatues* EstimateStatus(GraphStatues* pGraphStatues = nullptr, bool uninitialized = true) const _NOEXCEPT
		{
			if (pGraphStatues == nullptr)
				pGraphStatues = new GraphStatues;

			pGraphStatues->nodeCount = nodes.length();
			pGraphStatues->edgeCount = edges.length();
			pGraphStatues->isForest = IsForest();
			pGraphStatues->isConnected = IsConnected();
			if (uninitialized) {
				HasRing(nullptr, pGraphStatues);
			}
			else {
				HasRing(&pGraphStatues->uninitialized.ringEdgeList, pGraphStatues);
				LongestPath(nullptr, pGraphStatues);
				ShortestPath(nullptr, pGraphStatues);
				TopoSort(pGraphStatues);
			}

			return pGraphStatues;
		}

		virtual void CutEdge(Edge* , bool = false) _NOEXCEPT = 0;

		virtual void CutAllEdgeBetween(Node* pNode1, Node* pNode2) _NOEXCEPT
		{
			if (pNode1->edges.length() > pNode2->edges.length())
				std::swap(pNode1, pNode2);
			for (Edge* pEdge : pNode1->edges) {
				if (GetLinkedNode(pNode1, pEdge) == pNode2)
					CutEdge(pEdge);
			}
		}

		virtual int LongestPath(Node* = nullptr, GraphStatues* = nullptr) const _NOEXCEPT = 0;
		virtual int ShortestPath(Node* = nullptr, GraphStatues* = nullptr) const _NOEXCEPT = 0;

		// using abstract class that instead of function pointer can use like lambda function
		class SearchFunction {
		public:
			virtual bool operator()(Node*, bool*, void*&) = 0;
			virtual bool operator()(Node*, bool*, void*&, const Stack<Edge*>&) = 0;
		};

		// typedef bool SearchFunction(Node*, bool*, const Stack<Edge*>&, void*&);
		virtual void* DepthFirstSearch(SearchFunction*, SearchFunction*, Node* = nullptr, bool = false) const = 0;
		//virtual void DepthFirstSearch(void(*)(Node*)) = 0;
		virtual void* BreadthFirstSearch(SearchFunction*, SearchFunction*) const = 0;

		virtual bool IsConnected(GraphStatues* pGraphStatues = nullptr) const _NOEXCEPT
		{
			pGraphStatues->isConnected = false;
			return false;
		}

		virtual bool IsInRing(Node* pNode, const DynamicArray<Edge*>& ringEdgeList) const _NOEXCEPT
		{
			for (Ring ring(ringEdgeList.begin() - 1, 0);
				ring.get_pEdges() + ring.get_edgeCount() < ringEdgeList.end();)
			{
				ring = ring.GetNextRing();
				if (ring.IsInRing(pNode))
					return true;
			}
			return false;
		}

		virtual bool IsInRing(Node* pNode) const _NOEXCEPT
		{
			DynamicArray<Edge*> ringEdgeList;
			HasRing(&ringEdgeList, nullptr);
			return IsInRing(pNode, ringEdgeList);
		}

		virtual GraphType Type() const _NOEXCEPT = 0;

		const DynamicArray<Node*>& get_nodes() const _NOEXCEPT
		{
			return (nodes);
		}

		const DynamicArray<Edge*>& get_edges() const _NOEXCEPT
		{
			return (edges);
		}

		size_t get_edgeCount() const _NOEXCEPT
		{
			return edges.length();
		}

		size_t get_nodeCount() const _NOEXCEPT
		{
			return nodes.length();
		}

	protected:
		DynamicArray<Node*> nodes;
		DynamicArray<Edge*> edges;
	};

	template<class _Ty = void*>
	class DirectedGraph : public Graph<_Ty>
	{
		USING_SIMPLE_NAME(Edge);
		USING_SIMPLE_NAME(Node);
		USING_SIMPLE_NAME(DirectedOrUndirectedEdge);
		USING_SIMPLE_NAME(Graph);
		USING_SIMPLE_NAME(DirectedEdge);
		USING_SIMPLE_NAME(Ring);

	public:
		DirectedGraph() = default;
		virtual ~DirectedGraph() _NOEXCEPT
		{
			Graph::~Graph();
		}

		virtual Edge* LinkNode(Node* from, Node* to, int weight) _NOEXCEPT
		{
			if (from == to)
				return nullptr;
			DirectedEdge* pEdge = new DirectedEdge;
			pEdge->weight = weight;
			pEdge->from = from;
			pEdge->to = to;
			edges.PushBack(pEdge);
			from->outdegree++;
			to->indegree++;
			from->edges.PushBack(pEdge);
			to->edges.PushBack(pEdge);
			return pEdge;
		}

		virtual void CutEdge(Edge* pEdge, bool doNotEraseNodeEdge = false) _NOEXCEPT
		{
			static_cast<DirectedEdge*>(pEdge)->from->outdegree--;
			static_cast<DirectedEdge*>(pEdge)->to->indegree--;
			if (doNotEraseNodeEdge == false) {
				static_cast<DirectedEdge*>(pEdge)->from->edges.Erase(pEdge, 1);
				static_cast<DirectedEdge*>(pEdge)->to->edges.Erase(pEdge, 1);
			}
			delete pEdge;
			edges.Erase(pEdge, 1);
		}

		virtual Node** TopoSort(GraphStatues* pGraphStatues = nullptr) const _NOEXCEPT
		{
			if (HasRing(nullptr, pGraphStatues))
				return nullptr;
			const size_t nodeCount = nodes.length();
			size_t* pIndegree = new size_t[nodeCount];

			memset(pIndegree, 0, sizeof(size_t)*nodeCount);

			for (size_t index = 0; index < nodeCount; index++) {
				pIndegree[index] = nodes[index]->indegree;
			}

			Node** pTopoSequence = new Node*[nodeCount];
			size_t topoIndex = 0;

			for (size_t index = 0; index < nodeCount; index++) {
				if (pIndegree[index] == 0) {
					pTopoSequence[topoIndex++] = nodes[index];
				}
			}

			for (size_t index = 0; index < topoIndex; index++) {
				Node* pNode = pTopoSequence[index];
				for (Edge* pEdge : pNode->edges) {
					Node* to = static_cast<DirectedEdge*>(pEdge)->to;
					if (to != pNode) {
						if (--pIndegree[to->index] == 0) {
							pTopoSequence[topoIndex++] = to;
						}
					}
				}
			}

			if (topoIndex != nodeCount)
				return nullptr;
			if(pGraphStatues)
				pGraphStatues->uninitialized.pTopoSequence = pTopoSequence;
			return pTopoSequence;
		}

		// TOFIX
		virtual RingStatues HasRing(DynamicArray<Edge*>* pRingEdgeList = nullptr, GraphStatues* pGraphStatues = nullptr) const _NOEXCEPT
		{
			struct RingStatue
			{
				size_t ringCount{ 0 };
				size_t nagativeRingCount{ 0 };
			};

			bool IAllocated = pRingEdgeList == nullptr ? pRingEdgeList = new DynamicArray<Edge*>(), true : false;

			class RingSearchFunction : public SearchFunction
			{
			public:
				RingSearchFunction(DynamicArray<Edge*>* pRingEdgeList) _NOEXCEPT : pRingEdgeList(pRingEdgeList) {}
				virtual bool operator()(Node* pNode, bool* visited, void*& next, const Stack<Edge*>& path) _NOEXCEPT
				{
					for (Edge* pEdge : pNode->edges) {
						Node* to = static_cast<DirectedEdge*>(pEdge)->to;
						if (to != pNode && visited[to->index] == true) {
							for (Edge* pToEdge : to->edges) {
								if (static_cast<DirectedEdge*>(pToEdge)->to == to) continue;
								if (path._Get_container().Find(pToEdge, 1) != path._Get_container()._invaildPos) {
									size_t pathCount = 0;
									while (path[pathCount] != pToEdge) ++pathCount;
									size_t restring = ringStatue.ringCount;
									for (Ring ring(pRingEdgeList->begin() - 1, 0);
										restring; restring--)
									{
										ring = ring.GetNextRing();
										if (ring.IsMe(&path[pathCount], &path[0] + 1, pEdge))
											break;
									}
									if (restring) continue;
									pathCount++;
									int sum = pEdge->weight;
									for (size_t index = pathCount - 1; index != -1; --index) {
										sum += path[index]->weight;
										pRingEdgeList->PushBack(path[index]);
									}
									if (path[0] != pEdge)
										pRingEdgeList->PushBack(pEdge);
									pRingEdgeList->PushBack(pToEdge);
									sum += pToEdge->weight;
									if (sum >= 0)
										ringStatue.ringCount++;
									else
										ringStatue.nagativeRingCount++;
									break;
								}
							}
							break;
						}
					}
					return false;
				}
				virtual bool operator()(Node* pNode, bool* visited, void*& next) { throw std::runtime_error("xkein::Graph::RingSearchFunction do not support bfs"); }
				RingStatue ringStatue;
				DynamicArray<Edge*>* pRingEdgeList;
			} ringSearchFunction{ pRingEdgeList };
			pRingEdgeList->Clear();

			DepthFirstSearch(&ringSearchFunction, nullptr, nullptr, true);

			if (IAllocated)
				delete pRingEdgeList;
			RingStatues ret = NoRing;
			RingStatue& ringStatue = ringSearchFunction.ringStatue;
			if (pGraphStatues) {
				pGraphStatues->ringCount = ringStatue.ringCount + ringStatue.nagativeRingCount;
				pGraphStatues->nagativeRingCount = ringStatue.nagativeRingCount;
			}
			if (ringStatue.ringCount > 0)
				reinterpret_cast<char&>(ret) |= PRingOnly;
			if (ringStatue.nagativeRingCount > 0)
				reinterpret_cast<char&>(ret) |= NRingOnly;
			return ret;
		}

		virtual int LongestPath(Node* pStartNode = nullptr, GraphStatues* pGraphStatues = nullptr) const _NOEXCEPT
		{
			if (HasRing(nullptr, pGraphStatues) & PRingOnly) {
				if (pGraphStatues)
					pGraphStatues->uninitialized.pLongestPath = nullptr;
				return INT_MAX;
			}
			const size_t nodeCount = nodes.length();
			bool* pInQueue = new bool[nodeCount];
			memset(pInQueue, false, nodeCount);
			int* pLongestPath = new int[nodeCount];
			for (size_t index = 0; index < nodeCount; index++)
				pLongestPath[index] = INT_MIN;
			Queue<Node*> tailNodes;
			int longestPath = INT_MIN;

			if (pStartNode) {
				pLongestPath[pStartNode->index] = 0;
				pInQueue[pStartNode->index] = true;
				tailNodes.PushBack(pStartNode);

				while (tailNodes.empty() == false) {
					Node* pNode = tailNodes.front();
					pInQueue[pNode->index] = false;
					tailNodes.PopFront();
					for (Edge* pToEdge : pNode->edges) {
						Node* to = static_cast<DirectedEdge*>(pToEdge)->to;
						if (to == pNode) continue;
						int path = pLongestPath[to->index];
						for (Edge* pFromEdge : to->edges) {
							Node* from = static_cast<DirectedEdge*>(pFromEdge)->from;
							if (from != to) {
								if ((pFromEdge->weight >= 0 && pLongestPath[from->index] + pFromEdge->weight > path)
									|| (pFromEdge->weight < 0 && pLongestPath[from->index] > path - pFromEdge->weight)
									) {
									path = pLongestPath[from->index] + pFromEdge->weight;
									if (pInQueue[to->index] == false) {
										pInQueue[to->index] = true;
										tailNodes.PushBack(to);
									}
								}
							}
						}
						pLongestPath[to->index] = path;
						if (path > longestPath && to->outdegree == 0)
							longestPath = path;
					}
				}
			}
			else {
				for (size_t index = 0; index < nodeCount; index++) {
					if (nodes[index]->outdegree == 0) {
						pLongestPath[index] = 0;
						pInQueue[index] = true;
						tailNodes.PushBack(nodes[index]);
					}
				}

				while (tailNodes.empty() == false) {
					Node* pNode = tailNodes.front();
					pInQueue[pNode->index] = false;
					tailNodes.PopFront();
					for (Edge* pFromEdge : pNode->edges) {
						Node* from = static_cast<DirectedEdge*>(pFromEdge)->from;
						if (from == pNode) continue;
						int path = pLongestPath[from->index];
						for (Edge* pToEdge : from->edges) {
							Node* to = static_cast<DirectedEdge*>(pToEdge)->to;
							if (from != to) {
								if ((pToEdge->weight >= 0 && pLongestPath[to->index] + pToEdge->weight > path)
									|| (pToEdge->weight < 0 && pLongestPath[to->index] > path - pToEdge->weight)
									) {
									path = pLongestPath[to->index] + pToEdge->weight;
									if (pInQueue[from->index] == false) {
										pInQueue[from->index] = true;
										tailNodes.PushBack(from);
									}
								}
							}
						}
						pLongestPath[from->index] = path;
						if (path > longestPath && from->indegree == 0)
							longestPath = path;
					}
				}
			}

			if (pGraphStatues)
				pGraphStatues->uninitialized.pLongestPath = pLongestPath;
			else
				delete pLongestPath;

			return longestPath;
		}

		virtual int ShortestPath(Node* pStartNode = nullptr, GraphStatues* pGraphStatues = nullptr) const _NOEXCEPT
		{
			if (HasRing(nullptr, pGraphStatues) & NRingOnly) {
				if (pGraphStatues)
					pGraphStatues->uninitialized.pShortestPath = nullptr;
				return INT_MIN;
			}
			const size_t nodeCount = nodes.length();
			bool* pInQueue = new bool[nodeCount];
			memset(pInQueue, false, nodeCount);
			int* pShortestPath = new int[nodeCount];
			for (size_t index = 0; index < nodeCount; index++)
				pShortestPath[index] = INT_MAX;
			Queue<Node*> tailNodes;
			int shortestPath = INT_MAX;

			// if you don't give a start node, it will calculate from end node
			if (pStartNode) {
				pShortestPath[pStartNode->index] = 0;
				pInQueue[pStartNode->index] = true;
				tailNodes.PushBack(pStartNode);

				while (tailNodes.empty() == false) {
					Node* pNode = tailNodes.front();
					pInQueue[pNode->index] = false;
					tailNodes.PopFront();
					for (Edge* pToEdge : pNode->edges) {
						Node* to = static_cast<DirectedEdge*>(pToEdge)->to;
						if (to == pNode) continue;
						int path = pShortestPath[to->index];
						for (Edge* pFromEdge : to->edges) {
							Node* from = static_cast<DirectedEdge*>(pFromEdge)->from;
							if (from != to) {
								if ((pFromEdge->weight >= 0 && pShortestPath[from->index] < path - pFromEdge->weight)
									|| (pFromEdge->weight < 0 && pShortestPath[from->index] + pFromEdge->weight < path)
									) {
									path = pShortestPath[from->index] + pFromEdge->weight;
									if (pInQueue[to->index] == false) {
										pInQueue[to->index] = true;
										tailNodes.PushBack(to);
									}
								}
							}
						}
						pShortestPath[to->index] = path;
						if (path < shortestPath && to->outdegree == 0)
							shortestPath = path;
					}
				}
			}
			else {
				for (size_t index = 0; index < nodeCount; index++) {
					if (nodes[index]->outdegree == 0) {
						pShortestPath[index] = 0;
						pInQueue[index] = true;
						tailNodes.PushBack(nodes[index]);
					}
				}

				while (tailNodes.empty() == false) {
					Node* pNode = tailNodes.front();
					pInQueue[pNode->index] = false;
					tailNodes.PopFront();
					for (Edge* pFromEdge : pNode->edges) {
						Node* from = static_cast<DirectedEdge*>(pFromEdge)->from;
						if (from == pNode) continue;
						int path = pShortestPath[from->index];
						for (Edge* pToEdge : from->edges) {
							Node* to = static_cast<DirectedEdge*>(pToEdge)->to;
							if (from != to) {
								if ((pToEdge->weight >= 0 && pShortestPath[to->index] < path - pToEdge->weight)
									|| (pToEdge->weight < 0 && pShortestPath[to->index] + pToEdge->weight < path)
									) {
									path = pShortestPath[to->index] + pToEdge->weight;
									if (pInQueue[from->index] == false) {
										pInQueue[from->index] = true;
										tailNodes.PushBack(from);
									}
								}
							}
						}
						pShortestPath[from->index] = path;
						if (path < shortestPath && from->indegree == 0)
							shortestPath = path;
					}
				}
			}
			if (pGraphStatues)
				pGraphStatues->uninitialized.pShortestPath = pShortestPath;
			else
				delete pShortestPath;

			return shortestPath;
		}


		virtual void* DepthFirstSearch(SearchFunction* preTraverse, SearchFunction* postTraverse, Node* pStartNode = nullptr, bool unmarkWhenReturn = false) const
		{
			const size_t nodeCount = nodes.length();
			bool* visited = new bool[nodeCount];
			memset(visited, false, nodeCount);
			DynamicArray<Node*> headNodes;

			if (pStartNode)
				headNodes.PushBack(pStartNode);

			for (size_t index = 0; index < nodeCount; index++) {
				if (nodes[index]->indegree == 0)
					headNodes.PushBack(nodes[index]);
			}

			if (headNodes.empty())
				headNodes.PushBack(nodes[0]);

			class dfs {
			public:
				dfs(SearchFunction* preTraverse, SearchFunction* postTraverse, bool* visited, bool unmarkWhenReturn) _NOEXCEPT
					: preTraverse(preTraverse), postTraverse(postTraverse), visited(visited), unmarkWhenReturn(unmarkWhenReturn)
				{}
				bool operator()(Node* pNode) _NOEXCEPT {
					visited[pNode->index] = true;
					if (preTraverse && (*preTraverse)(pNode, visited, next, path))
						return true;
					for (Edge* pEdge : pNode->edges) {
						Node* to = static_cast<DirectedEdge*>(pEdge)->to;
						if (to != pNode && visited[to->index] == false) {
							path.Push(pEdge);
							if(this->operator()(to))
								return true;
							path.Pop();
						}
					}
					if(unmarkWhenReturn)
						visited[pNode->index] = false;
					if (postTraverse && (*postTraverse)(pNode, visited, next, path))
						return true;
					return false;
				};
				Stack<Edge*> path;
				bool* visited;
				SearchFunction* preTraverse;
				SearchFunction* postTraverse;
				void* next{ nullptr };
				bool unmarkWhenReturn;
			} dfs{ preTraverse, postTraverse, visited, unmarkWhenReturn };

			for (Node* pNode : headNodes) {
				if(dfs(pNode))
					break;
			}

			delete visited;
			return dfs.next;
		}

		virtual void* BreadthFirstSearch(SearchFunction* preTraverse, SearchFunction* postTraverse) const
		{
			const size_t nodeCount = nodes.length();
			bool* visited = new bool[nodeCount];
			memset(visited, false, nodeCount);
			Queue<Node*> queue;
			for (size_t index = 0; index < nodeCount; index++) {
				if (nodes[index]->indegree == 0)
					queue.PushBack(nodes[index]);
			}

			if (queue.empty())
				queue.PushBack(nodes[0]);

			void* next{ nullptr };
			while (queue.empty() == false) {
				Node* pNode = queue.front();
				queue.PopFront();
				visited[pNode->index] = true;
				(*preTraverse)(pNode, visited, next);
				for (Edge* pEdge : pNode->edges) {
					Node* to = static_cast<DirectedEdge*>(pEdge)->to;
					if (to != pNode && visited[to->index] == false) {
						queue.PushBack(to);
					}
				}
			}

			return next;
		}

		virtual GraphType Type() const _NOEXCEPT
		{
			return Directed;
		}
	};

	template<class _Ty = void*>
	class UndirectedGraph : public Graph<_Ty>
	{
		USING_SIMPLE_NAME(Edge);
		USING_SIMPLE_NAME(Node);
		USING_SIMPLE_NAME(DirectedOrUndirectedEdge);
		USING_SIMPLE_NAME(Graph);
		USING_SIMPLE_NAME(UndirectedEdge);
	public:
		UndirectedGraph() = default;
		virtual ~UndirectedGraph() _NOEXCEPT
		{
			Graph::~Graph();
		}

		virtual Edge* LinkNode(Node* pNode1, Node* pNode2, int weight) _NOEXCEPT
		{
			if (pNode1 == pNode2)
				return nullptr;
			UndirectedEdge* pEdge = new UndirectedEdge;
			pEdge->weight = weight;
			pEdge->pNode[0] = pNode1;
			pEdge->pNode[1] = pNode2;
			edges.PushBack(pEdge);
			pNode1->indegree++;
			pNode1->outdegree++;
			pNode2->indegree++;
			pNode2->outdegree++;
			pNode1->edges.PushBack(pEdge);
			pNode2->edges.PushBack(pEdge);
			return pEdge;
		}

		virtual void CutEdge(Edge* pEdge, bool doNotEraseNodeEdge = false) _NOEXCEPT
		{
			static_cast<UndirectedEdge*>(pEdge)->pNode[0]->outdegree--;
			static_cast<UndirectedEdge*>(pEdge)->pNode[0]->indegree--;
			static_cast<UndirectedEdge*>(pEdge)->pNode[1]->outdegree--;
			static_cast<UndirectedEdge*>(pEdge)->pNode[1]->indegree--;
			if (doNotEraseNodeEdge == false) {
				static_cast<UndirectedEdge*>(pEdge)->pNode[0]->edges.Erase(pEdge, 1);
				static_cast<UndirectedEdge*>(pEdge)->pNode[1]->edges.Erase(pEdge, 1);
			}
			delete pEdge;
			edges.Erase(pEdge, 1);
		}

		virtual RingStatues HasRing(DynamicArray<Node*>* pRingNodeList = nullptr, GraphStatues* pGraphStatues = nullptr) const _NOEXCEPT
		{
			struct RingStatue
			{
				size_t ringCount{ 0 };
				size_t nagativeRingCount{ 0 };
			};
			class RingSearchFunction : public SearchFunction
			{
			public:
				RingSearchFunction(DynamicArray<Node*>* pRingNodeList) _NOEXCEPT : pRingNodeList(pRingNodeList) {}
				virtual bool operator()(Node* pNode, bool* visited, void*& next, const Stack<Edge*>& path) _NOEXCEPT
				{
					for (Edge* pEdge : pNode->edges) {
						Node* linkedNode = GetLinkedNode(pNode, pEdge);
						if (visited[linkedNode->index] == true) {
							for (Edge* pToEdge : linkedNode->edges) {
								if (path[0] != pToEdge &&
									path._Get_container().Find(pToEdge, 1) != path._Get_container()._invaildPos) {
									int sum = 0;
									if (pRingNodeList)
										pRingNodeList->PushBack(pNode);
									for (size_t index = 0; path[index] != pToEdge; index++) {
										sum += path[index]->weight;
										if (pRingNodeList)
											pRingNodeList->PushBack(GetLinkedNode(pRingNodeList->back(), path[index]));
									}
									sum += pToEdge->weight;
									if (sum >= 0)
										ringStatue.ringCount++;
									else
										ringStatue.nagativeRingCount++;
									break;
								}
							}
							break;
						}
					}
					return false;
				}
				virtual bool operator()(Node* pNode, bool* visited, void*& next) { throw std::runtime_error("xkein::Graph::RingSearchFunction do not support bfs"); }
				RingStatue ringStatue;
				DynamicArray<Node*>* pRingNodeList;
			} ringSearchFunction{ pRingNodeList };
			pRingNodeList->Clear();

			DepthFirstSearch(&ringSearchFunction, nullptr, nullptr, true);
			RingStatues ret = NoRing;
			RingStatue& ringStatue = ringSearchFunction.ringStatue;
			if (pGraphStatues) {
				pGraphStatues->ringCount = ringStatue.ringCount + ringStatue.nagativeRingCount;
				pGraphStatues->nagativeRingCount = ringStatue.nagativeRingCount;
			}
			if (ringStatue.ringCount > 0)
				reinterpret_cast<char&>(ret) |= PRingOnly;
			if (ringStatue.nagativeRingCount > 0)
				reinterpret_cast<char&>(ret) |= NRingOnly;
			return ret;
		}

		virtual void* DepthFirstSearch(SearchFunction* preTraverse, SearchFunction* postTraverse, Node* pStartNode = nullptr, bool unmarkWhenReturn = false) const
		{
			const size_t nodeCount = nodes.length();
			bool* visited = new bool[nodeCount];
			bool* unvisited = new bool[nodeCount];
			memset(visited, false, nodeCount);
			memset(unvisited, true, nodeCount);

			class dfs {
			public:
				dfs(SearchFunction* preTraverse, SearchFunction* postTraverse, bool* visited, bool* unvisited, bool unmarkWhenReturn) _NOEXCEPT
					: preTraverse(preTraverse), postTraverse(postTraverse), visited(visited), unmarkWhenReturn(unmarkWhenReturn)
				{}
				bool operator()(Node* pNode) _NOEXCEPT {
					visited[pNode->index] = true;
					unvisited[pNode->index] = false;
					if(preTraverse && (*preTraverse)(pNode, visited, next, path))
						return true;
					for (Edge* pEdge : pNode->edges) {
						Node* linkedNode = GetLinkedNode(pNode, pEdge);
						if (linkedNode != pNode && visited[linkedNode->index] == false) {
							path.Push(pEdge);
							if(this->operator()(linkedNode))
								return true;
							path.Pop();
						}
					}
					if (unmarkWhenReturn)
						visited[pNode->index] = false;
					if (postTraverse && (*postTraverse)(pNode, visited, next, path))
						return true;
					return false;
				};
				Stack<Edge*> path;
				bool* visited;
				bool* unvisited;
				SearchFunction* preTraverse;
				SearchFunction* postTraverse;
				void* next{ nullptr };
				bool unmarkWhenReturn;
			} dfs{ preTraverse, postTraverse, visited, unvisited, unmarkWhenReturn };

			if (pStartNode == nullptr || dfs(pStartNode) == false) {
				size_t firstUnvisitedIndex = 0;
				while (firstUnvisitedIndex <= nodeCount) {
					if (dfs(nodes[firstUnvisitedIndex]))
						break;
					do {
						firstUnvisitedIndex++;
					} while (unvisited[firstUnvisitedIndex] == false);
				}
			}

			delete visited;
			return dfs.next;
		}

		virtual bool IsConnected(GraphStatues* pGraphStatues = nullptr) const _NOEXCEPT
		{
			class ConnectedSearchFunction : public SearchFunction {
			public:
				ConnectedSearchFunction(size_t nodeCount) _NOEXCEPT : nodeCount(nodeCount) {}
				virtual bool operator()(Node* pNode, bool* visited, void*& next, const Stack<Edge*>& path) _NOEXCEPT
				{
					if (path.empty()) {
						for (size_t index = nodeCount; index != -1; index--)
							if (visited[index] == false)
								return true;
						reinterpret_cast<bool&>(next) = true;
						return true;
					}
					return false;
				}
				virtual bool operator()(Node* pNode, bool* visited, void*& next) { throw std::runtime_error("xkein::Graph::ConnectedSearchFunction do not support bfs"); }
				size_t nodeCount;
			} connectedSearchFunction{ nodes.length() };
			bool isConnected = static_cast<bool>(DepthFirstSearch(nullptr, &connectedSearchFunction));

			pGraphStatues->isConnected = isConnected;
			return isConnected;
		}

		virtual GraphType Type() const _NOEXCEPT
		{
			return Undirected;
		}
	};

	template<class _Ty = void*>
	class Tree : protected DirectedGraph<_Ty>
	{
		USING_SIMPLE_NAME(Edge);
		USING_SIMPLE_NAME(Node);
		USING_SIMPLE_NAME(DirectedOrUndirectedEdge);
		USING_SIMPLE_NAME(Graph);

	};

	template<class _Ty = void*>
	class BinaryTree : protected Tree<_Ty>
	{
		USING_SIMPLE_NAME(Edge);
		USING_SIMPLE_NAME(Node);
		USING_SIMPLE_NAME(DirectedOrUndirectedEdge);
		USING_SIMPLE_NAME(Graph);

	};

	template<class _Ty = void*>
	class Forest final : public Graph<_Ty>
	{
		USING_SIMPLE_NAME(Edge);
		USING_SIMPLE_NAME(Node);
		USING_SIMPLE_NAME(DirectedOrUndirectedEdge);
		USING_SIMPLE_NAME(Graph);
	public:
		virtual ~Forest() _NOEXCEPT
		{
			Graph::~Graph();
			graphs.Clear();
		}

		virtual bool IsForest(GraphStatues* pGraphStatues = nullptr) const _NOEXCEPT
		{
			bool isForest = graphs.length() > 1 || graphs[0]->IsForest(pGraphStatues);
			if(pGraphStatues)
				pGraphStatues->isForest = isForest;
			return isForest;
		}
	private:
		DynamicArray<Graph*> graphs;
	};

XKEINNAMESPACE_END