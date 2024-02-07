//#include "pch.h"
#include "CppUnitTest.h"
#include <format>
#include "../src/expander.h"
#include "../src/simple_resolver.h"
#include "../src/registry_resolver.h"
#include "../src/cmd_resolver.h"
#include "../src/time_measure.h"
#include "fake.h"

#include <regex>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace hexpander;

namespace hexpander_tests
{

    TEST_CLASS(simple_expands)
    {
    public:
        expander tex;
        std::shared_ptr<simple_resolver> sp_sr;

        TEST_METHOD_INITIALIZE(registry_expands_init)
        {
            sp_sr = std::make_shared<simple_resolver>();
            tex.register_resolver(sp_sr);
        }

        TEST_METHOD(basics)
        {
            sp_sr->add_expand(L"%TEST1%", L"value1");
            sp_sr->add_expand(L"%TEST_2%", L"value 2");

            // Actual debug
            Assert::AreEqual<std::wstring>(L"value1value 2", tex.expand(L"%TEST1%%TEST_2%"));

            // 
            Assert::AreEqual<std::wstring>(L"no-token", tex.expand(L"no-token"));
            measure::time timer(L"expand");
            Assert::AreEqual<std::wstring>(L"value1", tex.expand(L"%TEST1%"));
            timer.end_measure([&](const wchar_t* id, double dDiff) {
                Logger::WriteMessage(std::format(L"Measure {} time = {:5.5f}", id, dDiff).c_str());
            });
            Assert::AreEqual<std::wstring>(L"xx value1 yy" , tex.expand(L"xx %TEST1% yy"));
            Assert::AreEqual<std::wstring>(L"%TEST%" , tex.expand(L"%TEST%"));
            Assert::AreEqual<std::wstring>(L"xx %TEST% yy", tex.expand(L"xx %TEST% yy"));
            Assert::AreEqual<std::wstring>(L"xx % yy", tex.expand(L"xx % yy"));
            Assert::AreEqual<std::wstring>(L"xx %% yy", tex.expand(L"xx %% yy"));

            Assert::AreEqual<std::wstring>(L"value 2", tex.expand(L"%TEST_2%"));
            Assert::AreEqual<std::wstring>(L"value1value 2", tex.expand(L"%TEST1%%TEST_2%"));
            Assert::AreEqual<std::wstring>(L"value1value 2 xx", tex.expand(L"%TEST1%%TEST_2% xx"));
            Assert::AreEqual<std::wstring>(L"yy value1value 2 xx", tex.expand(L"yy %TEST1%%TEST_2% xx"));
            Assert::AreEqual<std::wstring>(L"value1 value 2", tex.expand(L"%TEST1% %TEST_2%"));
        }

        TEST_METHOD(recurse)
        {
            sp_sr->add_expand(L"%TEST_1%", L"mid");
            sp_sr->add_expand(L"%TEST_2%", L"head %TEST_1% tail");
            sp_sr->add_expand(L"%TEST_1% %TEST_3%", L"head %TEST_1% tail");
            measure::time timer(L"expand");
            Assert::AreEqual<std::wstring>(L"head mid tail", tex.expand(L"%TEST_2%"));
            timer.end_measure([&](const wchar_t* id, double dDiff) {
                Logger::WriteMessage(std::format(L"Measure {} time = {:5.5f}", id, dDiff).c_str());
            });
        }

        TEST_METHOD(lang)
        {
            sp_sr->add_expand(L"%TEST_1%", L"en");
            sp_sr->add_expand(L"%TEST_1%", L"cz", L"cs-cz");
            //Assert::AreEqual<std::wstring>(L"en", tex.expand(L"%TEST_1%"));
            measure::time timer(L"expand");
            Assert::AreEqual<std::wstring>(L"cz", tex.expand(L"%TEST_1,cs-cz%"));
            timer.end_measure([&](const wchar_t* id, double dDiff) {
                Logger::WriteMessage(std::format(L"Measure {} time = {:5.5f}", id, dDiff).c_str());
            });
        }
    };
    
