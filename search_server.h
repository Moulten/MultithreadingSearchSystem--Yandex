#pragma once



#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <string_view>
#include <future>
#include <mutex>
using namespace std;

template <typename T>
class Synchronized {
public:
	explicit Synchronized(T initial = T())
		: value(move(initial))
	{
	}

	struct Access {
		lock_guard<mutex> guard;
		T& ref_to_value;
	};

	Access GetAccess() {
		return { lock_guard<mutex>(m), value };
	}
private:
	T value;
	mutex m;
};

class InvertedIndex {
public:
	struct Results {
		size_t count;
		size_t docid;
	};

	InvertedIndex() = default;
	explicit InvertedIndex(istream& document_input);
	const vector<Results>& Lookup(string_view word) const;

	const deque<string>& GetDocuments() const {
		return docs;
	}
private:
	map<string_view, vector<Results>> index;
	deque<string> docs;
};

class SearchServer {
public:
	SearchServer() = default;
	explicit SearchServer(istream& document_input)
		: index(InvertedIndex(document_input))
	{
	}

	void UpdateDocumentBase(istream& document_input);
	void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
	Synchronized<InvertedIndex> index;
	vector<future<void>> futures;
};
