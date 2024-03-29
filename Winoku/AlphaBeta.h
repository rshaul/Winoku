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
float AlphaBetaN(T *origin, int depth, bool maxNode) {
	return AlphaBetaN(origin, depth, -FLT_MAX, FLT_MAX, maxNode);
}

static int boardsExamined = 0;

template<class T>
float AlphaBetaN(T *origin, int depth, float alpha, float beta, bool maxNode) {
	if (depth == 0 || origin->IsTerminal()) {
		boardsExamined++;
		return origin->ValueHeuristic();
	}

	vector<T*> children = origin->Children();
	assert(children.size() > 0);

	if (maxNode) {
		for (unsigned int i=0; i < children.size(); i++) {
			T *child = children[i];
			float search = AlphaBetaN(child, depth-1, alpha, beta, false);
			if (search > alpha) {
				alpha = search;
			}
			if (beta <= alpha) break;
		}
		return alpha;
	} else {
		for (unsigned int i=0; i < children.size(); i++) {
			T *child = children[i];
			float search = AlphaBetaN(child, depth-1, alpha, beta, true);
			if (search < beta) {
				beta = search;
			}
			if (beta <= alpha) break;
		}
		return beta;
	}
}


#endif