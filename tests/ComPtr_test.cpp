#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <unknwn.h>
#include "../ComPtr.h"

using ::testing::Return;

/*
class IUnknownMock : public IUnknown {
public:
	// MOCK_METHOD(HRESULT STDMETHODCALLTYPE, QueryInterface, (REFIID riid, void** ppvObject), (override));
    MOCK_METHOD(ULONG, AddRef, (), (Calltype(STDMETHODCALLTYPE), override));
    MOCK_METHOD(ULONG, Release, (), (Calltype(STDMETHODCALLTYPE), override));
    MOCK_METHOD(HRESULT, QueryInterface, (REFIID riid, void** ppvObject), (Calltype(STDMETHODCALLTYPE), override));
};
*/

class IUnknownMock : public IUnknown {
public:
    IUnknownMock() : refCount_{ 0 } {}

    // MOCK_METHOD(HRESULT STDMETHODCALLTYPE, QueryInterface, (REFIID riid, void** ppvObject), (override));
    ULONG STDMETHODCALLTYPE AddRef() override {
        ++refCount_;
        return 0;
    }
    ULONG STDMETHODCALLTYPE Release() override {
        --refCount_;
        return 0;
    }

    MOCK_METHOD(HRESULT, QueryInterface, (REFIID riid, void** ppvObject), (Calltype(STDMETHODCALLTYPE), override));

    int refCount_;
};


TEST(TestComPtr, DefaultsToNull)
{
	Ime::ComPtr<IUnknownMock> ptr;
	EXPECT_EQ(ptr, nullptr);
}

TEST(TestComPtr, InitFromIUnknownPtr)
{
	IUnknownMock obj;
    Ime::ComPtr<IUnknownMock> ptr(&obj);
    EXPECT_EQ(ptr, &obj);
    EXPECT_EQ(obj.refCount_, 1);
}

TEST(TestComPtr, TakesOverIUnknownPtr)
{
    IUnknownMock obj;

    Ime::ComPtr<IUnknownMock> ptr(&obj, false);
    EXPECT_EQ(ptr, &obj);
    EXPECT_EQ(obj.refCount_, 0);
}

TEST(TestComPtr, Destructor)
{
    IUnknownMock obj;
    {
        Ime::ComPtr<IUnknownMock> ptr(&obj);
        EXPECT_EQ(obj.refCount_, 1);
    }
    EXPECT_EQ(obj.refCount_, 0);
}

TEST(TestComPtr, AddRefOnCopy)
{
    IUnknownMock obj;
    Ime::ComPtr<IUnknownMock> ptr{&obj};
    EXPECT_EQ(obj.refCount_, 1);

    Ime::ComPtr<IUnknownMock> ptr2{ ptr };
    EXPECT_EQ(obj.refCount_, 2);
    EXPECT_EQ(ptr2, &obj);
}

TEST(TestComPtr, MoveConstructor)
{
    IUnknownMock obj;
    Ime::ComPtr<IUnknownMock> ptr{ &obj };
    EXPECT_EQ(obj.refCount_, 1);

    Ime::ComPtr<IUnknownMock> ptr2{ std::move(ptr) };
    EXPECT_EQ(obj.refCount_, 1);
    EXPECT_EQ(ptr, nullptr);
    EXPECT_EQ(ptr2, &obj);
}
