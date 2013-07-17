
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"

#include "libpkg/component.h"
#include "fileutils.h"


/**
 * Test fixture to test the look_at class used for reading/writing
 * The lookup options for RISC OS
 */
class ComponentTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( ComponentTest );

	CPPUNIT_TEST( test_construct );
	CPPUNIT_TEST( test_compare );
	CPPUNIT_TEST( test_parse_list );
	CPPUNIT_TEST( test_io );

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
		pkg::component test;

		// Test empty constructor
		CPPUNIT_ASSERT(test.name().empty());
		CPPUNIT_ASSERT(test.flags() == 0);
		CPPUNIT_ASSERT(test.path().empty());

		// Test with no flags
		CPPUNIT_ASSERT_NO_THROW(test = pkg::component("Apps.Misc.!PackIt"));
		CPPUNIT_ASSERT(test.name() == "Apps.Misc.!PackIt");
		CPPUNIT_ASSERT(test.flags() == 0);
		CPPUNIT_ASSERT(test.path().empty());

		// Test moveable flag
		CPPUNIT_ASSERT_NO_THROW(test = pkg::component("Apps.Misc.!PackIt (Moveable)"));
		CPPUNIT_ASSERT(test.name() == "Apps.Misc.!PackIt");
		CPPUNIT_ASSERT(test.path().empty());
		CPPUNIT_ASSERT(test.flag(pkg::component::moveable));
		CPPUNIT_ASSERT(!test.flag(pkg::component::look_at));
		CPPUNIT_ASSERT(!test.flag(pkg::component::run));
		CPPUNIT_ASSERT(!test.flag(pkg::component::add_to_apps));

		// Test look_at flag
		CPPUNIT_ASSERT_NO_THROW(test = pkg::component("Apps.Misc.!PackIt ( LookAt )"));
		CPPUNIT_ASSERT(test.name() == "Apps.Misc.!PackIt");
		CPPUNIT_ASSERT(test.path().empty());
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::moveable), false);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::look_at), true);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::run), false);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::add_to_apps), false);

		// Test Run flag
		CPPUNIT_ASSERT_NO_THROW(test = pkg::component("Apps.Misc.!PackIt (Run )"));
		CPPUNIT_ASSERT(test.name() == "Apps.Misc.!PackIt");
		CPPUNIT_ASSERT(test.path().empty());
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::moveable), false);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::look_at), false);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::run), true);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::add_to_apps), false);

		// Test add_to_apps flag
		CPPUNIT_ASSERT_NO_THROW(test = pkg::component("Apps.Misc.!PackIt ( AddToApps)"));
		CPPUNIT_ASSERT(test.name() == "Apps.Misc.!PackIt");
		CPPUNIT_ASSERT(test.path().empty());
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::moveable), false);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::look_at), false);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::run), false);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::add_to_apps), true);

		// Test combination of two flags
		CPPUNIT_ASSERT_NO_THROW(test = pkg::component("Apps.Misc.!PackIt (Moveable LookAt)"));
		CPPUNIT_ASSERT(test.name() == "Apps.Misc.!PackIt");
		CPPUNIT_ASSERT(test.path().empty());
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::moveable), true);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::look_at), true);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::run), false);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::add_to_apps), false);

		// Test combination of three flags
		CPPUNIT_ASSERT_NO_THROW(test = pkg::component("Apps.Misc.!PackIt ( LookAt Moveable Run )"));
		CPPUNIT_ASSERT(test.name() == "Apps.Misc.!PackIt");
		CPPUNIT_ASSERT(test.path().empty());
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::moveable), true);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::look_at), true);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::run), true);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::add_to_apps), false);

		// Test all four flags together
		CPPUNIT_ASSERT_NO_THROW(test = pkg::component("Apps.Misc.!PackIt (  AddToApps   LookAt Moveable Run )"));
		CPPUNIT_ASSERT(test.name() == "Apps.Misc.!PackIt");
		CPPUNIT_ASSERT(test.path().empty());
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::moveable), true);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::look_at), true);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::run), true);
		CPPUNIT_ASSERT_EQUAL(test.flag(pkg::component::add_to_apps), true);

		// Check parse errors are caught
		CPPUNIT_ASSERT_THROW( test = pkg::component("Apps oops"), pkg::component::parse_error);
		CPPUNIT_ASSERT_THROW( test = pkg::component("Apps.Test (Moveable"), pkg::component::parse_error);
		CPPUNIT_ASSERT_THROW( test = pkg::component("Apps.Test (MadeUpFlag)"), pkg::component::parse_error);
		CPPUNIT_ASSERT_THROW( test = pkg::component("Apps.Test (LookAt MadeUpFlag)"), pkg::component::parse_error);
	}

	/**
	 * Check comparison
	 */
	void test_compare()
	{
		pkg::component test1("Apps.Misc.!PackIt"), test2("Apps.Misc.!PackIt"), test3("Apps.Misc.!PickIt");

		CPPUNIT_ASSERT(test1 == test2);
		CPPUNIT_ASSERT(test1 != test3);

		// Test changing paths
		test2.path("ADFS::$.Apps.Misc.!PackIt");
		CPPUNIT_ASSERT(test2.path() == "ADFS::$.Apps.Misc.!PackIt");
		CPPUNIT_ASSERT(test1 != test2);
		test1.path(test2.path());
		CPPUNIT_ASSERT(test1 == test2);

		// Test flags
		test1.flag(pkg::component::moveable, true);
		CPPUNIT_ASSERT(test1 != test2);
		test2.flag(pkg::component::moveable, true);
		CPPUNIT_ASSERT(test1 == test2);

		test1.flag(pkg::component::look_at, true);
		CPPUNIT_ASSERT(test1 != test2);
		test2.flag(pkg::component::look_at, true);
		CPPUNIT_ASSERT(test1 == test2);

		test1.flag(pkg::component::run, true);
		CPPUNIT_ASSERT(test1 != test2);
		test2.flag(pkg::component::run, true);
		CPPUNIT_ASSERT(test1 == test2);

		test1.flag(pkg::component::add_to_apps, true);
		CPPUNIT_ASSERT(test1 != test2);
		test2.flag(pkg::component::add_to_apps, true);
		CPPUNIT_ASSERT(test1 == test2);

		test1.flag(pkg::component::look_at, false);
		CPPUNIT_ASSERT(test1 != test2);
		test2.flag(pkg::component::look_at, false);
		CPPUNIT_ASSERT(test1 == test2);

		// Double check assignment
		test3 = test2;
		CPPUNIT_ASSERT_EQUAL(test2, test3);
	}

	/**
	 * Test parsing of a list of components
	 */
	void test_parse_list()
	{
		std::vector<pkg::component> components;
		std::string compstr("Apps.Misc.MyApp1 (Moveable LookAt), System.Stuff.Thingy , Manuals.MyAppDoc (Moveable)");
		CPPUNIT_ASSERT_NO_THROW(pkg::parse_component_list(compstr.begin(), compstr.end(), &components));

		CPPUNIT_ASSERT(components.size() == 3);

		CPPUNIT_ASSERT(components[0].name() == "Apps.Misc.MyApp1");
		CPPUNIT_ASSERT_EQUAL(components[0].flag(pkg::component::moveable), true);
		CPPUNIT_ASSERT_EQUAL(components[0].flag(pkg::component::look_at), true);
		CPPUNIT_ASSERT_EQUAL(components[0].flag(pkg::component::run), false);
		CPPUNIT_ASSERT_EQUAL(components[0].flag(pkg::component::add_to_apps), false);

		CPPUNIT_ASSERT(components[1].name() == "System.Stuff.Thingy");
		CPPUNIT_ASSERT_EQUAL(components[1].flag(pkg::component::moveable), false);
		CPPUNIT_ASSERT_EQUAL(components[1].flag(pkg::component::look_at), false);
		CPPUNIT_ASSERT_EQUAL(components[1].flag(pkg::component::run), false);
		CPPUNIT_ASSERT_EQUAL(components[1].flag(pkg::component::add_to_apps), false);

		CPPUNIT_ASSERT(components[2].name() == "Manuals.MyAppDoc");
		CPPUNIT_ASSERT_EQUAL(components[2].flag(pkg::component::moveable), true);
		CPPUNIT_ASSERT_EQUAL(components[2].flag(pkg::component::look_at), false);
		CPPUNIT_ASSERT_EQUAL(components[2].flag(pkg::component::run), false);
		CPPUNIT_ASSERT_EQUAL(components[2].flag(pkg::component::add_to_apps), false);
	}

	/**
	 * Test writing and reading of components
	 */
	void test_io()
	{
		const char *TestFile = "<PMUnitTests$Dir>.results.CompTest";
		pkg::component test1("Apps.Misc.!PackIt (Moveable)");
		pkg::component test2("Apps.Misc.!DocView (Moveable LookAt)");
		pkg::component test3("System.Modules.CleverMod");

		std::ofstream out(TestFile);
		CPPUNIT_ASSERT_MESSAGE("Unable to create output file", out.good());
		out << test1 << test2 << test3;
		out.close();

		std::ifstream in(TestFile);
		CPPUNIT_ASSERT_MESSAGE("Unable to open input file", in.good());
		pkg::component check1, check2, check3;
		in >> check1 >> check2 >> check3;

		CPPUNIT_ASSERT_EQUAL(test1, check1);
		CPPUNIT_ASSERT_EQUAL(test2, check2);
		CPPUNIT_ASSERT_EQUAL(test2, check2);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ComponentTest );
