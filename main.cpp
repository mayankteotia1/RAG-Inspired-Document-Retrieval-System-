#include <bits/stdc++.h>
#include "RateLimiter.h"
#include "LRUCache.h"
#include "SearchEngine.h"

using namespace std;

// flow: query -> rate limiter -> cache check -> search -> cache result -> print

int main()
{
    // try other algorithms:
    // RateLimiter rateLimiter(Algorithm::LEAKY_BUCKET, 3, 1.0);
    // RateLimiter rateLimiter(Algorithm::FIXED_WINDOW, 3, 5.0);
    RateLimiter rateLimiter(Algorithm::TOKEN_BUCKET, 3, 1.0);

    LRUCache cache(5);

    SearchEngine searchEngine;
    if (!searchEngine.loadDocuments("documents.txt"))
    {
        cout << "Error: could not open documents.txt" << endl;
        return 1;
    }

    cout << "Smart Document Search Engine" << endl;
    cout << "Loaded " << searchEngine.documentCount() << " documents." << endl;
    cout << "Type 'exit' to quit.\n" << endl;

    string userId = "user1";

    while (true)
    {
        cout << "Enter query: ";
        string query;
        getline(cin, query);

        if (query == "exit")
            break;

        if (query.empty())
            continue;

        if (!rateLimiter.allowRequest(userId))
        {
            cout << "Rate limit exceeded. Please wait before sending more requests.\n" << endl;
            continue;
        }

        string cachedResult;
        if (cache.get(query, cachedResult))
        {
            cout << "[CACHE HIT] " << cachedResult << "\n" << endl;
            continue;
        }

        auto result = searchEngine.search(query);
        string bestDoc = result.first;
        int score = result.second;

        string finalResult;
        if (score <= 0)
            finalResult = "No relevant document found.";
        else
            finalResult = bestDoc;

        cache.put(query, finalResult);

        cout << "[CACHE MISS] " << finalResult << "\n" << endl;
    }

    cout << "Goodbye!" << endl;
    return 0;
}
