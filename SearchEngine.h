#pragma once

#include <bits/stdc++.h>
using namespace std;

// loads documents.txt and finds the best matching document for a query
// uses an inverted index built at load time for efficient retrieval
//
// inverted index maps each word -> list of document indices that contain it
// so instead of scanning all documents per query, we only look at relevant ones
//
// example:
//   "binary" -> [0, 4, 7]
//   "search" -> [0, 2, 4]
//   "tree"   -> [0, 6]

class SearchEngine
{
public:
    bool loadDocuments(const string &filePath)
    {
        ifstream file(filePath);

        if (!file.is_open())
            return false;

        string line;
        while (getline(file, line))
        {
            if (!line.empty())
            {
                int docId = (int)documents.size();
                documents.push_back(line);
                buildIndex(line, docId);
            }
        }

        return true;
    }

    // returns {bestDoc, score}
    // only looks at documents that share at least one word with the query
    pair<string, int> search(const string &query)
    {
        unordered_set<string> queryWords = tokenize(query);

        // gather candidate document ids using inverted index
        unordered_map<int, int> scores;

        for (const string &word : queryWords)
        {
            auto it = invertedIndex.find(word);
            if (it != invertedIndex.end())
            {
                for (int docId : it->second)
                    scores[docId]++;
            }
        }

        // find highest scoring document
        string bestDoc = "";
        int bestScore = -1;

        for (auto &[docId, score] : scores)
        {
            if (score > bestScore)
            {
                bestScore = score;
                bestDoc = documents[docId];
            }
        }

        return {bestDoc, bestScore};
    }

    int documentCount() const
    {
        return (int)documents.size();
    }

private:
    vector<string> documents;
    unordered_map<string, vector<int>> invertedIndex;

    // lowercases and splits text into a set of words
    unordered_set<string> tokenize(const string &text)
    {
        unordered_set<string> words;
        string cur;

        for (char c : text)
        {
            if (isalnum((unsigned char)c))
            {
                cur += (char)tolower((unsigned char)c);
            }
            else
            {
                if (!cur.empty())
                {
                    words.insert(cur);
                    cur.clear();
                }
            }
        }

        if (!cur.empty())
            words.insert(cur);

        return words;
    }

    // called once per document at load time
    // maps every word in the document to this document's id
    void buildIndex(const string &doc, int docId)
    {
        unordered_set<string> words = tokenize(doc);

        for (const string &word : words)
            invertedIndex[word].push_back(docId);
    }
};
