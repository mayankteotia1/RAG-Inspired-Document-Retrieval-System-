#pragma once

#include <bits/stdc++.h>
using namespace std;

// rate limiter with 3 algorithms, pick one when creating the object
// TOKEN_BUCKET -> capacity = max tokens, rate = tokens added per sec
// LEAKY_BUCKET -> capacity = max level, rate = leak rate per sec
// FIXED_WINDOW -> capacity = max requests per window, rate = window size in sec

enum class Algorithm
{
    TOKEN_BUCKET,
    LEAKY_BUCKET,
    FIXED_WINDOW
};

class RateLimiter
{
public:
    RateLimiter(Algorithm algo, double capacity, double rate)
        : algo(algo), capacity(capacity), rate(rate)
    {
    }

    bool allowRequest(const string &userId)
    {
        if (algo == Algorithm::TOKEN_BUCKET)
            return allowTokenBucket(userId);
        if (algo == Algorithm::LEAKY_BUCKET)
            return allowLeakyBucket(userId);
        return allowFixedWindow(userId);
    }

private:
    Algorithm algo;
    double capacity;
    double rate;

    // bucket fills up to capacity over time, each request costs 1 token
    // allows bursts up to capacity, then slows down
    struct TokenBucketState
    {
        double tokens;
        chrono::steady_clock::time_point lastRefillTime;
    };
    unordered_map<string, TokenBucketState> tokenBuckets;

    bool allowTokenBucket(const string &userId)
    {
        auto now = chrono::steady_clock::now();

        if (tokenBuckets.find(userId) == tokenBuckets.end())
            tokenBuckets[userId] = {capacity, now};

        auto &state = tokenBuckets[userId];

        double elapsed = chrono::duration<double>(now - state.lastRefillTime).count();
        state.tokens = min(capacity, state.tokens + elapsed * rate);
        state.lastRefillTime = now;

        if (state.tokens >= 1.0)
        {
            state.tokens -= 1.0;
            return true;
        }

        return false;
    }

    // water level increases by 1 per request, drains at fixed rate
    // doesn't allow bursts like token bucket does
    struct LeakyBucketState
    {
        double level;
        chrono::steady_clock::time_point lastLeakTime;
    };
    unordered_map<string, LeakyBucketState> leakyBuckets;

    bool allowLeakyBucket(const string &userId)
    {
        auto now = chrono::steady_clock::now();

        if (leakyBuckets.find(userId) == leakyBuckets.end())
            leakyBuckets[userId] = {0.0, now};

        auto &state = leakyBuckets[userId];

        double elapsed = chrono::duration<double>(now - state.lastLeakTime).count();
        state.level = max(0.0, state.level - elapsed * rate);
        state.lastLeakTime = now;

        if (state.level + 1.0 <= capacity)
        {
            state.level += 1.0;
            return true;
        }

        return false;
    }

    // counts requests in current window, resets when window expires
    // can allow up to 2x capacity near window boundaries
    struct FixedWindowState
    {
        int count;
        chrono::steady_clock::time_point windowStart;
    };
    unordered_map<string, FixedWindowState> fixedWindows;

    bool allowFixedWindow(const string &userId)
    {
        auto now = chrono::steady_clock::now();

        if (fixedWindows.find(userId) == fixedWindows.end())
            fixedWindows[userId] = {0, now};

        auto &state = fixedWindows[userId];

        double elapsed = chrono::duration<double>(now - state.windowStart).count();

        if (elapsed >= rate)
        {
            state.count = 0;
            state.windowStart = now;
        }

        if (state.count < (int)capacity)
        {
            state.count++;
            return true;
        }

        return false;
    }
};