    TEST_CLASS(registry_expands)
    {
    public:
        std::shared_ptr<simple_resolver> sp_sr;
        std::shared_ptr<registry_fake>   sp_rf;
        expander                         tex;

        TEST_METHOD_INITIALIZE(registry_expands_init)
        {
            sp_sr = std::make_shared<simple_resolver>();
            sp_rf = std::make_shared<registry_fake>();
            std::shared_ptr<registry_resolver> sp_reg_resolver = std::make_shared<registry_resolver>(sp_rf);
            tex.register_resolver(sp_sr);
            tex.register_resolver(sp_reg_resolver);
        }

        TEST_METHOD(registry_basic)
        {
            // basic test
            sp_rf->m_read_reg_value_rv = L"reg1";
            Assert::AreEqual<std::wstring>(L"reg1", tex.expand(L"%REG[root1,key1,vn1,wow1]%"));
            Assert::AreEqual<std::wstring>(L"root1\\key1", sp_rf->m_read_reg_value_key);
            Assert::AreEqual<std::wstring>(L"vn1", sp_rf->m_read_reg_value_value_name);
            Assert::AreEqual<std::wstring>(L"wow1", sp_rf->m_read_reg_value_wow);
        }

        TEST_METHOD(registry_missing_parameter)
        {
            sp_rf->m_read_reg_value_rv = L"reg2";
            Assert::AreEqual<std::wstring>(L"reg2", tex.expand(L"%REG[root2,key2,vn2]%"));
            Assert::AreEqual<std::wstring>(L"root2\\key2", sp_rf->m_read_reg_value_key);
            Assert::AreEqual<std::wstring>(L"vn2", sp_rf->m_read_reg_value_value_name);
            Assert::AreEqual<std::wstring>(L"native-wow", sp_rf->m_read_reg_value_wow);
        }

        TEST_METHOD(registry_recurse)
        {
            sp_sr->add_expand(L"%TEST_1%", L"root");
            sp_sr->add_expand(L"%TEST_2%", L"root2");
            sp_sr->add_expand(L"%TEST_3%", L"%TEST_1%");
            //
            sp_rf->m_read_reg_value_rv = L"reg1";
            Assert::AreEqual<std::wstring>(L"reg1", tex.expand(L"%REG[%TEST_1%,key1,vn1,wow]%"));
            Assert::AreEqual<std::wstring>(L"root\\key1", sp_rf->m_read_reg_value_key);
            Assert::AreEqual<std::wstring>(L"vn1", sp_rf->m_read_reg_value_value_name);
            Assert::AreEqual<std::wstring>(L"wow", sp_rf->m_read_reg_value_wow);
            //
            sp_rf->m_read_reg_value_rv = L"reg2";
            Assert::AreEqual<std::wstring>(L"reg2", tex.expand(L"%REG[xx %TEST_1% yy,key1,vn1,wow]%"));
            Assert::AreEqual<std::wstring>(L"xx root yy\\key1", sp_rf->m_read_reg_value_key);\
            //
            sp_rf->m_read_reg_value_rv = L"reg3";
            Assert::AreEqual<std::wstring>(L"reg3", tex.expand(L"%REG[%TEST_2%,key1,vn1,wow]%"));
            Assert::AreEqual<std::wstring>(L"root2\\key1", sp_rf->m_read_reg_value_key);
        }

        TEST_METHOD(registry_and_normal)
        {
            sp_sr->add_expand(L"%TEST_1%", L"root");
            sp_sr->add_expand(L"%TEST_2%", L"root2");
            sp_sr->add_expand(L"%TEST_3%", L"%TEST_1%");
            //
            sp_rf->m_read_reg_value_rv = L"reg1";
            Assert::AreEqual<std::wstring>(L"root reg1,root", tex.expand(L"%TEST_1% %REG[%TEST_2%,key1,vn1,wow]%,%TEST_1%"));
            Assert::AreEqual<std::wstring>(L"root2\\key1", sp_rf->m_read_reg_value_key);
        }
    };
    

    TEST_CLASS(cmd_expands)
    {
    public:
        std::shared_ptr<simple_resolver> sp_sr;
        std::shared_ptr<cmd_resolver>    sp_cmdr;
        expander                         tex;

        TEST_METHOD_INITIALIZE(cmd_expands_init)
        {
            sp_sr = std::make_shared<simple_resolver>();
            sp_cmdr = std::make_shared<cmd_resolver>();
            tex.register_resolver(sp_sr);
            tex.register_resolver(sp_cmdr);
        }

        TEST_METHOD(cmd_basic)
        {
            // basic test
            sp_cmdr->add_cmd_parameter(L"cmd1", L"cmd-val1");
            Assert::AreEqual<std::wstring>(L"cmd-val1", tex.expand(L"%CMD[cmd1]%"));
        }

        TEST_METHOD(cmd_missing_parameter)
        {
            Assert::AreEqual<std::wstring>(L"", tex.expand(L"%CMD[unknown]%"));
        }

        TEST_METHOD(cmd_recurse)
        {
            sp_sr->add_expand(L"%TEST_1%", L"cmd-key1");
            sp_sr->add_expand(L"%TEST_2%", L"cmd-key2");
            sp_sr->add_expand(L"%TEST_3%", L"%TEST_2%");
            sp_cmdr->add_cmd_parameter(L"cmd-key2", L"cmd-val2");
            //
            Assert::AreEqual<std::wstring>(L"", tex.expand(L"%CMD[%TEST_1%]%"));
            Assert::AreEqual<std::wstring>(L"cmd-val2", tex.expand(L"%CMD[%TEST_2%]%"));
            //
            Assert::AreEqual<std::wstring>(L"", tex.expand(L"%CMD[xx %TEST_1% yy]%"));
            Assert::AreEqual<std::wstring>(L"cmd-val2", tex.expand(L"%CMD[%TEST_3%]%"));
        }

        TEST_METHOD(cmd_and_normal)
        {
            //sp_sr->add_expand(L"%TEST_1%", L"root");
            //sp_sr->add_expand(L"%TEST_2%", L"root2");
            //sp_sr->add_expand(L"%TEST_3%", L"%TEST_1%");
            ////
            //sp_rf->m_read_reg_value_rv = L"reg1";
            //Assert::AreEqual<std::wstring>(L"root reg1,root", tex.expand(L"%TEST_1% %REG[%TEST_2%,key1,vn1,wow]%,%TEST_1%"));
            //Assert::AreEqual<std::wstring>(L"root2\\key1", sp_rf->m_read_reg_value_key);
        }
    };

}