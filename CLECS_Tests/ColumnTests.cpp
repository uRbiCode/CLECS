#include "pch.h"
#include "Column.h"
#include "TrackedComponent.h"

struct ColumnTestComponent
{
    int Value = 0;
};

TEST(ColumnDescriptionTest, Make_SetsCorrectElementSize)
{
    const ColumnDescription Desc = ColumnDescription::Make<ColumnTestComponent>();
    EXPECT_EQ(Desc.ElementSize, sizeof(ColumnTestComponent));
}

TEST(ColumnDescriptionTest, Make_SetsNonNullMoveConstruct)
{
    const ColumnDescription Desc = ColumnDescription::Make<ColumnTestComponent>();
    EXPECT_NE(Desc.MoveConstruct, nullptr);
}

TEST(ColumnDescriptionTest, Make_SetsNonNullDestruct)
{
    const ColumnDescription Desc = ColumnDescription::Make<ColumnTestComponent>();
    EXPECT_NE(Desc.Destruct, nullptr);
}

TEST(ColumnDescriptionTest, MoveConstruct_MovesDataCorrectly)
{
    const ColumnDescription Desc = ColumnDescription::Make<ColumnTestComponent>();

    alignas(ColumnTestComponent) uint8_t SrcBuf[sizeof(ColumnTestComponent)];
    alignas(ColumnTestComponent) uint8_t DstBuf[sizeof(ColumnTestComponent)];
    new (SrcBuf) ColumnTestComponent{42};
    Desc.MoveConstruct(DstBuf, SrcBuf);

    EXPECT_EQ(reinterpret_cast<ColumnTestComponent*>(DstBuf)->Value, 42);

    Desc.Destruct(SrcBuf);
    Desc.Destruct(DstBuf);
}

TEST(ColumnDescriptionTest, Destruct_CallsDestructor)
{
    const ColumnDescription Desc = ColumnDescription::Make<TrackedComponent>();

    alignas(TrackedComponent) uint8_t Buf[sizeof(TrackedComponent)];
    int DestructCount = 0;
    new (Buf) TrackedComponent{&DestructCount};

    Desc.Destruct(Buf);

    EXPECT_EQ(DestructCount, 1);
}

class ColumnTest : public testing::Test
{
protected:
    static Column MakeTestColumn()
    {
        return Column(ColumnDescription::Make<ColumnTestComponent>());
    }

    static Column MakeTrackedColumn()
    {
        return Column(ColumnDescription::Make<TrackedComponent>());
    }
};

TEST_F(ColumnTest, Size_InitiallyZero)
{
    const Column Col = MakeTestColumn();
    EXPECT_EQ(Col.Size(), 0u);
}

TEST_F(ColumnTest, GetDescription_ReturnsDescriptionWithCorrectElementSize)
{
    const Column Col = MakeTestColumn();
    EXPECT_EQ(Col.GetDescription().ElementSize, sizeof(ColumnTestComponent));
}

TEST_F(ColumnTest, EmplaceBack_IncrementsSize)
{
    Column Col = MakeTestColumn();
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{1});
    EXPECT_EQ(Col.Size(), 1u);
}

TEST_F(ColumnTest, EmplaceBack_MultipleElements_SizeIsCorrect)
{
    Column Col = MakeTestColumn();
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{1});
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{2});
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{3});
    EXPECT_EQ(Col.Size(), 3u);
}

TEST_F(ColumnTest, EmplaceBack_StoredValueIsAccessible)
{
    Column Col = MakeTestColumn();
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{99});
    EXPECT_EQ(Col.AccessData<ColumnTestComponent>()[0].Value, 99);
}

TEST_F(ColumnTest, AccessData_MultipleElements_AllValuesCorrect)
{
    Column Col = MakeTestColumn();
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{10});
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{20});
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{30});

    const ColumnTestComponent* Data = Col.GetData<ColumnTestComponent>();
    EXPECT_EQ(Data[0].Value, 10);
    EXPECT_EQ(Data[1].Value, 20);
    EXPECT_EQ(Data[2].Value, 30);
}

TEST_F(ColumnTest, AccessData_AllowsModification)
{
    Column Col = MakeTestColumn();
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{5});

    Col.AccessData<ColumnTestComponent>()[0].Value = 100;

    EXPECT_EQ(Col.AccessData<ColumnTestComponent>()[0].Value, 100);
}

TEST_F(ColumnTest, GetData_ReturnsCorrectValues)
{
    Column Col = MakeTestColumn();
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{7});
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{8});

    const Column& ConstCol = Col;
    const ColumnTestComponent* Data = ConstCol.GetData<ColumnTestComponent>();
    EXPECT_EQ(Data[0].Value, 7);
    EXPECT_EQ(Data[1].Value, 8);
}

