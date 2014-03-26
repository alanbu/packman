
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"

#include "libpkg/module_info.h"
#include "libpkg/version.h"
#include "fileutils.h"


/**
 * Test fixture to test the module_info class used for reading
 * the details of a RISC OS module
 */
class ModuleTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( ModuleTest );

	CPPUNIT_TEST( test_construct );

	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Test fixture setup override
	 */
	void setUp()
	{
	}

	/**
	 * Text fixture tear down override
	 */
	void tearDown()
	{
	}

	/**
	 * Test the component constructors
	 */
	void test_construct()
	{
		pkg::module_info test;

		// Test empty constructor
		CPPUNIT_ASSERT_EQUAL(test.read_ok(), false);

		// Test constructor with invalid module
		pkg::module_info duff("System:Modules.ThisDontExist");
		CPPUNIT_ASSERT_EQUAL(duff.read_ok(), false);

		// Following test assumes SharedUnixLib is on the system
		// which is fairly safe as the program won't run without it
		pkg::module_info unixlib("System:Modules.SharedULib");
		CPPUNIT_ASSERT_EQUAL(unixlib.read_ok(), true);

		CPPUNIT_ASSERT(unixlib.title() == "SharedUnixLibrary");

		// Check version format
		std::string ver(unixlib.version());
		std::string::iterator c = ver.begin();
		CPPUNIT_ASSERT(c != ver.end());
		while (c != ver.end() && *c != '.')
		{
			CPPUNIT_ASSERT(*c >= '0' && *c <= '9');
			++c;
		}
		CPPUNIT_ASSERT(*c == '.');
		++c;
		while (c != ver.end() && (*c >= '0' && *c <= '9')) ++c;
		CPPUNIT_ASSERT(c == ver.end());

		// Check version is greater than or equal to 1.12 (used currently)
		pkg::version v(ver), v112("1.12");
		CPPUNIT_ASSERT(v >= v112);		

		// Temp code just to view output while testing
		// std::cout << "title  : " << unixlib.title() << std::endl;
		// std::cout << "help   : " << unixlib.help_string() << std::endl;
		// std::cout << "version: " << unixlib.version() << std::endl;
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ModuleTest );
