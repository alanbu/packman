
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"

#include "look_at.h"
#include "fileutils.h"

// Dummy application names used in tests
static const char *test_app = "HostFS:$.Dev.!GCC";
static const char *test_not_app = "HostFS:$.Dev.!Fred"; 
static const char *test_quiz_app = "HostFS:$.Games.!Quiz";
static const char *boot_drive_app = "Boot:^.!PackMan";
static const char *boot_drive_quiz_app = "Boot:^.Diversions.!Quiz";

// Expand boot drive app to full pathname
static std::string test_boot_drive_app = canonicalise(boot_drive_app);
static std::string test_boot_drive_quiz_app = canonicalise(boot_drive_quiz_app);

/**
 * Test fixture to test the look_at class used for reading/writing
 * The lookup options for RISC OS
 */
class LookAtTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( LookAtTest );

	CPPUNIT_TEST( test_path_name );
	CPPUNIT_TEST( test_section_presence );
	CPPUNIT_TEST( test_contains );
	CPPUNIT_TEST( test_add );
	CPPUNIT_TEST( test_remove );
	CPPUNIT_TEST( test_replace );

	CPPUNIT_TEST_SUITE_END();

private:
	look_at_options *_look_at;

public:
	/**
	 * Test fixture setup override
	 */
	void setUp()
	{
		_look_at = new look_at_options();
	}

	/**
	 * Text fixture tear down override
	 */
	void tearDown()
	{
		delete _look_at;
	}

	/**
	 * Test the path name is what we expect
	 */
	void test_path_name()
	{
		CPPUNIT_ASSERT(_look_at->path_name() == "<Choices$Write>.Boot.Desktop");
	}

	/**
	 * Test if presence/absense of look at section
	 */
	void test_section_presence()
	{
		_look_at->use_test_path_name("<PMUnitTests$Dir>.data.Desktop1");
		_look_at->rollback(); // Load test file
		CPPUNIT_ASSERT(!_look_at->has_section());

		_look_at->use_test_path_name("<PMUnitTests$Dir>.data.Desktop2");
		_look_at->rollback(); // Load test file
		CPPUNIT_ASSERT(_look_at->has_section());
	}

	/**
	 * Test contains method
	 */
	void test_contains()
	{
		// Check file which as no look at section
		_look_at->use_test_path_name("<PMUnitTests$Dir>.data.Desktop1");
		_look_at->rollback(); // Load test file

		CPPUNIT_ASSERT(!_look_at->contains(test_app));

		// Check file with one entry
		_look_at->use_test_path_name("<PMUnitTests$Dir>.data.Desktop2");
		_look_at->rollback(); // Load test file
		CPPUNIT_ASSERT(_look_at->contains(test_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_not_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_boot_drive_app));

		// Check file with two entries
		_look_at->use_test_path_name("<PMUnitTests$Dir>.data.Desktop3");
		_look_at->rollback(); // Load test file

		CPPUNIT_ASSERT(_look_at->contains(test_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_not_app));
		CPPUNIT_ASSERT(_look_at->contains(test_boot_drive_app));

		CPPUNIT_ASSERT(_look_at->contains_raw(boot_drive_app));
	}

	void test_add()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop1";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopAD";

		// Initialise test file
		copy_file(data_file, test_file);
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_look_at->use_test_path_name(test_file);
		_look_at->rollback(); // Load test file
		
		// Add first item
		CPPUNIT_ASSERT(_look_at->add(test_app));
		CPPUNIT_ASSERT(_look_at->contains(test_app));
		CPPUNIT_ASSERT(_look_at->modified());

		// Save it
		_look_at->commit();
		CPPUNIT_ASSERT(!_look_at->modified());
		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop2"));

		CPPUNIT_ASSERT(_look_at->add(test_boot_drive_app));
		CPPUNIT_ASSERT(_look_at->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_look_at->contains(test_app));
		CPPUNIT_ASSERT(_look_at->modified());
		_look_at->commit();

		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop3"));
	}

	void test_remove()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop3";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopRM";

		// Initialise test file
		CPPUNIT_ASSERT_NO_THROW(copy_file(data_file, test_file));
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_look_at->use_test_path_name(test_file);
		CPPUNIT_ASSERT_NO_THROW(_look_at->rollback()); // Load test file

		// Remove last item
		CPPUNIT_ASSERT(_look_at->remove(test_boot_drive_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_look_at->contains(test_app));
		CPPUNIT_ASSERT(_look_at->modified());
		_look_at->commit();
		CPPUNIT_ASSERT(!_look_at->modified());

		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop2"));


		// Remove first item
		CPPUNIT_ASSERT(_look_at->remove(test_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_app));
		CPPUNIT_ASSERT(_look_at->modified());

		// Save it
		_look_at->commit();
		CPPUNIT_ASSERT(!_look_at->modified());
		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop4"));
	}

	void test_replace()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop3";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopMV";

		// Initialise test file
		CPPUNIT_ASSERT_NO_THROW(copy_file(data_file, test_file));
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_look_at->use_test_path_name(test_file);
		CPPUNIT_ASSERT_NO_THROW(_look_at->rollback()); // Load test file

		// Test move on non-boot drive
		CPPUNIT_ASSERT(_look_at->replace(test_app, test_not_app));
		CPPUNIT_ASSERT(_look_at->modified());
		CPPUNIT_ASSERT(!_look_at->contains(test_app));
		CPPUNIT_ASSERT(_look_at->contains(test_not_app));

		// Test move on boot drive
		CPPUNIT_ASSERT(_look_at->replace(test_boot_drive_app, test_boot_drive_quiz_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_look_at->contains(test_boot_drive_quiz_app));
		CPPUNIT_ASSERT(_look_at->contains_raw(boot_drive_quiz_app));

		CPPUNIT_ASSERT_NO_THROW(_look_at->commit());
		CPPUNIT_ASSERT(compare_files("<PMUnitTests$Dir>.data.Desktop5", test_file));

		CPPUNIT_ASSERT_NO_THROW(_look_at->rollback()); // Reload from saved file
		CPPUNIT_ASSERT(_look_at->contains(test_not_app));
		CPPUNIT_ASSERT(_look_at->contains(test_boot_drive_quiz_app));

		// Move everything back
		CPPUNIT_ASSERT(_look_at->replace(test_boot_drive_quiz_app, test_boot_drive_app));
		CPPUNIT_ASSERT(_look_at->replace(test_not_app, test_app));
		CPPUNIT_ASSERT_NO_THROW(_look_at->commit());
		
		CPPUNIT_ASSERT(compare_files(data_file, test_file));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( LookAtTest );