TEST_F(ColumnTest, Reserve_DoesNotChangeSize)
{
    Column Col = MakeTestColumn();
    Col.Reserve(100);
    EXPECT_EQ(Col.Size(), 0u);
}

TEST_F(ColumnTest, Reserve_AllowsSubsequentEmplaceBack)
{
    Column Col = MakeTestColumn();
    Col.Reserve(5);
    for (int i = 0; i < 5; ++i)
    {
        Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{i});
    }
    EXPECT_EQ(Col.Size(), 5u);
}

TEST_F(ColumnTest, SwapRemove_OnlyElement_DecreasesSize)
{
    Column Col = MakeTestColumn();
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{1});
    Col.SwapRemove(0);
    EXPECT_EQ(Col.Size(), 0u);
}

TEST_F(ColumnTest, SwapRemove_MiddleElement_DecreasesSize)
{
    Column Col = MakeTestColumn();
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{1});
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{2});
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{3});
    Col.SwapRemove(1);
    EXPECT_EQ(Col.Size(), 2u);
}

TEST_F(ColumnTest, SwapRemove_FirstElement_LastElementFillsItsSlot)
{
    Column Col = MakeTestColumn();
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{10});
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{20});
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{30});
    Col.SwapRemove(0);

    const ColumnTestComponent* Data = Col.GetData<ColumnTestComponent>();
    EXPECT_EQ(Data[0].Value, 30);
    EXPECT_EQ(Data[1].Value, 20);
}

TEST_F(ColumnTest, SwapRemove_LastElement_OtherElementsUnchanged)
{
    Column Col = MakeTestColumn();
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{10});
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{20});
    Col.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{30});
    Col.SwapRemove(2);

    const ColumnTestComponent* Data = Col.GetData<ColumnTestComponent>();
    EXPECT_EQ(Data[0].Value, 10);
    EXPECT_EQ(Data[1].Value, 20);
}

TEST_F(ColumnTest, SwapRemove_OnlyElement_CallsDestructor)
{
    Column Col = MakeTrackedColumn();
    int DestructCount = 0;
    Col.EmplaceBack<TrackedComponent>(&DestructCount);

    Col.SwapRemove(0);

    EXPECT_EQ(DestructCount, 1);
}

TEST_F(ColumnTest, SwapRemove_MiddleElement_CallsDestructorOnRemovedElement)
{
    Column Col = MakeTrackedColumn();
    int DestructCount = 0;
    Col.EmplaceBack<TrackedComponent>(&DestructCount);
    Col.EmplaceBack<TrackedComponent>(&DestructCount);
    DestructCount = 0;

    Col.SwapRemove(0);

    EXPECT_EQ(DestructCount, 1);
}

TEST_F(ColumnTest, MoveAppendFrom_IncreasesDestinationSize)
{
    Column Source = MakeTestColumn();
    Column Destination = MakeTestColumn();

    Source.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{42});
    Destination.MoveAppendFrom(Source, 0);

    EXPECT_EQ(Destination.Size(), 1u);
}

TEST_F(ColumnTest, MoveAppendFrom_DoesNotChangeSourceSizeBeforeSwapRemove)
{
    Column Source = MakeTestColumn();
    Column Destination = MakeTestColumn();

    Source.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{42});
    Destination.MoveAppendFrom(Source, 0);
    EXPECT_EQ(Source.Size(), 1u);
}

TEST_F(ColumnTest, MoveAppendFrom_MovedDataHasCorrectValue)
{
    Column Source = MakeTestColumn();
    Column Destination = MakeTestColumn();

    Source.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{42});
    Destination.MoveAppendFrom(Source, 0);
    Source.SwapRemove(0);

    EXPECT_EQ(Destination.AccessData<ColumnTestComponent>()[0].Value, 42);
}

TEST_F(ColumnTest, MoveAppendFrom_SpecificRow_MovesCorrectElement)
{
    Column Source = MakeTestColumn();
    Column Destination = MakeTestColumn();

    Source.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{1});
    Source.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{2});
    Source.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{3});

    Destination.MoveAppendFrom(Source, 1);
    Source.SwapRemove(1);

    EXPECT_EQ(Destination.AccessData<ColumnTestComponent>()[0].Value, 2);
}

TEST_F(ColumnTest, MoveAppendFrom_AppendsAfterExistingElements)
{
    Column Source = MakeTestColumn();
    Column Destination = MakeTestColumn();

    Source.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{99});
    Destination.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{1});
    Destination.EmplaceBack<ColumnTestComponent>(ColumnTestComponent{2});

    Destination.MoveAppendFrom(Source, 0);
    Source.SwapRemove(0);

    EXPECT_EQ(Destination.Size(), 3u);
    EXPECT_EQ(Destination.AccessData<ColumnTestComponent>()[2].Value, 99);
}