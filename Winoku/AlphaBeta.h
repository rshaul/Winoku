#ifndef ALPHABETA_H
#define ALPHABETA_H

#include <vector>
#include <algorithm>
#include <climits>
#include <assert.h>

using namespace std;

template<class T>
class SearchNode
{
	public:
	virtual bool IsTerminal() = 0;
	virtual float ValueHeuristic() = 0;
	virtual vector<T*> Children() = 0;
};

template<class T>
struct NodeValue
{
	T node;
	float value;

	NodeValue() {
		value = -1;
	}
};

template<class T>
NodeValue<T> AlphaBetaSearch(const T *origin, int depth, float alpha, float beta, bool maxNode);

/* EXTERNAL FILES CALL THIS FUNCTION */
/* ASSUMES T inherits from SearchNode */
template<class T>
T AlphaBetaSearch(T *origin, int maxDepth) {
	NodeValue<T> alpha;
	alpha.value = -FLT_MAX;
	NodeValue<T> beta;
	beta.value = FLT_MAX;
	return AlphaBetaSearch(origin, maxDepth, alpha, beta, true).node;
}

template<class T>
NodeValue<T> AlphaBetaSearch(T *origin, int depth, NodeValue<T> alpha, NodeValue<T> beta, bool maxNode) {
	if (depth == 0 || origin->IsTerminal()) {
		NodeValue<T> r;
		r.node = *origin;
		r.value = origin->ValueHeuristic();
		return r;
	}

	vector<T*> children = origin->Children();
	assert(children.size() > 0);

	if (maxNode) {
		for (unsigned int i=0; i < children.size(); i++) {
			T *child = children[i];
			NodeValue<T> search = AlphaBetaSearch(child, depth-1, alpha, beta, false);
			if (search.value >= alpha.value) {
				alpha.node = *child;
				alpha.value = search.value;
			}
			if (beta.value <= alpha.value) break;
		}
		return alpha;
	} else {
		for (unsigned int i=0; i < children.size(); i++) {
			T *child = children[i];
			NodeValue<T> search = AlphaBetaSearch(child, depth-1, alpha, beta, true);
			if (search.value <= beta.value) {
				beta.node = *child;
				beta.value = search.value;
			}
			if (beta.value <= alpha.value) break;
		}
		return beta;
	}
}


#endif