#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <unknwn.h>
#include <msctf.h>

#include "../IUnknownImpl.h"

interface __declspec(uuid("5F840B91-F834-498D-9EAA-C3F65D87A7B2")) Interface1: public IUnknown {
};

interface __declspec(uuid("31C548DD-7FD8-4380-BBD4-2F4F47F0BC0D")) Interface2 : public IUnknown {
};


TEST(TestIUnknownImpl, RefCounts)
{
    auto obj = new Ime::IUnknownImpl<Interface1, Interface2>();
    EXPECT_EQ(obj->refCount(), 1);

    EXPECT_EQ(obj->AddRef(), 2);
    EXPECT_EQ(obj->refCount(), 2);

    EXPECT_EQ(obj->Release(), 1);
    EXPECT_EQ(obj->refCount(), 1);

    EXPECT_EQ(obj->Release(), 0);

    // TODO: how to verify that obj is delected?
}

TEST(TestIUnknownImpl, QueryInterface)
{
    auto obj = new Ime::IUnknownImpl<Interface1, Interface2>();
    EXPECT_EQ(obj->refCount(), 1);

    IUnknown* ptr = nullptr;
    EXPECT_EQ(obj->QueryInterface(IID_IUnknown, reinterpret_cast<void**>(&ptr)), S_OK);
    EXPECT_EQ(ptr, (IUnknown*)obj);
    EXPECT_EQ(obj->refCount(), 2);
    ptr->Release();
    EXPECT_EQ(obj->refCount(), 1);

    Interface1* ptr1 = nullptr;
    EXPECT_EQ(obj->QueryInterface(__uuidof(Interface1), reinterpret_cast<void**>(&ptr1)), S_OK);
    EXPECT_EQ(ptr1, (Interface1*)obj);
    EXPECT_EQ(obj->refCount(), 2);
    ptr1->Release();
    EXPECT_EQ(obj->refCount(), 1);

    Interface2* ptr2 = nullptr;
    EXPECT_EQ(obj->QueryInterface(__uuidof(Interface2), reinterpret_cast<void**>(&ptr2)), S_OK);
    EXPECT_EQ(ptr2, (Interface2*)obj);
    EXPECT_EQ(obj->refCount(), 2);
    ptr2->Release();
    EXPECT_EQ(obj->refCount(), 1);

    EXPECT_EQ(obj->Release(), 0);
}

TEST(TestIUnknownImpl, QueryInterfaceTSF)
{
    class TestImpl : public Ime::IUnknownImpl<ITfCompartmentEventSink > {
    public:
        MOCK_METHOD(HRESULT, OnChange, (REFGUID rguid), (Calltype(STDMETHODCALLTYPE), override));

        MOCK_METHOD(void, destroy, (), ());

        virtual ~TestImpl() {
            destroy();
        }
    };

    auto obj = new TestImpl();
    EXPECT_EQ(obj->refCount(), 1);

    EXPECT_EQ(obj->QueryInterface(IID_IUnknown, nullptr), E_INVALIDARG);
    EXPECT_EQ(obj->refCount(), 1);

    ITfCompartmentEventSink* ptr = nullptr;
    EXPECT_EQ(obj->QueryInterface(IID_ITfCompartmentEventSink, reinterpret_cast<void**>(&ptr)), S_OK);
    EXPECT_EQ(ptr, (ITfCompartmentEventSink*)obj);
    EXPECT_EQ(obj->refCount(), 2);

    EXPECT_CALL((*obj), OnChange(GUID_COMPARTMENT_KEYBOARD_DISABLED)).Times(1);
    obj->OnChange(GUID_COMPARTMENT_KEYBOARD_DISABLED);

    ptr->Release();
    EXPECT_EQ(obj->refCount(), 1);

    ITfTextInputProcessor* ptr2;
    EXPECT_EQ(obj->QueryInterface(IID_ITfTextInputProcessor, reinterpret_cast<void**>(&ptr2)), E_NOINTERFACE);
    EXPECT_EQ(ptr2, nullptr);
    EXPECT_EQ(obj->refCount(), 1);

    EXPECT_CALL((*obj), destroy()).Times(1);
    EXPECT_EQ(obj->Release(), 0);
}
