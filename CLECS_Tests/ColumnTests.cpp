#include "pch.h"
#include "Column.h"
#include "TestsTypes.h"

struct IntComp
{
    int Value = 0;
};

struct Vec2Comp
{
    float X = 0.f;
    float Y = 0.f;
};

TEST(ColumnDescriptionTest, Make_SetsCorrectElementSize)
{
    const auto Desc = ColumnDescription::Make<IntComp>();
    EXPECT_EQ(Desc.ElementSize, sizeof(IntComp));
}

TEST(ColumnDescriptionTest, Make_SetsNonNullMoveConstruct)
{
    const auto Desc = ColumnDescription::Make<IntComp>();
    EXPECT_NE(Desc.MoveConstruct, nullptr);
}

TEST(ColumnDescriptionTest, Make_SetsNonNullDestruct)
{
    const auto Desc = ColumnDescription::Make<IntComp>();
    EXPECT_NE(Desc.Destruct, nullptr);
}

TEST(ColumnDescriptionTest, MoveConstruct_MovesDataCorrectly)
{
    const auto Desc = ColumnDescription::Make<IntComp>();

    alignas(IntComp) uint8_t SrcBuf[sizeof(IntComp)];
    alignas(IntComp) uint8_t DstBuf[sizeof(IntComp)];

    new (SrcBuf) IntComp{42};
    Desc.MoveConstruct(DstBuf, SrcBuf);

    EXPECT_EQ(reinterpret_cast<IntComp*>(DstBuf)->Value, 42);

    Desc.Destruct(SrcBuf);
    Desc.Destruct(DstBuf);
}

TEST(ColumnDescriptionTest, Destruct_CallsDestructor)
{
    const auto Desc = ColumnDescription::Make<TrackedComp>();

    alignas(TrackedComp) uint8_t Buf[sizeof(TrackedComp)];
    int DestructCount = 0;
    new (Buf) TrackedComp{&DestructCount};

    Desc.Destruct(Buf);

    EXPECT_EQ(DestructCount, 1);
}

class ColumnTest : public ::testing::Test
{
protected:
    static Column MakeIntColumn()
    {
        return Column(ColumnDescription::Make<IntComp>());
    }

    static Column MakeTrackedColumn()
    {
        return Column(ColumnDescription::Make<TrackedComp>());
    }
};

TEST_F(ColumnTest, Size_InitiallyZero)
{
    auto Col = MakeIntColumn();
    EXPECT_EQ(Col.Size(), 0u);
}

TEST_F(ColumnTest, GetDescription_ReturnsDescriptionWithCorrectElementSize)
{
    auto Col = MakeIntColumn();
    EXPECT_EQ(Col.GetDescription().ElementSize, sizeof(IntComp));
}

TEST_F(ColumnTest, EmplaceBack_IncrementsSize)
{
    auto Col = MakeIntColumn();
    Col.EmplaceBack<IntComp>(IntComp{1});
    EXPECT_EQ(Col.Size(), 1u);
}

TEST_F(ColumnTest, EmplaceBack_MultipleElements_SizeIsCorrect)
{
    auto Col = MakeIntColumn();
    Col.EmplaceBack<IntComp>(IntComp{1});
    Col.EmplaceBack<IntComp>(IntComp{2});
    Col.EmplaceBack<IntComp>(IntComp{3});
    EXPECT_EQ(Col.Size(), 3u);
}

TEST_F(ColumnTest, EmplaceBack_StoredValueIsAccessible)
{
    auto Col = MakeIntColumn();
    Col.EmplaceBack<IntComp>(IntComp{99});
    EXPECT_EQ(Col.AccessData<IntComp>()[0].Value, 99);
}

TEST_F(ColumnTest, AccessData_MultipleElements_AllValuesCorrect)
{
    auto Col = MakeIntColumn();
    Col.EmplaceBack<IntComp>(IntComp{10});
    Col.EmplaceBack<IntComp>(IntComp{20});
    Col.EmplaceBack<IntComp>(IntComp{30});

    const auto* Data = Col.AccessData<IntComp>();
    EXPECT_EQ(Data[0].Value, 10);
    EXPECT_EQ(Data[1].Value, 20);
    EXPECT_EQ(Data[2].Value, 30);
}

TEST_F(ColumnTest, AccessData_AllowsModification)
{
    auto Col = MakeIntColumn();
    Col.EmplaceBack<IntComp>(IntComp{5});

    Col.AccessData<IntComp>()[0].Value = 100;

    EXPECT_EQ(Col.AccessData<IntComp>()[0].Value, 100);
}

TEST_F(ColumnTest, GetData_ReturnsCorrectValues)
{
    auto Col = MakeIntColumn();
    Col.EmplaceBack<IntComp>(IntComp{7});
    Col.EmplaceBack<IntComp>(IntComp{8});

    const Column& ConstCol = Col;
    const auto* Data = ConstCol.GetData<IntComp>();
    EXPECT_EQ(Data[0].Value, 7);
    EXPECT_EQ(Data[1].Value, 8);
}

TEST_F(ColumnTest, Reserve_DoesNotChangeSize)
{
    auto Col = MakeIntColumn();
    Col.Reserve(100);
    EXPECT_EQ(Col.Size(), 0u);
}

