#pragma once
#include "QueryContext.h"
#include "ArchetypeStorage.h"
#include <tuple>
#include <utility>
#include <vector>

template<typename T, typename... Types>
concept OverlapsTypes = (std::same_as<T, Types> || ...);

template<typename... Ts> struct WritesList {};
template<typename... Ts> struct ReadsList  {};

template<typename WritesTag, typename ReadsTag>
class Query;

// TODO: CHECK UNIQUE TYPES PER WRITE/READ TAG
template<typename... WriteTypes, typename... ReadTypes>
requires (!OverlapsTypes<WriteTypes, ReadTypes...> && ...) 
class Query<WritesList<WriteTypes...>, ReadsList<ReadTypes...>>
{
public:
    Query(QueryContext& Context)
    {
        for (auto& Handle : Context.Archetypes->template GetArchetypesWithComponents<WriteTypes..., ReadTypes...>())
        {
            CacheHandle(Handle);
        }
    }

    template<typename Func>
    void ForEach(Func&& Function)
    {
        for (auto& Match : MatchedArchetypes)
        {
            const size_t N = Match.EntityCount;
            const Entity* Entities = Match.Entities;

            for (size_t i = 0; i < N; ++i)
            {
                CallWithIndex(std::forward<Func>(Function), Entities[i], i, Match,
                              std::index_sequence_for<WriteTypes...>{},
                              std::index_sequence_for<ReadTypes...>{});
            }
        }
    }

private:
    struct MatchedArchetype
    {
        const Entity* Entities = nullptr;
        size_t EntityCount = 0;
        std::tuple<WriteTypes*...> WritePtrs;
        std::tuple<ReadTypes*...>  ReadPtrs;
    };

    void CacheHandle(ArchetypeHandle<WriteTypes..., ReadTypes...>& Handle)
    {
        MatchedArchetype Match{};
        Match.Entities = Handle.GetEntities().data();
        Match.EntityCount = Handle.Size();
        Match.WritePtrs = std::make_tuple(Handle.template AccessComponents<WriteTypes>()...);
        Match.ReadPtrs = std::make_tuple(Handle.template GetComponents<ReadTypes>()...);

        MatchedArchetypes.push_back(std::move(Match));
    }

    template<typename Func, size_t... WIs, size_t... RIs>
    static void CallWithIndex(Func&& Function, Entity E, size_t i,
                              const MatchedArchetype& Match,
                              std::index_sequence<WIs...>,
                              std::index_sequence<RIs...>)
    {
        Function(E, std::get<WIs>(Match.WritePtrs)[i]..., static_cast<const ReadTypes&>(std::get<RIs>(Match.ReadPtrs)[i])...);
    }

    std::vector<MatchedArchetype> MatchedArchetypes;
};