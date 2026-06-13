# Smart Document Search Engine

A C++17 backend project simulating the retrieval step of a RAG (Retrieval-Augmented Generation) pipeline. A user query passes through rate limiting, an LRU cache, and a keyword-based search engine over a local knowledge base.

## Structure

* `RateLimiter.h` — Rate limiter supporting Token Bucket, Leaky Bucket, and Fixed Window
* `LRUCache.h` — LRU cache built using `unordered_map` + `list`
* `SearchEngine.h` — Loads `documents.txt`, scores documents
* `documents.txt` — Local knowledge base (one document per line)
* `main.cpp` — CLI entry point

## Flow

```
Query -> Rate Limiter -> Cache Check -> Search Engine -> Best Match -> Cache Result -> Response
```

## Features

* Three rate limiting algorithms: Token Bucket, Leaky Bucket, Fixed Window (selectable via enum)
* LRU cache with O(1) average get/put
* Inverted index built at load time for efficient retrieval (maps words to document IDs, inspired by real search engines like Elasticsearch)

## Prerequisites

* C++17 compiler (g++ 9+)

## Building

```
g++ -std=c++17 -O2 -Wall -o search_engine main.cpp
./search_engine
```

## Sample Output

```
Enter query: binary search tree
[CACHE MISS] A binary search tree maintains the property that left children
are smaller and right children are larger than the parent node

Enter query: binary search tree
[CACHE HIT] A binary search tree maintains the property that left children
are smaller and right children are larger than the parent node
```

## Switching Rate Limiting Algorithm

In `main.cpp`, change the constructor:

```cpp
RateLimiter rateLimiter(Algorithm::TOKEN_BUCKET, 3, 1.0);
RateLimiter rateLimiter(Algorithm::LEAKY_BUCKET, 3, 1.0);
RateLimiter rateLimiter(Algorithm::FIXED_WINDOW, 3, 5.0);
```

## Future Improvements

* TF-IDF based scoring
* Multi-user support with thread safety (mutex)
* Inverted index for faster search
