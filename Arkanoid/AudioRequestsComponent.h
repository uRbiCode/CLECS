#pragma once

#include "RingBuffer.h"
#include <string>
#include <algorithm>
#include <cstdint>

enum class AudioType : uint8_t
{
    Invalid = 0,
    Sfx,
    Music
};

struct AudioRequest
{
    AudioType Type = AudioType::Invalid;
    std::string Name;
    float Volume = 1.f;
};

/* Specialized RingBuffer for AudioRequests.
 * Prevents the same sounds to be enqueued more than once, instead chooses max volume.
 */
template<size_t Capacity>
class AudioRequestQueue : public RingBuffer<AudioRequest, Capacity>
{
public:
    bool Push(const AudioRequest& Value) override
    {
        UpdateOrInsert(Value);
        return true;
    }

    bool Push(AudioRequest&& Value) override
    {
        UpdateOrInsert(std::move(Value));
        return true;
    }

private:
    void UpdateOrInsert(const AudioRequest& Request)
    {
        size_t Index = this->Head;
        while (Index != this->Tail)
        {
            auto& Existing = this->Buffer[Index];
            if (Existing.Type == Request.Type && Existing.Name == Request.Name)
            {
                Existing.Volume = std::max(Existing.Volume, Request.Volume);
                return;
            }

            Index = this->Increment(Index);
        }

        RingBuffer<AudioRequest, Capacity>::Push(Request);
    }

    void UpdateOrInsert(AudioRequest&& Request)
    {
        size_t Index = this->Head;
        while (Index != this->Tail)
        {
            auto& Existing = this->Buffer[Index];
            if (Existing.Type == Request.Type && Existing.Name == Request.Name)
            {
                Existing.Volume = std::max(Existing.Volume, Request.Volume);
                return;
            }

            Index = this->Increment(Index);
        }

        RingBuffer<AudioRequest, Capacity>::Push(std::move(Request));
    }
};

constexpr size_t MAX_AUDIO_REQUESTS = 16;

/* Tracks requested sounds for AudioSystem to consume them.
 * Prevents playing the same sound multiple times by utilizing custom RingBuffer.
 */	
struct AudioRequestsComponent
{
    AudioRequestQueue<MAX_AUDIO_REQUESTS> Requests;
    float RequestTimer = 0.f;
};