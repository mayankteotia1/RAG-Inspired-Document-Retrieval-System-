#pragma once

#include <bits/stdc++.h>
using namespace std;

// loads documents.txt and finds the best matching document for a query
// scoring = how many words overlap between query and document

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
                documents.push_back(line);
        }

        return true;
    }

    // returns {bestDoc, score}
    pair<string, int> search(const string &query)
    {
        unordered_set<string> queryWords = tokenize(query);

        string bestDoc = "";
        int bestScore = -1;

        for (const string &doc : documents)
        {
            int score = scoreDocument(doc, queryWords);

            if (score > bestScore)
            {
                bestScore = score;
                bestDoc = doc;
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

    // counts how many words from queryWords appear in doc
    int scoreDocument(const string &doc, const unordered_set<string> &queryWords)
    {
        unordered_set<string> docWords = tokenize(doc);

        int score = 0;
        for (const string &word : docWords)
        {
            if (queryWords.count(word))
                score++;
        }

        return score;
    }
};