TEST_F(ColumnTest, Reserve_AllowsSubsequentEmplaceBack)
{
    auto Col = MakeIntColumn();
    Col.Reserve(5);
    for (int i = 0; i < 5; ++i)
    {
        Col.EmplaceBack<IntComp>(IntComp{i});
    }
    EXPECT_EQ(Col.Size(), 5u);
}

TEST_F(ColumnTest, SwapRemove_OnlyElement_DecreasesSize)
{
    auto Col = MakeIntColumn();
    Col.EmplaceBack<IntComp>(IntComp{1});
    Col.SwapRemove(0);
    EXPECT_EQ(Col.Size(), 0u);
}

TEST_F(ColumnTest, SwapRemove_MiddleElement_DecreasesSize)
{
    auto Col = MakeIntColumn();
    Col.EmplaceBack<IntComp>(IntComp{1});
    Col.EmplaceBack<IntComp>(IntComp{2});
    Col.EmplaceBack<IntComp>(IntComp{3});
    Col.SwapRemove(1);
    EXPECT_EQ(Col.Size(), 2u);
}

TEST_F(ColumnTest, SwapRemove_FirstElement_LastElementFillsItsSlot)
{
    auto Col = MakeIntColumn();
    Col.EmplaceBack<IntComp>(IntComp{10});
    Col.EmplaceBack<IntComp>(IntComp{20});
    Col.EmplaceBack<IntComp>(IntComp{30});
    Col.SwapRemove(0);

    const auto* Data = Col.AccessData<IntComp>();
    EXPECT_EQ(Data[0].Value, 30);
    EXPECT_EQ(Data[1].Value, 20);
}

TEST_F(ColumnTest, SwapRemove_LastElement_OtherElementsUnchanged)
{
    auto Col = MakeIntColumn();
    Col.EmplaceBack<IntComp>(IntComp{10});
    Col.EmplaceBack<IntComp>(IntComp{20});
    Col.EmplaceBack<IntComp>(IntComp{30});
    Col.SwapRemove(2);

    const auto* Data = Col.AccessData<IntComp>();
    EXPECT_EQ(Data[0].Value, 10);
    EXPECT_EQ(Data[1].Value, 20);
}

TEST_F(ColumnTest, SwapRemove_OnlyElement_CallsDestructor)
{
    auto Col = MakeTrackedColumn();
    int DestructCount = 0;
    Col.EmplaceBack<TrackedComp>(&DestructCount);

    Col.SwapRemove(0);

    EXPECT_EQ(DestructCount, 1);
}

TEST_F(ColumnTest, SwapRemove_MiddleElement_CallsDestructorOnRemovedElement)
{
    auto Col = MakeTrackedColumn();
    int DestructCount = 0;
    Col.EmplaceBack<TrackedComp>(&DestructCount);
    Col.EmplaceBack<TrackedComp>(&DestructCount);
    DestructCount = 0;

    Col.SwapRemove(0);

    EXPECT_EQ(DestructCount, 1);
}

TEST_F(ColumnTest, MoveAppendFrom_IncreasesDestinationSize)
{
    auto Src = MakeIntColumn();
    auto Dst = MakeIntColumn();

    Src.EmplaceBack<IntComp>(IntComp{42});
    Dst.MoveAppendFrom(Src, 0);

    EXPECT_EQ(Dst.Size(), 1u);
}

TEST_F(ColumnTest, MoveAppendFrom_DoesNotChangeSourceSizeBeforeSwapRemove)
{
    auto Src = MakeIntColumn();
    auto Dst = MakeIntColumn();

    Src.EmplaceBack<IntComp>(IntComp{42});
    Dst.MoveAppendFrom(Src, 0);

    EXPECT_EQ(Src.Size(), 1u);
}

TEST_F(ColumnTest, MoveAppendFrom_MovedDataHasCorrectValue)
{
    auto Src = MakeIntColumn();
    auto Dst = MakeIntColumn();

    Src.EmplaceBack<IntComp>(IntComp{42});
    Dst.MoveAppendFrom(Src, 0);
    Src.SwapRemove(0);

    EXPECT_EQ(Dst.AccessData<IntComp>()[0].Value, 42);
}

TEST_F(ColumnTest, MoveAppendFrom_SpecificRow_MovesCorrectElement)
{
    auto Src = MakeIntColumn();
    auto Dst = MakeIntColumn();

    Src.EmplaceBack<IntComp>(IntComp{1});
    Src.EmplaceBack<IntComp>(IntComp{2});
    Src.EmplaceBack<IntComp>(IntComp{3});

    Dst.MoveAppendFrom(Src, 1);
    Src.SwapRemove(1);

    EXPECT_EQ(Dst.AccessData<IntComp>()[0].Value, 2);
}

TEST_F(ColumnTest, MoveAppendFrom_AppendsAfterExistingElements)
{
    auto Src = MakeIntColumn();
    auto Dst = MakeIntColumn();

    Src.EmplaceBack<IntComp>(IntComp{99});
    Dst.EmplaceBack<IntComp>(IntComp{1});
    Dst.EmplaceBack<IntComp>(IntComp{2});

    Dst.MoveAppendFrom(Src, 0);
    Src.SwapRemove(0);

    EXPECT_EQ(Dst.Size(), 3u);
    EXPECT_EQ(Dst.AccessData<IntComp>()[2].Value, 99);
}